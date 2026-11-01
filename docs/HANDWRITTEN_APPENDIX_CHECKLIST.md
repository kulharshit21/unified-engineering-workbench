# Handwritten Appendix Checklist & Verification Protocol (Task F)

> [!IMPORTANT]
> **Zero-Fabrication Disclosure**:
> In strict accordance with the PASS SCIENCE WORKS internship requirements and academic integrity rules, this repository **never generates fake digital handwriting or fabricates scanned physical notebook pages**. 
> The analytical derivation and numerical verifications are computed and validated natively in C++ (`BackpropagationSolver.cpp`), while the handwritten appendix is reserved for genuine physical candidate notes (maximum 2 pages).

---

## 1. Physical Handwriting Requirements Checklist (Maximum 2 Pages)

The student candidate will hand-write the following sections in their engineering notebook on physical paper (maximum two pages) and scan them at 300 DPI into `docs/appendix/HANDWRITTEN_APPENDIX_SCAN.pdf` prior to final submission:

- [ ] **Page 1: Problem Definition, Computation Graph & Multivariate Chain Rule**
  - [ ] Header: Project Title, Candidate Name, Candidate ID, Date
  - [ ] Block diagram of forward pass: $x \to z_1 \to h \to \hat{y} \to J$
  - [ ] Parameter definitions: $x=1.0, y=2.0, w=0.5, b=0, v=1, c=0, \eta=0.1, J=\frac{1}{2}(\hat{y}-y)^2$
  - [ ] First-principles derivation of activation derivative: $\frac{dh}{dz} = 1 - \tanh^2(z) = 1 - h^2$
  - [ ] Full chain rule derivations for $\frac{\partial J}{\partial c}, \frac{\partial J}{\partial v}, \frac{\partial J}{\partial b}, \frac{\partial J}{\partial w}$

- [ ] **Page 2: Step 0 Substitution, Parameter Updates & Central Finite Differences**
  - [ ] Forward evaluation: $z_1 = 0.50000000$, $h = 0.46211716$, $\hat{y} = 0.46211716$, residual $\hat{y}-y = -1.53788284$
  - [ ] Loss evaluation: $J = 1.18254182$
  - [ ] Explicit boxed values for each gradient:
    - [ ] $\frac{\partial J}{\partial c} = -1.53788284$
    - [ ] $\frac{\partial J}{\partial v} = -0.71068205$
    - [ ] $\frac{\partial J}{\partial b} = -1.20946448$
    - [ ] $\frac{\partial J}{\partial w} = -1.20946448$
  - [ ] Simultaneous parameter update equations with $\eta = 0.1$:
    - [ ] $w^{(1)} = 0.62094645, b^{(1)} = 0.12094645, v^{(1)} = 1.07106820, c^{(1)} = 0.15378828$
  - [ ] Central finite-difference verification table ($\epsilon = 1.0 \times 10^{-6}$, errors $\le 1.13 \times 10^{-10} < 10^{-5}$)

*(Note: In accordance with the assignment brief, no personal signature is required).*

---

## 2. Directory Placement
Scanned PDF file should be saved to:
- `docs/appendix/HANDWRITTEN_APPENDIX_SCAN.pdf`

The packaging script `scripts/package_msvc.cmd` automatically bundles this file into the final release ZIP.
