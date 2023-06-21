#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <sys/time.h>

static inline int min(int a, int b) { return (a < b) ? a : b; }
static inline int max(int a, int b) { return (a > b) ? a : b; }

// inline function to calculate the runtime
static inline uint64_t time_diff( const struct timeval *ts1, const struct timeval *ts2)
{
    return ((uint64_t) (ts2->tv_sec - ts1->tv_sec) * 1000000ULL +
            (uint64_t) ts2->tv_usec - (uint64_t) ts1->tv_usec);
}

// naive matrix multiplication
void multiply(double *A, int lda, int rowsA, int colsA,
        double *B, int ldb, int rowsB, int colsB, double *C)
{
    for (int i = 0; i < rowsA; i++) {
        for (int j = 0; j < colsB; j++) {
            for (int k = 0; k < colsA; k++) {
                C[i * ldb + j] += A[i * lda + k] * B[k * ldb + j];
            }
        }
    }
}

// cache oblivious matrix multiplication after Frigo et al.
void multiply_oblivious(double *A, int lda, int rowsA, int colsA,
        double *B, int ldb, int rowsB, int colsB, double *C)
{
    if (rowsA <= 16 && colsA <= 16 && colsB <= 16) {
        multiply(A, lda, rowsA, colsA, B, ldb, rowsB, colsB, C);

    } else {

        if (colsA >= max(rowsA, colsB)) {
            multiply_oblivious(A, lda, rowsA, colsA/2, B, ldb, rowsB/2, colsB, C);
            multiply_oblivious(A + colsA/2, lda, rowsA, colsA - colsA/2,
                B + ldb * (rowsB/2), ldb, rowsB - rowsB/2, colsB, C);


        } else if (rowsA >= max(colsA, colsB)) {
            multiply_oblivious(A, lda, rowsA/2, colsA, B, ldb, rowsB, colsB, C);
            multiply_oblivious(A + lda * (rowsA/2), lda, rowsA - rowsA/2, colsA,
                B, ldb, rowsB, colsB, C + rowsA/2 * ldb);

        } else {
            multiply_oblivious(A, lda, rowsA, colsA, B, ldb, rowsB, colsB/2, C);
            multiply_oblivious(A, lda, rowsA, colsA, B + colsB/2, ldb, rowsB,
                colsB - colsB/2, C + colsB/2);
        }
    }
}

// cache aware matrix multiplication with blocking
void multiply_blocked(double *A, int rowsA, int colsA,
        double *B, int rowsB, int colsB, double *C, int bs)
{
    for (int i = 0; i < rowsA; i += bs) {
        for (int j = 0; j < colsB; j += bs) {
            for (int k = 0; k < colsA; k += bs) {

                for (int ii = i; ii < min(i + bs, rowsA); ii++) {
                    for (int jj = j; jj < min(j + bs, colsB); jj++) {

                        for (int kk = k; kk < min(k + bs, colsA); kk++) {
                            C[ii * colsB + jj] += A[ii * colsA + kk] * B[kk * colsB + jj];
                        }
                    }
                }
            }
        }
    }
}

// test if the result is correct (by comparing it to the naive algorithm)
int test_result(double *A, int rowsA, int colsA, double *B, int rowsB, int colsB, double *C)
{
    double *D = malloc(rowsA * colsB * sizeof(double));
    multiply(A, colsA, rowsA, colsA, B, colsB, rowsB, colsB, D);
    for (int i = 0; i < rowsA * colsB; i++) {
        if (C[i] != D[i]) {
            printf("C[%d] = %.2f != %.2f\n", i, C[i], D[i]);
            return 0;
        }
    }

    free(D);
    return 1;
}

// minimal working example
int main(int argc, char *argv[])
{
    int rowsA = atoi(argv[1]);
    int colsA = atoi(argv[2]);
    int colsB = atoi(argv[3]);

    struct timeval start, end;

    // allocate memory for input and output matrices
    double *A = malloc(rowsA * colsA * sizeof(double));
    double *B = malloc(colsA * colsB * sizeof(double));
    double *C = malloc(rowsA * colsB * sizeof(double));

    // initialize random seed
    srand(time(NULL));

    int repeat = 3;

    double runtime[repeat];
    for (int i = 0; i < repeat; i++) {

        // initialize input matrices with random values
        for (int i = 0; i < rowsA * colsA; i++)
            A[i] = (double) rand() / (double) RAND_MAX;

        for (int i = 0; i < colsA * colsB; i++)
            B[i] = (double) rand() / (double) RAND_MAX;

        for (int i = 0; i < rowsA * colsB; i++)
            C[i] = 0;

        gettimeofday(&start, NULL);
        // use the naive transpose algorithm
        if (atoi(argv[4]) == 1)
            multiply(A, colsA, rowsA, colsA, B, colsB, colsA, colsB, C);

        // use the blocked transpose algorithm
        else if (atoi(argv[4]) == 2)
            multiply_blocked(A, rowsA, colsA, B, colsA, colsB, C, atoi(argv[5]));

        // use the cache oblivious transpose algorithm
        else if (atoi(argv[4]) == 3)
            multiply_oblivious(A, colsA, rowsA, colsA, B, colsB, colsA, colsB, C);

        gettimeofday(&end, NULL);
        runtime[i] = (double) time_diff(&start, &end) / 1e6;
    }

    // sum up the runtimes and take the average
    double avg_runtime = 0;
    for (int i = 0; i < repeat; i++)
        avg_runtime += runtime[i];

    avg_runtime /= repeat;

    // test if the result is correct
    if ( ! test_result(A, rowsA, colsA, B, colsA, colsB, C)) {
        printf("Wrong result!\n");
    }

    printf("%.4es\n", avg_runtime);

    free(A);
    free(B);
    free(C);
}
