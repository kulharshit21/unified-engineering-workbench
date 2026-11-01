# Multicore CPU Parallel Benchmarking Methodology (Task D)

## 1. Concurrency Architecture & Thread Pool Design

To evaluate multi-core CPU scaling for parametric thermal FEA simulations, the workbench implements an autonomous, bounded worker thread pool (`src/core/parallel/ThreadPool.hpp`):
- **Bounded Worker Lifecycle**: Rather than repeatedly creating and destroying OS threads per batch (which incurs expensive Windows `CreateThread` overhead of ~50–100 µs per thread), worker threads are spawned once upon initialization and persist in a dormant state waiting on a `std::condition_variable`.
- **Thread Synchronization**: Synchronization is coordinated via `std::mutex` protecting an in-memory task queue `std::queue<std::function<void()>>`.
- **Graceful Destruction**: When the pool shuts down or resizes, a `stop` atomic flag is set and `notify_all()` awakens all worker threads to join cleanly without thread leaks or orphaned resources.

---

## 2. Amdahl's Law & Synchronization Overhead Modeling

According to Amdahl's Law, the maximum theoretical speedup $S_p$ of a computational program on $p$ parallel processors is bounded by the non-parallelizable serial fraction $s = (1 - f)$:

$$S_p = \frac{T_1}{T_p} = \frac{1}{(1 - f) + \frac{f}{p}}$$

In real-world desktop operating systems, multithreaded execution incurs non-negligible synchronization overheads:
$$T_p = (1 - f)T_1 + \frac{f T_1}{p} + T_\text{sync}(p)$$

Where $T_\text{sync}(p)$ accounts for:
1. **Thread Wakeup Latency**: Windows kernel context switches and condition variable unblocking latencies (~10–30 µs per event).
2. **Mutex Contention**: Multiple workers competing to pop tasks from the job queue.
3. **Cache Invalidation & False Sharing**: L1/L2 cache line bouncing when writing adjacent results in memory.
4. **Timer Quantization**: Default Windows OS scheduling timer resolution is typically 1.0 ms.

---

## 3. Empirical Benchmarking Protocol

To guarantee reproducible, publication-grade performance measurements:
1. **Monotonic High-Resolution Timing**: Elapsed durations are measured using `std::chrono::steady_clock`, which is guaranteed monotonic and unaffected by NTP time adjustments or daylight saving shifts.
2. **Deterministic PRNG Workloads**: Workloads are generated using a deterministic pseudorandom linear congruential generator initialized with fixed seed `42` (`BatchGenerator.hpp`), ensuring 100% identical computational inputs across both serial and parallel runs.
3. **Cache & Frequency Warm-up**: Prior to timing, a warm-up batch of 100 cases is executed. This primes CPU instruction and data caches, expands thread stacks, and allows the AMD Precision Boost algorithm to elevate CPU core frequencies from idle states.
4. **Five Repetitions & Median Filtering**: Every worker count ($p \in \{1, 2, 4, 8\}$) is executed across 5 independent repetitions. The **median** runtime is used as the primary benchmark metric to filter out transient background OS interrupts, Defender scans, and context switches.
5. **Numerical Determinism Verification**: Every parallel result is compared against the serial baseline:
   $$\Delta_\text{max} = \max_{m, i} |T_{m, i}^\text{serial} - T_{m, i}^\text{parallel}| \le 1.0 \times 10^{-8\circ}\text{C}$$

---

## 4. Workload Sizing: Standard vs. Extended Workloads

During the independent engineering audit (`docs/audits/FINAL_INDEPENDENT_AUDIT.md`), an important timing limitation was evaluated:
- **Standard Workload (1,000 cases, $N=80$)**: Executes in ~1.85 ms total serial time on modern AMD Zen 5 cores. Because 1.85 ms is close to the Windows OS scheduling quantum (~1.0 ms), synchronization overheads represent a significant fraction of total execution time, bounding 4-thread speedup to $\sim 2.2\times$.
- **Extended Workload (10,000 cases, $N=80$)**: Executes in ~20.4 ms serial time. At this duration, compute saturation heavily amortizes thread pool dispatch costs, allowing the 8-thread speedup to reach $\sim 4.4\times$ ($3.12\times$ on 4 threads).

Both workloads are retained, documented, and tested in [`tests/integration/test_parallel_batch.cpp`](../../tests/integration/test_parallel_batch.cpp) to provide complete transparency.
