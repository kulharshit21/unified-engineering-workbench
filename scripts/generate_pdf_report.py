#!/usr/bin/env python3
"""
Convert FINAL_TECHNICAL_REPORT.md to styled HTML and print to high-quality PDF
using Microsoft Edge headless engine.
Includes full offline typesetting for mathematical formulas (zero raw LaTeX).
"""

import os
import re
import subprocess

md_path = 'docs/reports/FINAL_TECHNICAL_REPORT.md'
html_path = 'docs/reports/FINAL_TECHNICAL_REPORT.html'
pdf_path = 'docs/reports/FINAL_TECHNICAL_REPORT.pdf'

if not os.path.exists(md_path):
    print("Report markdown missing.")
    exit(1)

with open(md_path, 'r', encoding='utf-8') as f:
    md_content = f.read()

def parse_balanced_braces(s, start_idx):
    if start_idx >= len(s) or s[start_idx] != '{':
        return None, start_idx
    depth = 0
    for i in range(start_idx, len(s)):
        if s[i] == '{':
            depth += 1
        elif s[i] == '}':
            depth -= 1
            if depth == 0:
                return s[start_idx + 1:i], i + 1
    return None, start_idx

def replace_fractions(s):
    while True:
        idx = s.find(r'\frac')
        if idx == -1:
            break
        num, next_idx = parse_balanced_braces(s, idx + 5)
        if num is None:
            break
        den, end_idx = parse_balanced_braces(s, next_idx)
        if den is None:
            break
        frac_html = f'<span class="frac"><span class="num">{typeset_formula(num)}</span><span class="den">{typeset_formula(den)}</span></span>'
        s = s[:idx] + frac_html + s[end_idx:]
    return s

def replace_sqrts(s):
    while True:
        idx = s.find(r'\sqrt')
        if idx == -1:
            break
        content, end_idx = parse_balanced_braces(s, idx + 5)
        if content is None:
            break
        sqrt_html = f'&radic;({typeset_formula(content)})'
        s = s[:idx] + sqrt_html + s[end_idx:]
    return s

def typeset_formula(tex):
    """Convert LaTeX math syntax into clean, styled HTML and Unicode."""
    s = tex.strip()
    
    # 1. Matrices: \begin{bmatrix} ... \end{bmatrix}
    def bmatrix_repl(m):
        rows = m.group(1).strip().split(r'\\')
        row_htmls = []
        for r in rows:
            cells = [typeset_formula(c.strip()) for c in r.split('&')]
            row_htmls.append('<tr>' + ''.join(f'<td>{c}</td>' for c in cells) + '</tr>')
        table = '<table class="matrix">' + ''.join(row_htmls) + '</table>'
        return f'<span class="bracket">[</span>{table}<span class="bracket">]</span>'
    s = re.sub(r'\\begin\{bmatrix\}(.*?)\\end\{bmatrix\}', bmatrix_repl, s, flags=re.DOTALL)

    # 2. Fractions & Sqrts with balanced braces
    s = replace_fractions(s)
    s = replace_sqrts(s)

    # 3. Text & Formatting
    s = re.sub(r'\\text\{([^}]+)\}', r'<span>\1</span>', s)
    s = re.sub(r'\\mathbf\{([^}]+)\}', r'<b>\1</b>', s)
    s = re.sub(r'\\boldsymbol\{([^}]+)\}', r'<b>\1</b>', s)
    s = re.sub(r'\\vec\{([^}]+)\}', r'<b>\1</b>', s)
    s = re.sub(r'\\hat\{([a-zA-Z])\}', r'\1&#770;', s)

    # 4. Evaluation bar: \Bigg|_{\boldsymbol{\theta}^{(k)}}
    s = re.sub(r'\\Bigg\|_\{([^{}]+)\}', r'<span style="font-size:14pt;vertical-align:middle;">|</span><sub>\1</sub>', s)

    # 5. Greek symbols & operators
    replacements = [
        (r'\\Longleftrightarrow', '&hArr;'),
        (r'\\longrightarrow', '&rarr;'),
        (r'\\partial', '&part;'),
        (r'\\nabla', '&nabla;'),
        (r'\\theta', '&theta;'),
        (r'\\delta', '&delta;'),
        (r'\\eta', '&eta;'),
        (r'\\epsilon', '&epsilon;'),
        (r'\\xi', '&xi;'),
        (r'\\cdot', '&middot;'),
        (r'\\times', '&times;'),
        (r'\\le', '&le;'),
        (r'\\ge', '&ge;'),
        (r'\\approx', '&asymp;'),
        (r'\\in', '&isin;'),
        (r'\\quad', '&nbsp;&nbsp;&nbsp;&nbsp;'),
        (r'\\,', '&nbsp;'),
        (r'\^\\circ', '&deg;'),
        (r'\^2', '<sup>2</sup>'),
        (r'\^e', '<sup>e</sup>'),
        (r'\^\*', '<sup>*</sup>'),
        (r'_e', '<sub>e</sub>'),
        (r'_1', '<sub>1</sub>'),
        (r'_2', '<sub>2</sub>'),
        (r'_0', '<sub>0</sub>'),
        (r'_L', '<sub>L</sub>'),
        (r'_i', '<sub>i</sub>'),
        (r'_N', '<sub>N</sub>'),
        (r'_\text\{mid\}', '<sub>mid</sub>'),
        (r'_\text\{exact\}', '<sub>exact</sub>'),
        (r'_\text\{FEA\}', '<sub>FEA</sub>'),
        (r'_\text\{min\}', '<sub>min</sub>'),
        (r'_\text\{max\}', '<sub>max</sub>'),
        (r'_\text\{mean\}', '<sub>mean</sub>'),
        (r'_\text\{ana\}', '<sub>ana</sub>'),
        (r'_\text\{num\}', '<sub>num</sub>'),
    ]
    for pattern, repl in replacements:
        s = re.sub(pattern, repl, s)

    # General sub/superscript regex
    s = re.sub(r'_\{([^{}]+)\}', r'<sub>\1</sub>', s)
    s = re.sub(r'\^\{([^{}]+)\}', r'<sup>\1</sup>', s)

    return s

