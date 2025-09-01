#!/usr/bin/env python3
"""Simple detector for TLB breakpoints from results.txt
Usage: python3 detect_breaks.py results.txt
"""
import sys
import math

fn = sys.argv[1] if len(sys.argv) > 1 else 'results.txt'

pts = []
with open(fn, 'r') as f:
    for line in f:
        line = line.strip()
        if not line or line.startswith('#'):
            continue
        parts = line.split()
        try:
            p = int(parts[0])
            t = float(parts[1])
            pts.append((p, t))
        except Exception:
            continue

if not pts:
    print('No data points found in', fn)
    sys.exit(1)

breaks = []
for i in range(1, len(pts)):
    prev_p, prev_t = pts[i-1]
    p, t = pts[i]
    # Avoid division by tiny numbers: require either >=2x ratio or >=5 ns absolute increase
    ratio = (t / prev_t) if prev_t > 0 else float('inf')
    delta = t - prev_t
    if ratio >= 2.0 or delta >= 5.0:
        breaks.append((prev_p, p, prev_t, t, ratio, delta))

print('Detected breakpoints:')
if not breaks:
    print('  (none)')
else:
    for bp in breaks:
        prev_p, p, prev_t, t, ratio, delta = bp
        print(f'  Between {prev_p} and {p} pages: avg_ns {prev_t:.3f} -> {t:.3f} (ratio {ratio:.2f}, delta {delta:.3f} ns)')

if breaks:
    # infer TLB size as the last page count before the jump
    inferred = [b[0] for b in breaks]
    print('\nInferred TLB sizes (approx):', ', '.join(str(x) for x in inferred))
