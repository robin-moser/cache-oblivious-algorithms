GCC_BIN=gcc
GCC_OPT=-O3 -Wall

.PHONY: all clean plot

all: transpose multiplication
plot: plot.benchmark.pdf
benchmark: benchmark-transpose benchmark-multiply

transpose: transpose.c
	$(GCC_BIN) $(GCC_OPT) -o $@ -lm $^

multiplication: multiplication.c
	$(GCC_BIN) $(GCC_OPT) -o $@ -lm $^

plot.benchmark.pdf: plot.benchmark.plt data*
	gnuplot plot.benchmark.plt

benchmark-transpose: transpose
	rm -f data.transpose.csv
	for i in `seq 100 100 900` `seq 1000 200 16000` \
		1024 2048 4096 8192; do \
		echo "Benchmarking $$i x $$i matrix..."; \
		echo $$i, \
			`./transpose $$i $$i 1`,      \
			`./transpose $$i $$i 3`,      \
			`./transpose $$i $$i 2 8`,    \
			`./transpose $$i $$i 2 16`,   \
			`./transpose $$i $$i 2 32`,   \
			`./transpose $$i $$i 2 64`,   \
			`./transpose $$i $$i 2 128`,  \
		>> data.transpose.csv; \
	done

benchmark-multiply: multiplication
	rm -f data.multiply.csv
	for i in `seq 100 100 900` `seq 1000 200 4000` \
		1024 2048; do \
		echo "Benchmarking $$i x $$i matrix..."; \
		echo $$i, \
			`./multiplication $$i $$i $$i 1`,     \
			`./multiplication $$i $$i $$i 3`,     \
			`./multiplication $$i $$i $$i 2 8`,   \
			`./multiplication $$i $$i $$i 2 16`,  \
			`./multiplication $$i $$i $$i 2 32`,  \
			`./multiplication $$i $$i $$i 2 64`,  \
			`./multiplication $$i $$i $$i 2 128`, \
		>> data.multiply.csv; \
	done

clean:
	rm -f plot.benchmark.pdf transpose multiplication