def render_math(content):
    # First convert display math: $$ ... $$
    def display_repl(m):
        math_content = typeset_formula(m.group(1))
        return f'<div class="math-display">{math_content}</div>'
    content = re.sub(r'\$\$(.*?)\$\$', display_repl, content, flags=re.DOTALL)

    # Next convert inline math: $ ... $
    def inline_repl(m):
        math_content = typeset_formula(m.group(1))
        return f'<span class="math-inline">{math_content}</span>'
    content = re.sub(r'\$(.*?)\$', inline_repl, content)

    return content

# Preprocess math before markdown parsing
html_body = render_math(md_content)

# Headers
html_body = re.sub(r'^### (.*?)$', r'<h3>\1</h3>', html_body, flags=re.MULTILINE)
html_body = re.sub(r'^## (.*?)$', r'<h2>\1</h2>', html_body, flags=re.MULTILINE)
html_body = re.sub(r'^# (.*?)$', r'<h1>\1</h1>', html_body, flags=re.MULTILINE)

# Bold & Italic
html_body = re.sub(r'\*\*(.*?)\*\*', r'<strong>\1</strong>', html_body)
html_body = re.sub(r'\*(.*?)\*', r'<em>\1</em>', html_body)

# Tables
lines = html_body.split('\n')
in_table = False
new_lines = []

for line in lines:
    if line.strip().startswith('|') and line.strip().endswith('|'):
        if not in_table:
            in_table = True
            new_lines.append('<table class="data-table">')
        cells = [c.strip() for c in line.strip().split('|')[1:-1]]
        if all(re.match(r'^:?-+:?$', c) for c in cells):
            continue # separator line
        row_type = 'th' if '<th>' not in ''.join(new_lines[-5:]) and in_table and len(new_lines) > 0 and '<thead>' not in ''.join(new_lines[-2:]) else 'td'
        row_str = '<tr>' + ''.join(f'<{row_type}>{c}</{row_type}>' for c in cells) + '</tr>'
        new_lines.append(row_str)
    else:
        if in_table:
            in_table = False
            new_lines.append('</table>')
        new_lines.append(line)

if in_table:
    new_lines.append('</table>')

html_body = '\n'.join(new_lines)

# Code blocks
html_body = re.sub(r'```(.*?)```', r'<pre><code>\1</code></pre>', html_body, flags=re.DOTALL)

# Inline code
html_body = re.sub(r'`(.*?)`', r'<code>\1</code>', html_body)

# Horizontal rule
html_body = re.sub(r'^---$', r'<hr/>', html_body, flags=re.MULTILINE)

# Paragraphs
paragraphs = html_body.split('\n\n')
formatted_paragraphs = []
for p in paragraphs:
    p_strip = p.strip()
    if p_strip.startswith('<h') or p_strip.startswith('<table') or p_strip.startswith('<pre') or p_strip.startswith('<hr') or p_strip.startswith('<div class="math-display'):
        formatted_paragraphs.append(p_strip)
    elif p_strip:
        formatted_paragraphs.append(f'<p>{p_strip}</p>')

