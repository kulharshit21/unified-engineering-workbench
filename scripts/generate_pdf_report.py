#!/usr/bin/env python3
"""
Convert FINAL_TECHNICAL_REPORT.md to HTML and print to high-quality PDF
using Microsoft Edge headless engine.
"""

import os
import subprocess

md_path = 'docs/reports/FINAL_TECHNICAL_REPORT.md'
html_path = 'docs/reports/FINAL_TECHNICAL_REPORT.html'
pdf_path = 'docs/reports/FINAL_TECHNICAL_REPORT.pdf'

if not os.path.exists(md_path):
    print("Report markdown missing.")
    exit(1)

with open(md_path, 'r', encoding='utf-8') as f:
    md_content = f.read()

# Simple markdown to styled HTML conversion
import re

# Escape basic HTML
html_body = md_content

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
table_html = []
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
    if p_strip.startswith('<h') or p_strip.startswith('<table') or p_strip.startswith('<pre') or p_strip.startswith('<hr'):
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
        margin: 15mm 15mm 15mm 15mm;
    }}
    body {{
        font-family: 'Segoe UI', Arial, sans-serif;
        font-size: 9.5pt;
        line-height: 1.35;
        color: #222;
        background: #fff;
    }}
    h1 {{
        font-size: 15pt;
        color: #0d2d59;
        margin-top: 0;
        margin-bottom: 4px;
        border-bottom: 2px solid #0d2d59;
        padding-bottom: 4px;
        text-transform: uppercase;
        letter-spacing: 0.5px;
    }}
    h2 {{
        font-size: 12pt;
        color: #1a4d8c;
        margin-top: 14px;
        margin-bottom: 6px;
        border-bottom: 1px solid #ddd;
        padding-bottom: 2px;
        page-break-after: avoid;
    }}
    h3 {{
        font-size: 10.5pt;
        color: #2a3d54;
        margin-top: 10px;
        margin-bottom: 4px;
        page-break-after: avoid;
    }}
    p {{
        margin-top: 0;
        margin-bottom: 6px;
        text-align: justify;
    }}
    table.data-table {{
        width: 100%;
        border-collapse: collapse;
        margin: 8px 0;
        font-size: 8.5pt;
        page-break-inside: avoid;
    }}
    table.data-table th, table.data-table td {{
        border: 1px solid #b0b8c5;
        padding: 4px 6px;
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
        padding: 6px 8px;
        font-family: Consolas, monospace;
        font-size: 8pt;
        overflow-x: auto;
        margin: 6px 0;
        page-break-inside: avoid;
    }}
    code {{
        font-family: Consolas, monospace;
        font-size: 8.5pt;
        background-color: #f3f4f6;
        padding: 1px 3px;
        border-radius: 3px;
    }}
    hr {{
        border: 0;
        border-top: 1px solid #ccc;
        margin: 10px 0;
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
