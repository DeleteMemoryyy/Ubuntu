/*
* Liu DeXin	1500017704@pku.edu.cn
*
* Use different strategies in transpose 
* of matrixs with different size .
*
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
#include "contracts.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/*
* transpose_submit - This is the solution transpose function that you
*     will be graded on for Part B of the assignment. Do not change
*     the description string "Transpose submission", as the driver
*     searches for that string to identify the transpose function to
*     be graded. The REQUIRES and ENSURES from 15-122 are included
*     for your convenience. They can be removed if you like.
*/
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, ii, jj, t0, t1, t2, t3, t4, t5, t6, t7;

    REQUIRES(M > 0);
    REQUIRES(N > 0);

    /* 32*32 matrix， erery 8 integers in a column 
    should be put into different sets. */
    if (M == 32)
    {
        /* So the largest size of block will be 8*8. */
        for (i = 0; i < N; i += 8)
            for (j = 0; j < M; j += 8)
                for (ii = i; ii < i + 8; ++ii)
                {
                    /*  This part will cause 64 more misses.
                    for(jj = j;jj<j+8;++jj)
                    {
                        t0 = A[ii][jj];
                        B[jj][ii]=t0;
                    }
                    */
                    /* Unrolling cycle to avoid memory access miss. */
                    t0 = A[ii][j];
                    t1 = A[ii][j + 1];
                    t2 = A[ii][j + 2];
                    t3 = A[ii][j + 3];
                    t4 = A[ii][j + 4];
                    t5 = A[ii][j + 5];
                    t6 = A[ii][j + 6];
                    t7 = A[ii][j + 7];
                    B[j][ii] = t0;
                    B[j + 1][ii] = t1;
                    B[j + 2][ii] = t2;
                    B[j + 3][ii] = t3;
                    B[j + 4][ii] = t4;
                    B[j + 5][ii] = t5;
                    B[j + 6][ii] = t6;
                    B[j + 7][ii] = t7;
                }
    }

    /* 64*64 matrix， erery 4 integers in a column should be 
    put into different sets. */
    else if (M == 64)
    {
        /* So the size of block should be 8*8, 
        and we could devide it into four pars.  */
        for (i = 0; i < M; i += 8)
        {
            for (j = 0; j < N; j += 8)
            {
                /* 
                    A         
                +——————+  
                | a  b | 
                | c  d |  
                +——————+     

                    B               B
                +——————+        +——————+
                |      |    →   | a' b'|
                |      |        |      |
                +——————+        +——————+

                 */
                for (ii = i; ii < i + 4; ++ii)
                {
                    /* 5 misses, save the last 4 integers in a line of cashe,
                    so we can use it */
                    t0 = A[ii][j];
                    t1 = A[ii][j + 1];
                    t2 = A[ii][j + 2];
                    t3 = A[ii][j + 3];
                    t4 = A[ii][j + 4];
                    t5 = A[ii][j + 5];
                    t6 = A[ii][j + 6];
                    t7 = A[ii][j + 7];
                    B[j][ii] = t0;
                    B[j][ii + 4] = t4;
                    B[j + 1][ii] = t1;
                    B[j + 1][ii + 4] = t5;
                    B[j + 2][ii] = t2;
                    B[j + 2][ii + 4] = t6;
                    B[j + 3][ii] = t3;
                    B[j + 3][ii + 4] = t7;
                }

                /* 
                    A         
                +——————+  
                | a  b | 
                | c  d |  
                +——————+     

                    B               B               B
                +——————+        +——————+        +——————+
                | a' b'|    →   | a'   |    →   | a' c'|
                |      |        | b'   |        | b'   |
                +——————+        +——————+        +——————+

                 */
                for (jj = j; jj < j + 4; ++jj)
                {
                    /* 4 misses, transform data normally from A. */
                    t0 = A[i + 4][jj];
                    t1 = A[i + 5][jj];
                    t2 = A[i + 6][jj];
                    t3 = A[i + 7][jj];
                    t4 = B[jj][i + 4];
                    /* 4 misses, load the data which were just stored in 
                    the diagonal position of where they should be set */
                    t5 = B[jj][i + 5];
                    t6 = B[jj][i + 6];
                    t7 = B[jj][i + 7];
                    B[jj][i + 4] = t0;
                    B[jj][i + 5] = t1;
                    B[jj][i + 6] = t2;
                    B[jj][i + 7] = t3;
                    /* 4 misses, store the data just loaded from B.*/
                    B[jj + 4][i] = t4;
                    B[jj + 4][i + 1] = t5;
                    B[jj + 4][i + 2] = t6;
                    B[jj + 4][i + 3] = t7;
                }

                /* 
                    A         
                +——————+  
                | a  b | 
                | c  d |  
                +——————+     

                    B               B
                +——————+        +——————+
                | a' c'|    →   | a' c'|
                | b'   |        | b' d'|
                +——————+        +——————+

                 */
                for (jj = j + 4; jj < j + 8; ++jj)
                {
                    /* 4 misses, transform data normally from A. */
                    t0 = A[i + 4][jj];
                    t1 = A[i + 5][jj];
                    t2 = A[i + 6][jj];
                    t3 = A[i + 7][jj];
                    B[jj][i + 4] = t0;
                    B[jj][i + 5] = t1;
                    B[jj][i + 6] = t2;
                    B[jj][i + 7] = t3;
                }
            }
        }
    }

    /* Try to use different sizes of blocks */
    else if (M == 61)
    {
        /* The 13 * 8 block perfumes well enough. */
        for (j = 0; j < M; j += 8)
            for (i = 0; i < N; i += 13)
            {
                for (ii = i; ii < i + 13 && ii < N; ++ii)
                {
                    /* Unrolling cycle to avoid memory access miss. */
                    t0 = A[ii][j];
                    t1 = A[ii][j + 1];
                    t2 = A[ii][j + 2];
                    t3 = A[ii][j + 3];
                    t4 = A[ii][j + 4];
                    t5 = A[ii][j + 5];
                    t6 = A[ii][j + 6];
                    t7 = A[ii][j + 7];
                    B[j][ii] = t0;
                    B[j + 1][ii] = t1;
                    B[j + 2][ii] = t2;
                    B[j + 3][ii] = t3;
                    B[j + 4][ii] = t4;
                    B[j + 5][ii] = t5;
                    B[j + 6][ii] = t6;
                    B[j + 7][ii] = t7;
                }
            }
    }

    /* Normal case, use 8 * 8 block. */
    else
    {
        for (j = 0; j + 8 <= M; j += 8)
        {
            for (i = 0; i + 8 <= N; i += 8)
            {
                for (ii = i; ii < i + 8; ++ii)
                {
                    t0 = A[ii][j];
                    t1 = A[ii][j + 1];
                    t2 = A[ii][j + 2];
                    t3 = A[ii][j + 3];
                    t4 = A[ii][j + 4];
                    t5 = A[ii][j + 5];
                    t6 = A[ii][j + 6];
                    t7 = A[ii][j + 7];
                    B[j][ii] = t0;
                    B[j + 1][ii] = t1;
                    B[j + 2][ii] = t2;
                    B[j + 3][ii] = t3;
                    B[j + 4][ii] = t4;
                    B[j + 5][ii] = t5;
                    B[j + 6][ii] = t6;
                    B[j + 7][ii] = t7;
                }
            }
        }

        if (i < N && j < M)
        {
            for (ii = 0; i + ii < N; ++ii)
            {
                for (jj = 0; j + jj < M; ++jj)
                {
                    t0 = A[i + ii][j + jj];
                    B[j + jj][i + ii] = t0;
                }
            }
        }

        if (i < N)
        {
            for (t7 = 0; t7 < j; t7 += 8)
            {
                for (ii = 0; i + ii < N; ++ii)
                {
                    for (jj = 0; jj < 8; ++jj)
                    {
                        t0 = A[i + ii][t7 + jj];
                        B[t7 + jj][i + ii] = t0;
                    }
                }
            }
        }

        if (j < M)
        {
            for (t7 = 0; t7 < i; t7 += 8)
            {
                for (jj = 0; j + jj < M; ++jj)
                {
                    for (ii = 0; ii < 8; ++ii)
                    {
                        t0 = A[t7 + ii][j + jj];
                        B[j + jj][t7 + ii] = t0;
                    }
                }
            }
        }
    }

    ENSURES(is_transpose(M, N, A, B));
}