html_body = '\n'.join(formatted_paragraphs)

html_document = f"""<!DOCTYPE html>
<html>
<head>
<meta charset="utf-8">
<title>PASS Science Works — Final Technical Report</title>
<style>
    @page {{
        size: A4 portrait;
        margin: 14mm 14mm 14mm 14mm;
    }}
    body {{
        font-family: 'Segoe UI', Arial, sans-serif;
        font-size: 9pt;
        line-height: 1.35;
        color: #222;
        background: #fff;
    }}
    h1 {{
        font-size: 14pt;
        color: #0d2d59;
        margin-top: 0;
        margin-bottom: 4px;
        border-bottom: 2px solid #0d2d59;
        padding-bottom: 4px;
        text-transform: uppercase;
        letter-spacing: 0.5px;
    }}
    h2 {{
        font-size: 11.5pt;
        color: #1a4d8c;
        margin-top: 12px;
        margin-bottom: 5px;
        border-bottom: 1px solid #ddd;
        padding-bottom: 2px;
        page-break-after: avoid;
    }}
    h3 {{
        font-size: 10pt;
        color: #2a3d54;
        margin-top: 9px;
        margin-bottom: 3px;
        page-break-after: avoid;
    }}
    p {{
        margin-top: 0;
        margin-bottom: 5px;
        text-align: justify;
    }}
    table.data-table {{
        width: 100%;
        border-collapse: collapse;
        margin: 6px 0;
        font-size: 8pt;
        page-break-inside: avoid;
    }}
    table.data-table th, table.data-table td {{
        border: 1px solid #b0b8c5;
        padding: 3px 5px;
        text-align: left;
    }}
    table.data-table th {{
        background-color: #eaf1fa;
        color: #102a45;
        font-weight: bold;
    }}
    table.data-table tr:nth-child(even) {{
        background-color: #f8fafc;
    }}
    pre {{
        background-color: #f1f4f8;
        border: 1px solid #d0d7de;
        border-radius: 4px;
        padding: 5px 7px;
        font-family: Consolas, monospace;
        font-size: 7.5pt;
        overflow-x: auto;
        margin: 5px 0;
        page-break-inside: avoid;
    }}
    code {{
        font-family: Consolas, monospace;
        font-size: 8pt;
        background-color: #f3f4f6;
        padding: 1px 3px;
        border-radius: 3px;
    }}
    hr {{
        border: 0;
        border-top: 1px solid #ccc;
        margin: 8px 0;
    }}

    /* Mathematical Typesetting Styles */
    .math-display {{
        text-align: center;
        margin: 7px 0;
        font-family: 'Cambria Math', 'Georgia', 'Times New Roman', serif;
        font-size: 10pt;
        line-height: 1.5;
        color: #111;
    }}
    .math-inline {{
        font-family: 'Cambria Math', 'Georgia', 'Times New Roman', serif;
        font-size: 9.5pt;
        white-space: nowrap;
    }}
    .frac {{
        display: inline-block;
        vertical-align: middle;
        text-align: center;
        padding: 0 2px;
        font-size: 9pt;
    }}
    .frac .num {{
        display: block;
        border-bottom: 1px solid #222;
        padding-bottom: 1px;
    }}
    .frac .den {{
        display: block;
        padding-top: 1px;
    }}
    .matrix {{
        display: inline-table;
        vertical-align: middle;
        border-collapse: collapse;
        margin: 0 1px;
    }}
    .matrix td {{
        padding: 1px 4px !important;
        text-align: center !important;
        border: none !important;
        background: transparent !important;
    }}
    .bracket {{
        font-size: 15pt;
        vertical-align: middle;
        font-family: 'Cambria Math', serif;
    }}
</style>
</head>
<body>
{html_body}
</body>
</html>
"""

with open(html_path, 'w', encoding='utf-8') as f:
    f.write(html_document)

print(f"[PASS] Written HTML document: {html_path}")

# Run Microsoft Edge headless to print to PDF
edge_cmd = [
    r"C:\Program Files (x86)\Microsoft\Edge\Application\msedge.exe",
    "--headless",
    "--disable-gpu",
    "--no-pdf-header-footer",
    f"--print-to-pdf={os.path.abspath(pdf_path)}",
    os.path.abspath(html_path)
]

res = subprocess.run(edge_cmd, capture_output=True, text=True)
if res.returncode == 0 and os.path.exists(pdf_path):
    size_kb = os.path.getsize(pdf_path) / 1024
    print(f"[PASS] Successfully generated PDF: {pdf_path} ({size_kb:.1f} KB)")
else:
    print(f"[WARN] Edge print failed: {res.stderr}")
