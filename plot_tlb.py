#!/usr/bin/env python3
# plot_tlb.py
import matplotlib.pyplot as plt
import numpy as np

data = np.loadtxt('results.txt', comments='#')
pages = data[:,0]
times = data[:,1]

plt.figure(figsize=(6,5))
plt.plot(pages, times, marker='o', color='orange')
plt.xscale('log', base=2)
plt.xlabel('Number Of Pages')
plt.ylabel('Time Per Access (ns)')
plt.title('TLB Size Measurement')
plt.grid(True, which='both', ls='--', alpha=0.3)
plt.tight_layout()
plt.savefig('tlb_plot.png', dpi=150)
print('Saved tlb_plot.png')
plt.show()
