
.PHONY: all clean
all: fractal_seq matmul_seq qsort_seq
clean:
	rm -rf fractal_seq matmul_seq qsort_seq

fractal_seq: src/fractal_seq.c
	$(CC) -o $@ $<

matmul_seq: src/matmul_seq.c
	$(CC) -o $@ $<

qsort_seq: src/qsort_seq.c
	$(CC) -o $@ $<
