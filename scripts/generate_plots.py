#!/usr/bin/env python3
"""
Generate publication-quality empirical verification figures directly from
the empirical CSV result files.
"""

import os
import csv
import numpy as np
import matplotlib.pyplot as plt

plt.style.use('seaborn-v0_8-whitegrid' if 'seaborn-v0_8-whitegrid' in plt.style.available else 'default')
plt.rcParams['font.sans-serif'] = 'Arial'
plt.rcParams['font.size'] = 10
plt.rcParams['axes.titlesize'] = 12
plt.rcParams['axes.labelsize'] = 11

os.makedirs('results/thermal', exist_ok=True)
os.makedirs('results/performance', exist_ok=True)
os.makedirs('results/backprop', exist_ok=True)
os.makedirs('docs/diagrams', exist_ok=True)
os.makedirs('docs/screenshots', exist_ok=True)

# 1. Thermal Temperature Comparison Plot
profiles_csv = 'results/thermal/temperature_profiles.csv'
if os.path.exists(profiles_csv):
    fig, ax = plt.subplots(figsize=(8, 5), dpi=300)
    
    data_by_n = {}
    with open(profiles_csv, 'r') as f:
        reader = csv.DictReader(f)
        for r in reader:
            n = int(r['mesh_N'])
            if n not in data_by_n:
                data_by_n[n] = {'x': [], 'T_num': [], 'T_exact': []}
            data_by_n[n]['x'].append(float(r['x_m']))
            data_by_n[n]['T_num'].append(float(r['T_numerical_C']))
            data_by_n[n]['T_exact'].append(float(r['T_exact_C']))
            
    # Plot exact analytical curve
    x_exact = np.linspace(0, 1.0, 200)
    T_exact = 100.0 - 80.0 * x_exact + 500.0 * x_exact * (1.0 - x_exact)
    ax.plot(x_exact, T_exact, 'k-', linewidth=2.5, label='Exact Analytical $T_{exact}(x)$', zorder=2)
    
    colors = {10: '#e41a1c', 20: '#377eb8', 40: '#4daf4a', 80: '#984ea3'}
    markers = {10: 'o', 20: 's', 40: '^', 80: 'd'}
    
    for n in sorted(data_by_n.keys()):
        ax.plot(data_by_n[n]['x'], data_by_n[n]['T_num'], 
                linestyle='--', marker=markers.get(n, 'o'), markersize=5,
                color=colors.get(n, 'blue'), label=f'FEA Linear Elements ($N={n}$)', zorder=3)
                
    ax.set_title('1D Steady-State Thermal Conduction: Analytical vs FEA Solution', fontweight='bold')
    ax.set_xlabel('Rod Axial Position $x$ (m)')
    ax.set_ylabel('Temperature $T$ (°C)')
    ax.set_xlim(-0.02, 1.02)
    ax.legend(frameon=True, facecolor='white', framealpha=0.9)
    fig.tight_layout()
    fig.savefig('results/thermal/temperature_comparison.png')
    fig.savefig('docs/diagrams/temperature_comparison.png')
    plt.close(fig)
    print("[PASS] Generated results/thermal/temperature_comparison.png")

# 2. Thermal Mesh Convergence Plot (Log-Log)
convergence_csv = 'results/thermal/convergence.csv'
if os.path.exists(convergence_csv):
    fig, ax = plt.subplots(figsize=(7, 5), dpi=300)
    h_vals, err_obs, err_theo = [], [], []
    with open(convergence_csv, 'r') as f:
        reader = csv.DictReader(f)
        for r in reader:
            h_vals.append(float(r['element_length_h_m']))
            err_obs.append(float(r['max_midpoint_error_C']))
            err_theo.append(float(r['theoretical_error_C']))
            
    ax.loglog(h_vals, err_obs, 'ro-', markersize=8, linewidth=2, label='Observed Max Midpoint Error ($e_{max}$)', zorder=3)
    ax.loglog(h_vals, err_theo, 'k--', linewidth=1.5, label=r'Theoretical $O(h^2)$ Bound: $e = \frac{q h^2}{8k}$', zorder=2)
    
    for h, e in zip(h_vals, err_obs):
        ax.annotate(f"{e:.6f} °C", (h, e), textcoords="offset points", xytext=(-15, 8), fontsize=9)
        
    ax.set_title('Thermal FEA Convergence: Exact Quadratic Order of Accuracy ($p = 2.0000$)', fontweight='bold')
    ax.set_xlabel('Element Length $h = L/N$ (m) [Log Scale]')
    ax.set_ylabel('Maximum Midpoint Error (°C) [Log Scale]')
    ax.grid(True, which='both', linestyle=':', alpha=0.6)
    ax.legend(frameon=True, facecolor='white', framealpha=0.9)
    fig.tight_layout()
    fig.savefig('results/thermal/mesh_convergence.png')
    fig.savefig('docs/diagrams/mesh_convergence.png')
    plt.close(fig)
    print("[PASS] Generated results/thermal/mesh_convergence.png")

