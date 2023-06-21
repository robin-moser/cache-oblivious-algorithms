GCC_BIN=gcc
GCC_OPT=-O3 -Wall

.PHONY: all clean plot

all: transpose multiplication
plot: plot.benchmark.pdf

transpose: transpose.c
	$(GCC_BIN) $(GCC_OPT) -o $@ -lm $^

multiplication: multiplication.c
	$(GCC_BIN) $(GCC_OPT) -o $@ -lm $^

plot.benchmark.pdf: plot.benchmark.plt data*
	gnuplot plot.benchmark.plt

benchmark:
	for i in `seq 100 100 900` `seq 1000 1000 32000`; do \
		echo "Benchmarking $$i x $$i matrix..."; \
		echo $$i,`./transpose $$i $$i 1`, \
			`./transpose $$i $$i 2 32`, \
			`./transpose $$i $$i 3` \
		>> data.transpose.csv; \
	done

clean:
	rm -f plot.benchmark.pdf transpose multiplication
