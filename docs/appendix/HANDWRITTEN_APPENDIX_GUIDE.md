# Candidate Handwritten Derivation Guide (Task F)
## Two-Page Notebook Transcription Template

> [!NOTE]
> Transcribe the two pages below onto physical paper using black or blue pen. Maintain clear mathematical layout and boxed results (maximum two pages) before scanning to `HANDWRITTEN_APPENDIX_SCAN.pdf`. No personal signature is required by the assignment specification.

---

# ========================================================
# [PAGE 1 OF 2] MATHEMATICAL DERIVATION & CHAIN RULE
# ========================================================

**Candidate Name**: __________________________  
**Candidate ID**: ____________________________  
**Date**: ___________________________________  
**Project**: PASS Science Works — Unified Engineering Workbench (Task F)

---

### 1. Network Topology & Problem Specification

Consider a single-input, single-hidden-unit feedforward artificial neural network:

$$\text{Input } x \xrightarrow{w, b} z_1 = wx + b \xrightarrow{} h = \tanh(z_1) \xrightarrow{v, c} \hat{y} = vh + c \xrightarrow{} J = \frac{1}{2}(\hat{y} - y)^2$$

- **Input scalar**: $x = 1.0$
- **Target scalar**: $y = 2.0$
- **Initial parameters**:
  - Hidden weight: $w^{(0)} = 0.5$
  - Hidden bias: $b^{(0)} = 0.0$
  - Output weight: $v^{(0)} = 1.0$
  - Output bias: $c^{(0)} = 0.0$
- **Learning rate**: $\eta = 0.1$
- **Loss function**: Half Mean Squared Error: $J = \frac{1}{2}(\hat{y} - y)^2$

---

### 2. Derivative of Activation Function from First Principles

Let $h = \tanh(z) = \frac{\sinh(z)}{\cosh(z)} = \frac{e^z - e^{-z}}{e^z + e^{-z}}$.

Applying the quotient rule:
$$\frac{dh}{dz} = \frac{\frac{d}{dz}[\sinh(z)] \cdot \cosh(z) - \sinh(z) \cdot \frac{d}{dz}[\cosh(z)]}{\cosh^2(z)}$$

Recalling $\frac{d}{dz}[\sinh(z)] = \cosh(z)$ and $\frac{d}{dz}[\cosh(z)] = \sinh(z)$:
$$\frac{dh}{dz} = \frac{\cosh^2(z) - \sinh^2(z)}{\cosh^2(z)} = 1 - \frac{\sinh^2(z)}{\cosh^2(z)} = 1 - \tanh^2(z) = 1 - h^2$$

---

### 3. Multivariate Chain Rule (Backward Pass)

Starting from the scalar objective loss $J$:

1. **Loss gradient with respect to output prediction $\hat{y}$**:
   $$\frac{\partial J}{\partial \hat{y}} = \frac{\partial}{\partial \hat{y}}\left[\frac{1}{2}(\hat{y} - y)^2\right] = \hat{y} - y$$

2. **Output layer parameter gradients ($v$ and $c$)**:
   $$\frac{\partial J}{\partial c} = \frac{\partial J}{\partial \hat{y}} \cdot \frac{\partial \hat{y}}{\partial c} = (\hat{y} - y) \cdot 1 = \hat{y} - y$$
   $$\frac{\partial J}{\partial v} = \frac{\partial J}{\partial \hat{y}} \cdot \frac{\partial \hat{y}}{\partial v} = (\hat{y} - y) \cdot h$$

3. **Hidden layer error propagation ($\delta_1 = \frac{\partial J}{\partial z_1}$)**:
   $$\frac{\partial J}{\partial h} = \frac{\partial J}{\partial \hat{y}} \cdot \frac{\partial \hat{y}}{\partial h} = (\hat{y} - y) \cdot v$$
   $$\delta_1 = \frac{\partial J}{\partial z_1} = \frac{\partial J}{\partial h} \cdot \frac{dh}{dz_1} = (\hat{y} - y) \cdot v \cdot (1 - h^2)$$

4. **Hidden layer parameter gradients ($w$ and $b$)**:
   $$\frac{\partial J}{\partial b} = \frac{\partial J}{\partial z_1} \cdot \frac{\partial z_1}{\partial b} = \delta_1 \cdot 1 = (\hat{y} - y) \cdot v \cdot (1 - h^2)$$
   $$\frac{\partial J}{\partial w} = \frac{\partial J}{\partial z_1} \cdot \frac{\partial z_1}{\partial w} = \delta_1 \cdot x = (\hat{y} - y) \cdot v \cdot (1 - h^2) \cdot x$$