/*
* You can define additional transpose functions below. We've defined
* a simple one below to help you get started.
*/

/*
* simple 8*8 block
*/
char trans_blocking_desc[] = "8 * 8 block transpose";
void trans_blocking(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, ii, jj, t0, t1, t2, t3, t4, t5, t6, t7;

    REQUIRES(M > 0);
    REQUIRES(N > 0);

    for (j = 0; j < M; j += 8)
    {
        for (i = 0; i < N; i += 8)
        {
            for (ii = i; ii < i + 8; ++ii)
            {
                t0 = A[ii][j];
                t1 = A[ii][j + 1];
                t2 = A[ii][j + 2];
                t3 = A[ii][j + 3];
                t4 = A[ii][j + 4];
                t5 = A[ii][j + 5];
                t6 = A[ii][j + 6];
                t7 = A[ii][j + 7];
                B[j][ii] = t0;
                B[j + 1][ii] = t1;
                B[j + 2][ii] = t2;
                B[j + 3][ii] = t3;
                B[j + 4][ii] = t4;
                B[j + 5][ii] = t5;
                B[j + 6][ii] = t6;
                B[j + 7][ii] = t7;
            }
        }
    }
    i -= 8;
    j -= 8;

    if (i < N && j < M)
    {
        for (ii = 0; i + ii < N; ++ii)
        {
            for (jj = 0; j + jj < M; ++jj)
            {
                t0 = A[i + ii][j + jj];
                B[j + jj][i + ii] = t0;
            }
        }
    }

    if (i < N)
    {
        for (t7 = 0; t7 < j; t7 += 8)
        {
            for (ii = 0; i + ii < N; ++ii)
            {
                for (jj = 0; jj < 8; ++jj)
                {
                    t0 = A[i + ii][t7 + jj];
                    B[t7 + jj][i + ii] = t0;
                }
            }
        }
    }

    if (j < M)
    {
        for (t7 = 0; t7 < i; t7 += 8)
        {
            for (jj = 0; j + jj < M; ++jj)
            {
                for (ii = 0; ii < 8; ++ii)
                {
                    t0 = A[t7 + ii][j + jj];
                    B[j + jj][t7 + ii] = t0;
                }
            }
        }
    }

    ENSURES(is_transpose(M, N, A, B));
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
    // registerTransFunction(trans_blocking, trans_blocking_desc);
}

/*
* is_transpose - This helper function checks if B is the transpose of
*     A. You can check the correctness of your transpose by calling
*     it before returning from the transpose function.
*/
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++)
    {
        for (j = 0; j < M; ++j)
        {
            if (A[i][j] != B[j][i])
            {
                return 0;
            }
        }
    }
    return 1;
}
