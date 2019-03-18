#include <stdio.h>
#include <stdlib.h>

void dgemm(int n, double *A, double *B, double *C)
{
    int i, j, k;
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            double Cij = C[i + j*n];
            for(k = 0; k < n; k++) {
                Cij += A[i + k*n] * B[k + j*n];
            }
            C[i + j*n] = Cij;
        }
    }
    return ;
}

int main(int argc, char **argv)
{
    int ret = 0;
    int size = 30;
    double *A = NULL, *B = NULL, *C = NULL;
    int j;

    if (argc == 2) {
        size = atoi(argv[1]);
    }

    A = (double *)malloc(sizeof(double) * (size * size));
    B = (double *)malloc(sizeof(double) * (size * size));
    C = (double *)malloc(sizeof(double) * (size * size));
    
    if (A == NULL || B == NULL || C == NULL) {
        printf("malloc failed\n");
        goto failed;
    }
    for (j = 0; j < size * size; j ++) {
        A[j] = rand();
    }
    for (j = 0; j < size * size; j ++) {
        B[j] = rand();
    }

    dgemm(size, A, B, C);

    for (j = 1; j <= size * size; j ++) {
        printf("%f ", C[j]);
        if (j%size == 0) {
            printf("\n");
        }
    }
    
failed:
    if (A) {
        free(A);
    }
    if (B) {
        free(B);
    }
    if (C) {
        free(C);
    }
    return 0;
}






