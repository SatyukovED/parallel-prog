#include <stdio.h>
#include <stdlib.h> 
#include <time.h>

int main() {
    int n, i, j, *A, *B;
    FILE *fpA, *fpB;
 
    fpA = fopen("./A.txt", "wb");
    fpB = fopen("./B.txt", "wb");
  
    printf("Enter n (A: n x n, B: n x n): ");
    scanf("%d", &n);
    
    srand(time(NULL));
    
    A = malloc(sizeof(int) * n * n);
    B = malloc(sizeof(int) * n * n);
    
    for(i = 0; i < n; i++) {
        for(j = 0; j < n; j++) {
            A[i * n + j] = (rand() % 20) - 10;
            fprintf(fpA, "%d ", A[i * n + j]);
        }
        fprintf(fpA, "\n");
    }
    
    for(i = 0; i < n; i++) {
        for(j = 0; j < n; j++) {
            B[i * n + j] = (rand() % 20) - 10;
            fprintf(fpB, "%d ", B[i * n + j]);
        }
        fprintf(fpB, "\n");
    }

    fclose(fpA);
    fclose(fpB);

    free(A);
    free(B);

    return 0;
}
