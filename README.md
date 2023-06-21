# Cache-Aware and Cache-Oblivious Algorithms for Matrix Operations

This repository contains the implementation of cache-aware and cache-oblivious algorithms for matrix operations such as multiplication and transposition. The implemented algorithms are part of a term paper in the field of memory hierarchies.

## Contents

`multiplication.c` holds the source code for the matrix multiplication algorithms, `transpose.c` the source code for the matrix transposition algorithms.

## Benchmarking

To mesure the algorithms, one can use the procedures from the Makefile.

1. First, compile the algorithms:

```
make all
```

2. Start the benchmarking process for various problem sizes:
Depending on the running computer, this could take a long while!

```
make benchmark
```

The benchmarking procedure now have logged the time mesurements into the files `data.multiply.csv` and `data.transpose.csv`.

3. Plot the results

```
make plot
```

The output is a pdf with two pages, containing the two crated graphes.
