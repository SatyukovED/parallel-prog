#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

int n, rowsByThread;
long *A, *B, *C;
 
void *pthreadMult(void *args);

int main(int argc, char *argv[]) {
    int threadsNum, i, j, k, threadNum, *threads;
    long temp;
    pthread_t* threadsHandles;
    pthread_attr_t attr;
    struct timeval begin, end;
	FILE *fpA, *fpB, *fpC;

    if (argc < 3) {
        printf("Program needs 2 arguments: n and threadsNum\n");
        return 1;
    } else {
        n = atoi(argv[1]);
        threadsNum = atoi(argv[2]);
    }
    printf("n = %d\n", n);
    printf("Threads number: %d\n", threadsNum);

    threads = malloc(threadsNum * sizeof(int));
    threadsHandles = malloc(threadsNum * sizeof(pthread_t));

    fpA = fopen("./A.txt", "rb");
    fpB = fopen("./B.txt", "rb");
    fpC = fopen("./C_pthread.txt", "wb");

    A = (long*)malloc(sizeof(long) * n * n);
    B = (long*)malloc(sizeof(long) * n * n);
    C = (long*)malloc(sizeof(long) * n * n);

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
  
    for(threadNum = 0; threadNum < threadsNum; threadNum++) {
        threads[threadNum] = threadNum;
        pthread_attr_init(&attr);
        pthread_create(&threadsHandles[threadNum], &attr, pthreadMult, (void*)&threads[threadNum]);
    }
   
    for (threadNum = 0; threadNum < threadsNum; threadNum++) {
        pthread_join(threadsHandles[threadNum], NULL);
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
    free(threads);
    free(threadsHandles);
    
    return 0;
}

void *pthreadMult(void *args) {
    int i, j, k;
    long temp;
    int *threadNum = (int*) args;
    int threadFirstRow = *threadNum * rowsByThread;
    int threadLastRow = threadFirstRow + rowsByThread;
    
    for (i = threadFirstRow; i < threadLastRow; i++) {
        for (j = 0; j < n; j++) {
            temp = 0;
            for (k = 0; k < n; k++) {
                temp += A[i * n + k] * B[j * n + k];
            }
            C[i * n + j] = temp;
        }
    }
}
