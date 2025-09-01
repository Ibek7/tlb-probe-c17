TLB Probe
=========

Files:
- tlb.c: C program to measure average time per page access. Usage: ./tlb <num_pages> <num_trials>
- run_tlb.sh: Script to run `tlb` across powers of two page counts. Usage: ./run_tlb.sh [trials] [max_pages]
- plot_tlb.py: Python script to plot `results.txt` and save `tlb_plot.png`.

Build:
  gcc -O2 -std=c11 -Wall tlb.c -o tlb -lrt

Run example:
  chmod +x run_tlb.sh
  ./run_tlb.sh 10000 8192
  python3 plot_tlb.py

Notes:
- On macOS, link flags differ; remove -lrt if not available.
- To pin to a CPU on Linux, the program attempts to set affinity. On macOS, `thread_policy_set` is used.
- The program prints a checksum to stderr to avoid compiler optimization removing the main loop.

Lab report should answer the seven questions in the assignment and include hardware/software details, compiler options, and a collaboration statement.
