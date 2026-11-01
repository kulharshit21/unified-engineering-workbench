# Handwritten Appendix Checklist & Verification Protocol (Task F)

> [!IMPORTANT]
> **Zero-Fabrication Disclosure**:
> In strict accordance with the PASS SCIENCE WORKS internship requirements and academic integrity rules, this repository **never generates fake digital handwriting or fabricates scanned physical notebook pages**. 
> The analytical derivation and numerical verifications are computed and validated natively in C++ (`BackpropagationSolver.cpp`), while the handwritten appendix is reserved for genuine physical student notes.

---

## 1. Physical Handwriting Requirements Checklist

The student candidate will hand-write the following sections in their engineering notebook on physical paper and scan them at 300 DPI into `docs/handwritten/` prior to final PDF submission:

- [ ] **Page 1: Problem Definition & Computation Graph**
  - [ ] Title: *PASS Science Works — Task F: Backpropagation by Hand*
  - [ ] Candidate ID / Name / Date block
  - [ ] Block diagram of forward pass: $x \to z_1 \to h \to \hat{y} \to J$
  - [ ] Variable definition table ($x=1.0, y=2.0, w=0.5, b=0, v=1, c=0, \eta=0.1$)
  - [ ] Loss function definition: $J = \frac{1}{2}(\hat{y}-y)^2$

- [ ] **Page 2: Full Mathematical Derivation via Chain Rule**
  - [ ] Proof of $\frac{d}{dz}\tanh(z) = 1 - \tanh^2(z)$ from first principles ($\sinh / \cosh$)
  - [ ] Derivation of $\frac{\partial J}{\partial \hat{y}} = \hat{y} - y$
  - [ ] Derivation of $\frac{\partial J}{\partial v} = (\hat{y}-y)h$
  - [ ] Derivation of $\frac{\partial J}{\partial c} = \hat{y}-y$
  - [ ] Derivation of $\frac{\partial J}{\partial h} = (\hat{y}-y)v$
  - [ ] Derivation of $\frac{\partial J}{\partial z_1} = (\hat{y}-y)v(1-h^2)$
  - [ ] Derivation of $\frac{\partial J}{\partial w} = (\hat{y}-y)v(1-h^2)x$
  - [ ] Derivation of $\frac{\partial J}{\partial b} = (\hat{y}-y)v(1-h^2)$

- [ ] **Page 3: Step 0 Manual Numerical Substitution**
  - [ ] Forward evaluation: $z_1 = 0.50000000$, $h = 0.46211716$, $\hat{y} = 0.46211716$
  - [ ] Loss evaluation: $J = 1.18255182$
  - [ ] Explicit boxed values for each gradient:
    - [ ] $\frac{\partial J}{\partial v} = -0.71068193$
    - [ ] $\frac{\partial J}{\partial c} = -1.53788284$
    - [ ] $\frac{\partial J}{\partial w} = -1.20946440$
    - [ ] $\frac{\partial J}{\partial b} = -1.20946440$

- [ ] **Page 4: Step 1 Parameter Update & Finite-Difference Verification**
  - [ ] Simultaneous update equations with $\eta = 0.1$:
    - [ ] $w^{(1)} = 0.62094644$
    - [ ] $b^{(1)} = 0.12094644$
    - [ ] $v^{(1)} = 1.07106819$
    - [ ] $c^{(1)} = 0.15378828$
  - [ ] Central difference verification formula:
    $$\frac{J(\theta + \epsilon) - J(\theta - \epsilon)}{2\epsilon}$$
  - [ ] Comparison table proving $|\nabla_\text{ana} - \nabla_\text{num}| < 10^{-10}$ across all 4 parameters.
  - [ ] Candidate signature and date.

---

## 2. Directory Placement
Scanned PDF/JPEG files should be saved to:
- `docs/handwritten/backpropagation_derivation_page1.pdf` (or `.png`)
- `docs/handwritten/backpropagation_derivation_page2.pdf` (or `.png`)
- `docs/handwritten/backpropagation_derivation_page3.pdf` (or `.png`)
- `docs/handwritten/backpropagation_derivation_page4.pdf` (or `.png`)

The compilation script `scripts/package_msvc.cmd` automatically bundles this directory into the final release `.zip`.
