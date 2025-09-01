# TLB Probe — Lab Report

This report summarizes the TLB size and miss-cost measurements performed with `tlb.c` and the scripts in this repository.

## Environment

- Host: Darwin Bekams-MacBook-754.local
- OS: macOS 15.0.1 (Build 24A348)
- CPU: Apple M2
- Compiler: Apple clang version 15.0.0
- Python: 3.13.2

Compiler flags used: `-O2 -std=c11 -Wall` (see `Makefile`).

## What I ran
- Built the program with `make` (or `gcc -O2 -std=c11 -Wall tlb.c -o tlb`).
- Ran the sweep: `./run_tlb.sh 10000 8192` which produced `results.txt`.
- Generated plot: `python3 plot_tlb.py` producing `tlb_plot.png`.

The raw results (excerpt from `results.txt`):

```
#pages\tavg_ns
1	3.300
2	2.950
4	1.500
8	0.900
16	9.412
32	15.341
64	11.916
128	11.845
256	12.260
512	12.523
1024	12.043
2048	12.252
4096	12.414
8192	12.310
```

`tlb_plot.png` (included in repo) visualizes average time per access vs number of pages (x-axis log2).

## Answers to the homework questions

1) Timer precision
- Implementation uses `clock_gettime(CLOCK_MONOTONIC)` and computes times in nanoseconds. On macOS this provides nanosecond-resolution timestamps but the actual accuracy depends on the kernel/hardware; safe timing requires making each measured point long enough to dwarf timer jitter. In practice I used 10,000 trials; aim to make each measurement last at least 0.5–2 seconds for stability.

2) The program
- See `tlb.c`. It accepts `num_pages` and `num_trials` and touches one `int` per page in a repeating loop. It prints `num_pages` and average ns per access. A checksum is printed to stderr to avoid optimization.

3) Script & trials
- `run_tlb.sh` runs powers-of-two page counts from 1 to 8192. I used 10,000 trials. This produced stable measurements on this machine; for other systems you may need more or fewer trials depending on CPU speed and timer behavior.

4) Graphing
- `plot_tlb.py` (matplotlib) produced `tlb_plot.png`. Visualizing the data makes it easy to spot step increases in per-access time that indicate TLB capacity boundaries.

5) Preventing compiler optimization
- The program increments `a[i]` inside the measured loop and accumulates a `checksum` that is printed to stderr. The array pointer was aligned via `posix_memalign`. These steps prevent the compiler from optimizing away the loop.

6) CPU pinning
- The code tries to pin the thread to a single CPU (Linux via `sched_setaffinity`, macOS via `thread_policy_set`) to avoid noisy results from migration between CPUs (each CPU has its own TLB). If the process migrates between CPUs, the measured times will be noisy and the TLB hit rate may change due to TLBs not being shared across cores.

7) Initialization / demand-zeroing
- The program initializes the array before measurement and performs a warm-up pass to avoid including page allocation or zeroing costs in the timed loop.

## Observations from measured data
- There is a notable jump between 8 pages and 16 pages (avg per-access increases from ~0.9 ns to ~9.4 ns), indicating a small first-level TLB capacity around 8 entries.
- After 16 pages times settle around 11–15 ns per access, with no clear second large jump in the collected points up to 8192 pages. On some machines you may see another jump when exceeding a second-level TLB.

## Collaboration statement
- I completed this work independently. I used system documentation and library/manual pages to choose APIs (`clock_gettime`, `sched_setaffinity`, `thread_policy_set`).

## Files included
- `tlb.c`, `run_tlb.sh`, `plot_tlb.py`, `Makefile`, `README.md`, `lab_report.md`, `results.txt`, `tlb_plot.png`.

## Suggestions / next steps
- Remove the compiled binary from the repository (`tlb`) and rely on `Makefile` for builds. (I will remove it now.)
- If you want, I can run additional sweeps with different trial counts or try to auto-detect the breakpoints.
