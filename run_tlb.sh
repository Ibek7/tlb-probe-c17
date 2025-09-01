#!/bin/bash
# run_tlb.sh
# Run tlb for powers-of-two page counts and collect results.

if [ ! -x ./tlb ]; then
  echo "tlb binary not found or not executable. Build it with: gcc -O2 -std=c11 -Wall tlb.c -o tlb -lrt"
  exit 1
fi

OUT=results.txt
echo "#pages\tavg_ns" > "$OUT"
TRIALS=${1:-10000}
MAX_PAGES=${2:-8192}

pages=1
while [ $pages -le $MAX_PAGES ]; do
  echo "Running pages=$pages trials=$TRIALS"
  ./tlb $pages $TRIALS >> "$OUT"
  pages=$((pages * 2))
done

echo "Results written to $OUT"
