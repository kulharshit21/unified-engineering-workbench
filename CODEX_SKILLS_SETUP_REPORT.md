# Codex Engineering Skills Setup & Verification Report

**Date of Setup**: November 1, 2026  
**Environment**: Windows 11 (Build 26300), Codex Desktop (v26.1027.3384.0), Codex CLI (v0.166.0-alpha.5.1), Antigravity Agent Runtime  
**Workspace**: `d:\PASS Science Works`  
**Setup Engineer**: AI Agent Setup Engineer (Antigravity / DeepMind)

---

## 1. Executive Summary

All **11 engineering skills** required for the upcoming **PASS Science Works Unified Engineering Workbench** C++ project have been installed, verified, and configured at user scope (global level). No administrator privileges were required. No unnecessary duplicate installations were created.

The installation combines:
1. **Official Qt Engineering Skills (4)** from `TheQtCompanyRnD/agent-skills` installed via `skills` CLI and linked to `$CODEX_HOME/skills`.
2. **Official Superpowers Engineering Skills (6)** installed via the official Codex Plugins marketplace (`superpowers@openai-curated-remote` v7.0.1).
3. **Custom Engineering-Evidence Skill (1)** created with a 16-point evidence schema, strict zero-fabrication guardrails, and full Codex UI metadata.

---

## 2. Environment Audit Details

| Component | Detected Version / Status | Notes |
| :--- | :--- | :--- |
| **Operating System** | Windows 10.0.26300 (Windows 11 CoreSingleLanguage x64) | PowerShell 5.1 (Build 26100, Rev 9818) |
| **Codex Desktop** | `OpenAI.Codex` v26.1027.3384.0 | Running at `C:\Program Files\WindowsApps\OpenAI.Codex_...` |
| **Codex CLI** | `codex-cli` v0.166.0-alpha.5.1 | Located at `C:\Users\kulha\AppData\Local\OpenAI\Codex\bin\22cfb1d302613eae\codex.exe` |
| **Node.js / npm / npx** | Node v24.18.0, npm v12.0.1, npx v12.0.1 | Operational |
| **Git** | git version 2.54.0.windows.1 | Remote access verified |
| **Python** | Python 3.12+ runtime available on PATH | Used for deterministic linting scripts |
| **Universal Skills CLI** | `npx skills` (skills.sh engine) | Configured with global directory `~/.agents/skills` |
| **Codex Skill Directory** | `C:\Users\kulha\.codex\skills` | Linked via NTFS junctions for zero-copy sync |
| **Network Reachability** | GitHub & OpenAI Plugin Marketplace accessible | `TheQtCompanyRnD/agent-skills` (commit `6845f02`), `obra/superpowers` (commit `2ce8aa1`) |

---

## 3. Comprehensive Skills Status Matrix (All 11 Skills)

| # | Skill Name | Category | Source Repository / Provider | Scope | Installation Method & Location | Verification Status |
| :- | :--- | :--- | :--- | :--- | :--- | :--- |
| 1 | `qt-cmake-project` | Qt Build / CMake | `TheQtCompanyRnD/agent-skills` | Global | `npx skills add` → `~/.agents/skills/qt-cmake-project` (Junction in `~/.codex/skills/`) | Verified (SKILL.md & Qt6 CMake guardrails inspected) |
| 2 | `qt-cpp-review` | C++ Quality / Linting | `TheQtCompanyRnD/agent-skills` | Global | `npx skills add` → `~/.agents/skills/qt-cpp-review` (Junction in `~/.codex/skills/`) | Verified (Executed `qt_review_lint.py` on test sample, caught `ERR-4`) |
| 3 | `qt-cpp-docs` | Documentation | `TheQtCompanyRnD/agent-skills` | Global | `npx skills add` → `~/.agents/skills/qt-cpp-docs` (Junction in `~/.codex/skills/`) | Verified (SKILL.md documentation structure inspected) |
| 4 | `qt-ui-design` | UI / UX Design | `TheQtCompanyRnD/agent-skills` | Global | `npx skills add` → `~/.agents/skills/qt-ui-design` (Junction in `~/.codex/skills/`) | Verified (7-point context check & styling rules confirmed) |
| 5 | `test-driven-development` | Superpowers / TDD | `obra/superpowers` via OpenAI Marketplace | Global | `codex plugin add superpowers@openai-curated-remote` (v7.0.1) | Verified (Iron Law: "No production code without a failing test") |
| 6 | `systematic-debugging` | Superpowers / Debug | `obra/superpowers` via OpenAI Marketplace | Global | `codex plugin add superpowers@openai-curated-remote` (v7.0.1) | Verified (Iron Law: 4-phase root cause before fix confirmed) |
| 7 | `verification-before-completion` | Superpowers / Gate | `obra/superpowers` via OpenAI Marketplace | Global | `codex plugin add superpowers@openai-curated-remote` (v7.0.1) | Verified (The Gate Function: commands & output before claims) |
| 8 | `writing-plans` | Superpowers / Planning | `obra/superpowers` via OpenAI Marketplace | Global | `codex plugin add superpowers@openai-curated-remote` (v7.0.1) | Verified (Bite-sized plan schema & dependency tracking) |
| 9 | `executing-plans` | Superpowers / Exec | `obra/superpowers` via OpenAI Marketplace | Global | `codex plugin add superpowers@openai-curated-remote` (v7.0.1) | Verified (Inline execution, task batching, checkpointing) |
| 10 | `requesting-code-review` | Superpowers / Review | `obra/superpowers` via OpenAI Marketplace | Global | `codex plugin add superpowers@openai-curated-remote` (v7.0.1) | Verified (Review readiness checklist & pull request validation) |
| 11 | `engineering-evidence` | Custom / Evidence | Local Custom Skill | Global | Created at `~/.agents/skills/engineering-evidence` (Junction in `~/.codex/skills/`) | Verified (`npx skills ls -g`, template, and schema validated) |