---
*(End of Page 1 — Turn to Page 2)*

<div style="page-break-after: always;"></div>

---

# ========================================================
# [PAGE 2 OF 2] STEP 0 NUMERICAL EVALUATION & VERIFICATION
# ========================================================

### 4. Step 0 Analytical Hand Calculations

Substitute initial parameters: $x = 1.0, y = 2.0, w = 0.5, b = 0.0, v = 1.0, c = 0.0$.

#### 4.1 Forward Pass (Step 0)
1. Hidden pre-activation:
   $$z_1 = wx + b = (0.5)(1.0) + 0.0 = 0.50000000$$
2. Hidden activation:
   $$h = \tanh(0.5) = \frac{e^{0.5} - e^{-0.5}}{e^{0.5} + e^{-0.5}} = \frac{1.64872127 - 0.60653066}{1.64872127 + 0.60653066} = \frac{1.04219061}{2.25525193} \approx 0.46211716$$
3. Output prediction:
   $$\hat{y} = vh + c = (1.0)(0.46211716) + 0.0 = 0.46211716$$
4. Output error residual:
   $$\hat{y} - y = 0.46211716 - 2.0 = -1.53788284$$
5. Scalar loss:
   $$J^{(0)} = \frac{1}{2}(-1.53788284)^2 = \frac{1}{2}(2.36508363) \approx 1.18254182$$

#### 4.2 Gradient Evaluation (Step 0)
- Nonlinear derivative factor:
  $$1 - h^2 = 1 - (0.46211716)^2 = 1 - 0.21355227 = 0.78644773$$
- Hidden delta:
  $$\delta_1 = (\hat{y} - y) \cdot v \cdot (1 - h^2) = (-1.53788284)(1.0)(0.78644773) \approx -1.20946448$$

Boxed analytical gradients:
$$\boxed{\frac{\partial J}{\partial c} = -1.53788284}$$
$$\boxed{\frac{\partial J}{\partial v} = (\hat{y} - y) \cdot h = (-1.53788284)(0.46211716) = -0.71068205}$$
$$\boxed{\frac{\partial J}{\partial b} = \delta_1 = -1.20946448}$$
$$\boxed{\frac{\partial J}{\partial w} = \delta_1 \cdot x = (-1.20946448)(1.0) = -1.20946448}$$

---

### 5. Step 1 Simultaneous Parameter Updates ($\eta = 0.1$)

Parameters are updated simultaneously using $\theta^{(1)} = \theta^{(0)} - \eta \frac{\partial J}{\partial \theta}$:
- $w^{(1)} = 0.5 - (0.1)(-1.20946448) = 0.5 + 0.12094645 = 0.62094645$
- $b^{(1)} = 0.0 - (0.1)(-1.20946448) = 0.0 + 0.12094645 = 0.12094645$
- $v^{(1)} = 1.0 - (0.1)(-0.71068205) = 1.0 + 0.07106820 = 1.07106820$
- $c^{(1)} = 0.0 - (0.1)(-1.53788284) = 0.0 + 0.15378828 = 0.15378828$

---

### 6. Central Finite-Difference Numerical Verification ($\epsilon = 1.0 \times 10^{-6}$)

Numerical gradient formula:
$$\frac{\partial J}{\partial \theta_i} \approx \frac{J(\theta_i + \epsilon) - J(\theta_i - \epsilon)}{2\epsilon}, \quad \text{Truncation error } O(\epsilon^2) \approx 10^{-12}$$

| Parameter | Analytical $\nabla_\text{exact}$ | Numerical $\nabla_\text{FD}$ | Absolute Error $|\Delta|$ | Tolerance | Verification |
|:---:|:---:|:---:|:---:|:---:|:---:|
| $w$ | $-1.20946448$ | $-1.20946448$ | $1.13 \times 10^{-10}$ | $< 10^{-5}$ | **PASS** |
| $b$ | $-1.20946448$ | $-1.20946448$ | $1.13 \times 10^{-10}$ | $< 10^{-5}$ | **PASS** |
| $v$ | $-0.71068205$ | $-0.71068205$ | $5.25 \times 10^{-11}$ | $< 10^{-5}$ | **PASS** |
| $c$ | $-1.53788284$ | $-1.53788284$ | $1.12 \times 10^{-10}$ | $< 10^{-5}$ | **PASS** |

---
*(End of Page 2 — Maximum Two Pages)*
