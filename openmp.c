#include<stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <sys/time.h>

int main(int argc, char *argv[]) {
    int n, threadsNum, rowsByThread, i, j, k, threadNum, threadFirstRow, threadLastRow;
    long *A, *B, *C, temp;
    FILE *fpA, *fpB, *fpC;
	struct timeval begin, end;

    if (argc < 3) {
		printf("Program needs 2 arguments: n and threadsNum\n");
		return 1;
    } else {
    	n = atoi(argv[1]);
        threadsNum = atoi(argv[2]); 
    }
    printf("n = %d\n", n);
    printf("Threads number: %d\n", threadsNum);

    fpA = fopen("./A.txt", "rb");
    fpB = fopen("./B.txt", "rb");
    fpC = fopen("./C_openmp.txt", "wb");

    A = malloc(sizeof(long) * n * n);
    B = malloc(sizeof(long) * n * n);
    C = malloc(sizeof(long) * n * n);

    rowsByThread = n / threadsNum;

    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
    	    fscanf(fpA, "%ld", &A[i * n + j]);
        }
    }
    
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            fscanf(fpB, "%ld", &B[i * n + j]);
        }
    }

    for (i = 0; i < n; i++) {
        for(j = i + 1; j < n; j++){
            temp = B[i * n + j];
            B[i * n + j] = B[j * n + i];
            B[j * n + i] = temp;
        }
    }

    gettimeofday(&begin, NULL);
 
    omp_set_num_threads(threadsNum);
    # pragma omp parallel private(i, j, k, temp, threadNum, threadFirstRow, threadLastRow)
    {
        threadNum = omp_get_thread_num();
        threadFirstRow = threadNum * rowsByThread;
        threadLastRow = threadFirstRow + rowsByThread;
        
        for (i = threadFirstRow; i <= threadLastRow; i++) {
            for (j = 0; j < n; j++) {
                temp = 0;
                for (k = 0; k < n; k++) {
                    temp += A[i * n + k] * B[j * n + k];
                }
                C[i * n + j] = temp;
            }
        }
    }

	gettimeofday(&end, NULL);
    printf("Time of calculating matrix C = %f seconds\n", (double)(end.tv_usec - begin.tv_usec) / 1000000 + (double) (end.tv_sec - begin.tv_sec));

    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            fprintf(fpC, "%ld ", C[i * n + j]);
        }
        fprintf(fpC, "\n");
    }
    
    fclose(fpA);
    fclose(fpB);
    fclose(fpC);
    
	free(A);
	free(B);
	free(C);

    return 0;
}


