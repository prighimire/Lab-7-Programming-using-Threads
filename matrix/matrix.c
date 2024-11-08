#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define MAX 20

int matA[MAX][MAX]; 
int matB[MAX][MAX]; 

int matSumResult[MAX][MAX];
int matDiffResult[MAX][MAX]; 
int matProductResult[MAX][MAX]; 

typedef struct {
    int row;
    int col;
} Index;

void fillMatrix(int matrix[MAX][MAX]) {
    for(int i = 0; i < MAX; i++) {
        for(int j = 0; j < MAX; j++) {
            matrix[i][j] = rand() % 10 + 1;
        }
    }
}

void printMatrix(int matrix[MAX][MAX]) {
    for(int i = 0; i < MAX; i++) {
        for(int j = 0; j < MAX; j++) {
            printf("%5d", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void* computeSum(void* args) {
    Index* index = (Index*) args;
    int row = index->row;
    int col = index->col;
    matSumResult[row][col] = matA[row][col] + matB[row][col];
    free(args);
    return NULL;
}

void* computeDiff(void* args) {
    Index* index = (Index*) args;
    int row = index->row;
    int col = index->col;
    matDiffResult[row][col] = matA[row][col] - matB[row][col];
    free(args);
    return NULL;
}

void* computeProduct(void* args) {
    Index* index = (Index*) args;
    int row = index->row;
    int col = index->col;
    matProductResult[row][col] = 0;
    for (int k = 0; k < MAX; k++) {
        matProductResult[row][col] += matA[row][k] * matB[k][col];
    }
    free(args);
    return NULL;
}

int main() {
    srand(time(0));  // Do Not Remove. Just ignore and continue below.
    
    // 1. Fill the matrices (matA and matB) with random values.
    fillMatrix(matA);
    fillMatrix(matB);
    
    // 2. Print the initial matrices.
    printf("Matrix A:\n");
    printMatrix(matA);
    printf("Matrix B:\n");
    printMatrix(matB);
    
    // 3. Create pthread_t objects for our threads.
    pthread_t threads[10];
    
    // 4. Create threads for matrix addition.
    for (int i = 0; i < 10; i++) {
        Index* index = malloc(sizeof(Index));
        index->row = (i / 5); // 2 rows for sum
        index->col = (i % 5); // 5 columns for sum
        pthread_create(&threads[i], NULL, computeSum, (void*) index);
    }
    
    // Wait for all addition threads to finish
    for (int i = 0; i < 10; i++) {
        pthread_join(threads[i], NULL);
    }
    
    // 5. Create threads for matrix subtraction.
    for (int i = 0; i < 10; i++) {
        Index* index = malloc(sizeof(Index));
        index->row = (i / 5); // 2 rows for diff
        index->col = (i % 5); // 5 columns for diff
        pthread_create(&threads[i], NULL, computeDiff, (void*) index);
    }
    
    // Wait for all subtraction threads to finish
    for (int i = 0; i < 10; i++) {
        pthread_join(threads[i], NULL);
    }
    
    // 6. Create threads for matrix multiplication.
    for (int i = 0; i < 10; i++) {
        Index* index = malloc(sizeof(Index));
        index->row = (i / 5); // 2 rows for product
        index->col = (i % 5); // 5 columns for product
        pthread_create(&threads[i], NULL, computeProduct, (void*) index);
    }
    
    // Wait for all multiplication threads to finish
   