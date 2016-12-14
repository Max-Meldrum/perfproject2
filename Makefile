
.PHONY: all clean
all: fractal_seq fractal_par matmul_seq matmul_par qsort_seq qsort_par
clean:
	rm -rf \
		fractal_seq matmul_seq qsort_seq \
		fractal_par matmul_par qsort_par \
		fracout.tga

# Fractal
fractal_seq: src/fractal_seq.c
	$(CC) -o $@ $<
fractal_par: src/fractal_par.c
	$(CC) -o $@ $<

# Matmul
matmul_seq: src/matmul_seq.c
	$(CC) -o $@ $<
matmul_par: src/matmul_par.c
	$(CC) -o $@ $<

# Qsort
qsort_seq: src/qsort_seq.c
	$(CC) -o $@ $<
qsort_par: src/qsort_par.c
	$(CC) -o $@ $<