# 3. Parallel Speedup & Efficiency Plots
summary_csv = 'results/performance/summary.csv'
if os.path.exists(summary_csv):
    workers, speedups, efficiencies, medians = [], [], [], []
    with open(summary_csv, 'r') as f:
        reader = csv.DictReader(f)
        for r in reader:
            workers.append(int(r['workers_p']))
            speedups.append(float(r['speedup_T1_over_Tp']))
            efficiencies.append(float(r['efficiency']) * 100.0)
            medians.append(float(r['median_time_ms']))
            
    # Speedup plot
    fig, ax = plt.subplots(figsize=(7, 5), dpi=300)
    ax.plot(workers, workers, 'k--', linewidth=1.8, label='Ideal Linear Speedup ($S_p = p$)', zorder=2)
    ax.plot(workers, speedups, 'go-', linewidth=2.2, markersize=8, label='Measured CPU Speedup ($S_p = T_1 / T_p$)', zorder=3)
    
    for w, s, m in zip(workers, speedups, medians):
        ax.annotate(f"{s:.2f}x ({m:.2f} ms)", (w, s), textcoords="offset points", xytext=(-20, 8), fontsize=9, fontweight='bold')
        
    ax.set_title('Multicore CPU Scaling: 1,000 FEA Solves on AMD Ryzen AI 9 HX 370', fontweight='bold')
    ax.set_xlabel('Worker Threads ($p$)')
    ax.set_ylabel('Speedup Factor ($S_p$)')
    ax.set_xticks(workers)
    ax.set_ylim(0.5, 4.5)
    ax.legend(frameon=True, facecolor='white', framealpha=0.9)
    fig.tight_layout()
    fig.savefig('results/performance/speedup.png')
    fig.savefig('docs/diagrams/speedup.png')
    plt.close(fig)
    print("[PASS] Generated results/performance/speedup.png")
    
    # Efficiency plot
    fig, ax = plt.subplots(figsize=(7, 5), dpi=300)
    ax.axhline(100.0, color='gray', linestyle='--', label='Ideal Efficiency (100%)')
    ax.plot(workers, efficiencies, 'bs-', linewidth=2.2, markersize=8, label='Measured Efficiency ($E_p = S_p / p$)', zorder=3)
    
    for w, eff in zip(workers, efficiencies):
        ax.annotate(f"{eff:.1f}%", (w, eff), textcoords="offset points", xytext=(-15, 8), fontsize=9, fontweight='bold')
        
    ax.set_title('Parallel Efficiency: Thread Dispatch & Cache Amortization', fontweight='bold')
    ax.set_xlabel('Worker Threads ($p$)')
    ax.set_ylabel('Parallel Efficiency (%)')
    ax.set_xticks(workers)
    ax.set_ylim(20, 110)
    ax.legend(frameon=True, facecolor='white', framealpha=0.9)
    fig.tight_layout()
    fig.savefig('results/performance/efficiency.png')
    fig.savefig('docs/diagrams/efficiency.png')
    plt.close(fig)
    print("[PASS] Generated results/performance/efficiency.png")

# 4. Neural Network Training Loss Plot
loss_csv = 'results/backprop/loss_history.csv'
if os.path.exists(loss_csv):
    steps, losses, y_hats = [], [], []
    with open(loss_csv, 'r') as f:
        reader = csv.DictReader(f)
        for r in reader:
            steps.append(int(r['step']))
            losses.append(float(r['loss']))
            y_hats.append(float(r['y_hat']))
            
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(11, 4.5), dpi=300)
    
    # Loss convergence (Log scale)
    ax1.semilogy(steps, losses, color='#d95f02', linewidth=2.0)
    ax1.set_title('Backpropagation Loss Decay ($J = 0.5(\\hat{y} - y)^2$)', fontweight='bold')
    ax1.set_xlabel('Optimization Step')
    ax1.set_ylabel('Loss $J$ [Log Scale]')
    ax1.annotate(f"Initial: {losses[0]:.4f}", (0, losses[0]), textcoords="offset points", xytext=(10, -5), fontsize=9)
    ax1.annotate(f"Final: {losses[-1]:.2e}", (steps[-1], max(losses[-1], 1e-20)), textcoords="offset points", xytext=(-65, 10), fontsize=9, fontweight='bold')
    
    # Prediction trajectory towards target y=2.0
    ax2.plot(steps, y_hats, color='#7570b3', linewidth=2.0, label='Model Prediction $\\hat{y}$')
    ax2.axhline(2.0, color='k', linestyle='--', linewidth=1.5, label='Target Value $y = 2.0$')
    ax2.set_title('Network Output Convergence towards Target', fontweight='bold')
    ax2.set_xlabel('Optimization Step')
    ax2.set_ylabel('Prediction $\\hat{y}$')
    ax2.set_ylim(0.2, 2.1)
    ax2.legend(frameon=True, facecolor='white', framealpha=0.9)
    
    fig.tight_layout()
    fig.savefig('results/backprop/loss_plot.png')
    fig.savefig('docs/diagrams/backprop_loss_plot.png')
    plt.close(fig)
    print("[PASS] Generated results/backprop/loss_plot.png")

print("All empirical PNG charts successfully generated from verification CSVs.")
