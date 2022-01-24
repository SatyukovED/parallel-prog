#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <sys/time.h>
 
int main(int argc, char *argv[]) {
    int n, procsNum, rowsByProc, i, j, k, procNum, procFirstRow, procLastRow;
    long *A, *B, *C, *buffer, *ans, temp;
	struct timeval begin, end; 

    if (argc < 2) {
        printf("Program needs 1 argument: n\n");
        return 1;
    } else {
        n = atoi(argv[1]);
    }
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &procNum);
    MPI_Comm_size(MPI_COMM_WORLD, &procsNum);
    rowsByProc = n / procsNum;

    A = malloc(sizeof(long) * n * n);
    B = malloc(sizeof(long) * n * n);
    C = malloc(sizeof(long) * n * n);

    buffer = malloc(sizeof(long) * n * rowsByProc);
    ans = malloc(sizeof(long) * n * rowsByProc);
 
    if (procNum == 0) {
        printf("n = %d\n", n);
        printf("Processes number: %d\n", procsNum);
      
        FILE *fpA = fopen("./A.txt", "rb");
        FILE *fpB = fopen("./B.txt", "rb");
        FILE *fpC = fopen("./C_mpi.txt", "wb");
 
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
            for(j = i + 1; j < n; j++) {
                temp = B[i * n + j];
                B[i * n + j] = B[j * n + i];
                B[j * n + i] = temp;
            }
        }

		gettimeofday(&begin, NULL);

        for (i = 1; i < procsNum; i++) {
            MPI_Send(B, n * n, MPI_LONG, i, 0, MPI_COMM_WORLD);
            procFirstRow = i * rowsByProc;
            procLastRow = procFirstRow + rowsByProc;
            for (j = procFirstRow; j < procLastRow; j++) {
                for (k = 0; k < n; k++){
                    buffer[(j - procFirstRow) * n + k] = A[j * n + k];
                }
            }
            MPI_Send(buffer, rowsByProc * n, MPI_LONG, i, 1, MPI_COMM_WORLD);
        }
        
        for (i = 1; i < procsNum; i++) {
            MPI_Recv(ans, rowsByProc * n, MPI_LONG, i, 3, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            procFirstRow = i * rowsByProc;
            procLastRow = procFirstRow + rowsByProc;
            for (j = procFirstRow; j < procLastRow; j++) {
                for (k = 0; k < n; k++) {
                    C[j * n + k] = ans[(j - procFirstRow) * n + k];
                }
            }
        }

        for (i = 0; i < rowsByProc; i++)
        {
            for (j = 0; j < n; j++)
            {
                temp = 0;
                for (k = 0; k < n; k++)
                    temp += A[i * n + k] * B[j * n + k];
                C[i * n + j] = temp;
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
        free(buffer);
        free(ans);

    } else {

        MPI_Recv(B, n * n, MPI_LONG, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(buffer, rowsByProc * n, MPI_LONG, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        for (i = 0; i < rowsByProc; i++) {
            for (j = 0; j < n; j++) {
                temp = 0;
                for(k = 0; k < n; k++) {
                    temp += buffer[i * n + k] * B[j * n + k];
                }
                ans[i * n + j] = temp;
            }
        }

        MPI_Send(ans, rowsByProc * n, MPI_LONG, 0, 3, MPI_COMM_WORLD);
    }
 
    MPI_Finalize();
 
    return 0;
}
