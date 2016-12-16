
CFLAGS=-std=c99
LIBS=-lm -lpthread

ifndef THREAD_COUNT
	THREAD_COUNT=8
endif

THREAD_CONF=-DTHREAD_COUNT=$(THREAD_COUNT)

.PHONY: all clean
.PHONY: *
all: fractal_seq fractal_par matmul_seq matmul_par matmul_lab3_par qsort_seq qsort_par qsort_par_balanced
clean:
	rm -rf \
		fractal_seq matmul_seq qsort_seq \
		fractal_par matmul_par matmul_lab3_par qsort_par \
		fracout.tga

# Fractal
fractal_seq: src/fractal_seq.c
	$(CC) $(CFLAGS) -o $@ $<
fractal_par: src/fractal_par.c
	$(CC) $(CFLAGS) -o $@ $< $(LIBS) $(THREAD_CONF)

# Matmul
matmul_seq: src/matmul_seq.c
	$(CC) $(CFLAGS) -o $@ $<
matmul_par: src/matmul_par.c
	$(CC) $(CFLAGS) -o $@ $< $(LIBS) $(THREAD_CONF)

matmul_lab3_par: src/matmul_lab3_par.c
	$(CC) $(CFLAGS) -w  -o $@ $< $(LIBS) $(THREAD_CONF)

# Qsort
qsort_seq: src/qsort_seq.c
	$(CC) $(CFLAGS) -o $@ $<
qsort_par: src/qsort_par.c
	$(CC) $(CFLAGS) -o $@ $< $(LIBS) $(THREAD_CONF)
qsort_par_balanced: src/qsort_par_balanced.c
	$(CC) $(CFLAGS) -o $@ $< $(LIBS) $(THREAD_CONF)
