CC = gcc
CFLAGS = -O2 -std=c11 -Wall

all: tlb

tlb: tlb.c
	$(CC) $(CFLAGS) tlb.c -o tlb

clean:
	rm -f tlb *.o results.txt tlb_plot.png

run: tlb
	./run_tlb.sh 10000 8192

plot:
	python3 plot_tlb.py