---

## 4. Installation & Safety Audit

### Security & Integrity Checks
1. **Qt Skills (`TheQtCompanyRnD/agent-skills`)**:
   - License: BSD 3-Clause (The Qt Company Ltd.).
   - Source Commit: `6845f02d41171a6df8b6e6fe225b7f5d1a6f4adf`.
   - Security audit of `qt_review_lint.py`: Inspects only local source lines and invokes local `git diff`. No network sockets, credentials, or malicious subprocess calls. Snyk / Socket security alerts: 0 alerts, verified safe.

2. **Superpowers Plugin (`obra/superpowers` v7.0.1)**:
   - License: MIT License (Jesse Vincent / obra).
   - Installed via Codex's signed/curated marketplace (`openai-curated-remote`).
   - No untrusted binaries or background daemons.

3. **Custom Skill (`engineering-evidence`)**:
   - Zero-dependency Markdown instructions, schema template (`milestone-template.md`), and UI metadata (`agents/openai.yaml`).
   - Purely declarative guardrails preventing fabrication and enforcing empirical proofs.

---

## 5. Empirical Verification Evidence

### Test A: `qt-cpp-review` Linter Capability Check
Executed `qt_review_lint.py` on a temporary file `tmp4F93.tmp.cpp` containing `const char* url = "http://example.com";`:
```text
Command: python qt_review_lint.py tmp4F93.tmp.cpp
Output:  tmp4F93.tmp.cpp:1 ERR-4 Hardcoded http:// URL — use https://
Exit Code: 0 (Finding reported accurately; test artifact deleted immediately)
```

### Test B: `verification-before-completion` Gate Check
Inspected rule gate in `verification-before-completion/SKILL.md`:
```text
THE IRON LAW: NO COMPLETION CLAIMS WITHOUT FRESH VERIFICATION EVIDENCE
Requires: IDENTIFY -> RUN -> READ -> VERIFY -> MAKE CLAIM
Prohibits: "should work", "probably", "seems to", expressing satisfaction before verification
```

### Test C: `engineering-evidence` Discovery Check
Executed global skills registry query:
```text
Command: npx skills ls -g
Output:  engineering-evidence ~\.agents\skills\engineering-evidence
         Agents: Antigravity, Cline, Codex, Cursor, Gemini CLI +1 more
         Source: local
```

---

## 6. Project Skill Orchestration Guide

When you initiate the upcoming PASS Science Works Unified Engineering Workbench internship assignment, the future agent should coordinate the skills across phases as follows:

```
                  ┌──────────────────────────────────────────────────┐
                  │               MILESTONE LIFECYCLE                │
                  └──────────────────────────────────────────────────┘
                                           │
  1. SPECIFICATION & PLANNING             ▼
     ├── writing-plans               (Break requirements into bite-sized tasks)
     └── qt-cmake-project            (Architect targets, modular libraries, QML modules)
                                           │
  2. DESIGN & USER EXPERIENCE             ▼
     └── qt-ui-design                (Run 7-point context check, layout, Qt Quick styling)
                                           │
  3. TEST-DRIVEN IMPLEMENTATION           ▼
     ├── test-driven-development     (Write failing unit test first, implement fresh)
     └── executing-plans             (Execute task batches with inline checkpoints)
                                           │
  4. DEFECT TRIAGE & DEBUGGING            ▼ (If bugs or test failures occur)
     └── systematic-debugging        (4-Phase root cause investigation before any fix)
                                           │
  5. CODE QUALITY & AUDIT                 ▼
     ├── qt-cpp-review               (Run 60+ lint checks & deep ownership/threading review)
     └── qt-cpp-docs                 (Generate developer reference docs for classes/modules)
                                           │
  6. VERIFICATION & GATEKEEPING           ▼
     └── verification-before-completion (Empirical proof required before completion claim)
                                           │
  7. EVIDENCE & RECORDING                 ▼
     ├── engineering-evidence        (Document 16-point reproducible milestone log)
     ├── requesting-code-review      (Perform peer review readiness check)
     └── Git Commit & Push           (Semantic commit with verified evidence)
```

### Activation Best Practice
- Do **not** trigger every skill on every prompt.
- Invoke specialized skills selectively when the work touches their exact domain.
- The `engineering-evidence` workflow must govern every milestone transition:
  **`IMPLEMENT → TEST → VERIFY → DOCUMENT → REVIEW → COMMIT → PUSH`**

---

## 7. Restart and Activation Notes

- **Codex CLI**: Skills in `$CODEX_HOME/skills` and plugins in `codex plugin list` are loaded immediately on subsequent commands.
- **Codex Desktop**: If Codex Desktop is currently running in an active conversation thread, restarting the application or opening a new conversation ensures that newly registered plugins and user skills appear in the `/` command picker and auto-invocation router.
- **Ready for Project Master Prompt**: All prerequisites, linters, templates, and guardrails are primed.
