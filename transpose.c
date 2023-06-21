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

// naive transpose algorithm (row by row)
void transpose_base(double *A, int lda, int rows, int cols, double *B, int ldb)
{
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            B[j*ldb + i] = A[i*lda + j];
        }
    }
}

// cache oblivious recursive transpose algorithm
void transpose_oblivious(double *A, int lda, int rows, int cols, double *B, int ldb)
{
    // Basecase: if the matrix is small enough, transpose it directly
    if (rows < 16) {
        transpose_base( A, lda, rows, cols, B, ldb );

    } else {
        if (rows > cols) {
            // transpose upper half of A
            transpose_oblivious( A, lda, rows/2, cols, B, ldb );
            // transpose lower half of A
            transpose_oblivious( A + lda * (rows/2) ,lda,
                    rows - rows/2, cols, B + rows/2, ldb );
        } else {
            // transpose left half of A
            transpose_oblivious( A, lda, rows, cols/2, B, ldb );
            // transpose right half of A
            transpose_oblivious( A + cols/2, lda,
                    rows, cols - cols/2, B + ldb * (cols/2), ldb );
        }
    }
}

// cache aware tiled transpose algorithm
void transpose_block(double *A, int lda, int rows, int cols, double *B, int ldb, int bs)
{
    for (int i = 0; i < rows; i += bs) {
        for (int j = 0; j < cols; j += bs) {
            for (int ii = i; ii < min(i + bs, rows); ii++) {
                for (int jj = j; jj < min(j + bs, cols); jj++) {
                    B[jj*ldb + ii] = A[ii*lda + jj];
                }
            }
        }
    }
}

// test function to check for correct matrix transpose
int test_result(double *A, double *AT, int rows, int cols)
{
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (A[i * cols + j] != AT[j * rows + i]) {
                return 0;
            }
        }
    }
    return 1;
}

// minimal working example
int main(int argc, char *argv[])
{
    int rows = atoi(argv[1]);
    int cols = atoi(argv[2]);

    struct timeval start, end;

    // allocate memory for input and output matrices
    double *A  = malloc(rows * cols * sizeof(double));
    double *AT = malloc(rows * cols * sizeof(double));

    // initialize random seed
    srand(time(NULL));

    int repeat = 5;

    double runtime[repeat];
    for (int i = 0; i < repeat; i++) {

        // initialize input matrix with random values
        for (int i = 0; i < rows * cols; i++)
            A[i] = (double) rand() / (double) RAND_MAX;

        gettimeofday(&start, NULL);
        // use the naive transpose algorithm
        if (atoi(argv[3]) == 1)
            transpose_base(A, cols, rows, cols, AT, rows);

        // use the blocked transpose algorithm
        else if (atoi(argv[3]) == 2)
            transpose_block(A, cols, rows, cols, AT, rows, atoi(argv[4]));

        // use the cache oblivious transpose algorithm
        else if (atoi(argv[3]) == 3)
            transpose_oblivious(A, cols, rows, cols, AT, rows);

        gettimeofday(&end, NULL);
        runtime[i] = (double) time_diff(&start, &end) / 1e6;
    }

    // sum up the runtimes and take the average
    double avg_runtime = 0;
    for (int i = 0; i < repeat; i++)
        avg_runtime += runtime[i];

    avg_runtime /= repeat;

    // test if the result is correct
    if ( ! test_result(A, AT, rows, cols)) {
        printf("Wrong result!\n");
    }

    printf("%.4es\n", avg_runtime);

    free(A);
    free(AT);
}
