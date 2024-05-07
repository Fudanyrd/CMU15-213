/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/**
 * @cite <https://arthals.ink/posts/experience/cache-lab> 
 * I have learnt from the blog about block size and its impact on
 * cache misses.
 */
/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    int v0, v1, v2, v3;
    int v4, v5, v6, v7;
    if (M == 32) {
        // 32 * 32 case, use 8 * 8 blocks
        for (int i = 0; i < N; i += 8) {
            for (int j = 0; j < M; j += 8) {
                for (int k = 0; k < 8; ++k) {
                    v0 = A[i + k][j + 0];
                    v1 = A[i + k][j + 1];
                    v2 = A[i + k][j + 2];
                    v3 = A[i + k][j + 3];
                    v4 = A[i + k][j + 4];
                    v5 = A[i + k][j + 5];
                    v6 = A[i + k][j + 6];
                    v7 = A[i + k][j + 7];
                    B[j + k][i + 0] = v0;
                    B[j + k][i + 1] = v1;
                    B[j + k][i + 2] = v2;
                    B[j + k][i + 3] = v3;
                    B[j + k][i + 4] = v4;
                    B[j + k][i + 5] = v5;
                    B[j + k][i + 6] = v6;
                    B[j + k][i + 7] = v7;
                }
                // transpose B in place:)
                for (int k = 0; k < 8; ++k) {
                    for (int l = 0; l < k; ++l) {
                        v0 = B[j + k][i + l];
                        B[j + k][i + l] = B[j + l][i + k];
                        B[j + l][i + k] = v0;
                    }
                }
            }
        }
        return;
    }
    if (M == 64) {
        // trick: copy a 4 * 4 block into the correct position in B, 
        // and transpose that block in B(since that is already in block!)
        for (int i = 0; i < N; i += 4) {
            for (int j = 0; j < M; j += 4) {
                // move the data in block
                for (int k = 0; k < 4; ++k) {
                    v0 = A[i + k][j + 0];
                    v1 = A[i + k][j + 1];
                    v2 = A[i + k][j + 2];
                    v3 = A[i + k][j + 3];
                    B[j + k][i + 0] = v0;
                    B[j + k][i + 1] = v1;
                    B[j + k][i + 2] = v2;
                    B[j + k][i + 3] = v3;
                }
                // in-place transpose the block:)
                for (int k = 0; k < 4; ++k) {
                    for (int l = 0; l < k; ++l) {
                        v0 = B[j + k][i + l];
                        B[j + k][i + l] = B[j + l][i + k];
                        B[j + l][i + k] = v0;
                    }
                }
            }
        }
    }  else  {
        // 67 * 61 block.. what the hell is this😇
        // first take care of the 66 * 60, using 3 * 3 small blocks.
        for (int i = 0; i < 66; i += 3) {
            for (int j = 0; j < 60; j += 3) {

            }
        }
    }
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc); 

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

