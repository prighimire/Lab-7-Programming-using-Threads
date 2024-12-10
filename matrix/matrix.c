#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

// Structure to hold thread arguments
typedef struct {
    int thread_id;    // Identifier for the thread
    int startRow;     // Starting row index (inclusive)
    int endRow;       // Ending row index (exclusive)
    int size;         // Size of the matrix (N for N x N)
    int *matA;        // Pointer to Matrix A
    int *matB;        // Pointer to Matrix B
    int *matSum;      // Pointer to Sum Result Matrix
    int *matDiff;     // Pointer to Difference Result Matrix
    int *matProduct;  // Pointer to Product Result Matrix
} thread_arg_t;

// Function to fill a matrix with random values between 1 and 10
void fillMatrix(int *matrix, int size) {
    for(int i = 0; i < size * size; i++) {
        matrix[i] = rand() % 10 + 1;
    }
}

// Function to print a matrix
void printMatrix(int *matrix, int size) {
    for(int i = 0; i < size; i++) {
        for(int j = 0; j < size; j++) {
            printf("%5d", matrix[i * size + j]);
        }
        printf("\n");
    }
    printf("\n");
}

// Thread function to compute sum, difference, and product for a subset of rows
void* computeOperations(void* args) {
    thread_arg_t* data = (thread_arg_t*)args;
    int start = data->startRow;
    int end = data->endRow;
    int size = data->size;

    // Perform matrix addition and subtraction
    for(int i = start; i < end; i++) {
        for(int j = 0; j < size; j++) {
            // Compute Sum
            data->matSum[i * size + j] = data->matA[i * size + j] + data->matB[i * size + j];
            // Compute Difference
            data->matDiff[i * size + j] = data->matA[i * size + j] - data->matB[i * size + j];
        }
    }

    // Perform matrix multiplication (dot product)
    for(int i = start; i < end; i++) {
        for(int j = 0; j < size; j++) {
            int sum = 0;
            for(int k = 0; k < size; k++) {
                sum += data->matA[i * size + k] * data->matB[k * size + j];
            }
            data->matProduct[i * size + j] = sum;
        }
    }

    free(data); // Free the allocated memory for thread arguments
    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    srand(time(0));  // Initialize random number generator

    int MAX_SIZE = 20; // Default matrix size

    // 0. Get the matrix size from the command line if provided
    if(argc == 2) {
        MAX_SIZE = atoi(argv[1]);
        if(MAX_SIZE <= 0) {
            fprintf(stderr, "Invalid matrix size provided. Using default size 20.\n");
            MAX_SIZE = 20;
        }
    } else if(argc > 2) {
        fprintf(stderr, "Usage: %s [matrix_size]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // 1. Dynamically allocate memory for the matrices
    int *matA = (int*)malloc(MAX_SIZE * MAX_SIZE * sizeof(int));
    int *matB = (int*)malloc(MAX_SIZE * MAX_SIZE * sizeof(int));
    int *matSum = (int*)malloc(MAX_SIZE * MAX_SIZE * sizeof(int));
    int *matDiff = (int*)malloc(MAX_SIZE * MAX_SIZE * sizeof(int));
    int *matProduct = (int*)malloc(MAX_SIZE * MAX_SIZE * sizeof(int));

    if(matA == NULL || matB == NULL || matSum == NULL || matDiff == NULL || matProduct == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }

    // 2. Fill the matrices (matA and matB) with random values.
    fillMatrix(matA, MAX_SIZE);
    fillMatrix(matB, MAX_SIZE);

    // 3. Print the initial matrices.
    printf("Matrix A (%dx%d):\n", MAX_SIZE, MAX_SIZE);
    printMatrix(matA, MAX_SIZE);
    printf("Matrix B (%dx%d):\n", MAX_SIZE, MAX_SIZE);
    printMatrix(matB, MAX_SIZE);

    // 4. Create pthread_t objects for our threads.
    const int NUM_THREADS = 10;
    pthread_t threads[NUM_THREADS];
    int rc;
    int rowsPerThread = (MAX_SIZE + NUM_THREADS - 1) / NUM_THREADS; // Ceiling division

    // 5. Create a thread for each subset of rows.
    for(int t = 0; t < NUM_THREADS; t++) {
        thread_arg_t* arg = (thread_arg_t*)malloc(sizeof(thread_arg_t));
        if(arg == NULL) {
            fprintf(stderr, "Failed to allocate memory for thread arguments.\n");
            exit(EXIT_FAILURE);
        }
        arg->thread_id = t;
        arg->size = MAX_SIZE;
        arg->matA = matA;
        arg->matB = matB;
        arg->matSum = matSum;
        arg->matDiff = matDiff;
        arg->matProduct = matProduct;

        arg->startRow = t * rowsPerThread;
        arg->endRow = (t + 1) * rowsPerThread;
        if(arg->endRow > MAX_SIZE) {
            arg->endRow = MAX_SIZE;
        }

        // If startRow >= MAX_SIZE, no rows to process
        if(arg->startRow >= MAX_SIZE) {
            arg->startRow = MAX_SIZE;
            arg->endRow = MAX_SIZE;
        }

        rc = pthread_create(&threads[t], NULL, computeOperations, (void*)arg);
        if(rc) {
            fprintf(stderr, "Error: Unable to create thread %d, return code %d\n", t, rc);
            exit(EXIT_FAILURE);
        }
    }

    // 6. Wait for all threads to finish.
    for(int t = 0; t < NUM_THREADS; t++) {
        pthread_join(threads[t], NULL);
    }

    // 7. Print the results.
    printf("Results:\n");

    printf("Sum (%dx%d):\n", MAX_SIZE, MAX_SIZE);
    printMatrix(matSum, MAX_SIZE);

    printf("Difference (%dx%d):\n", MAX_SIZE, MAX_SIZE);
    printMatrix(matDiff, MAX_SIZE);

    printf("Product (%dx%d):\n", MAX_SIZE, MAX_SIZE);
    printMatrix(matProduct, MAX_SIZE);

    // 8. Free allocated memory
    free(matA);
    free(matB);
    free(matSum);
    free(matDiff);
    free(matProduct);

    return 0;
}
