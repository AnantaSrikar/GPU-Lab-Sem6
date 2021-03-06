/*
	Author: Ananta Srikar

	Program to multiply two square-matrices of order 1024 X 1024 using Block Matrix Multiplication
	algorithm on multiple threads by considering block sizes: 4, 8, 16, 32, and 64.

	Original code can be found here: https://github.com/AnantaSrikar/DAA-Lab-Sem4/tree/master/lab_4
*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include<sys/time.h>
#include<omp.h>

int main(int argc, char **argv)
{
	// Initial code to get all command line values
	
	if(argc != 3)
	{
		printf("Incorrect number of arguments! Please go through README.md\n");
		return -1;
	}

	for(int i = 1; i < argc; i++)
		for(int j = 0; j < strlen(argv[i]); j++)
			if(!isdigit(argv[i][j]))
			{
				printf("Invalid arguments! Please go through README.md"); // Enter only numbers!
				return -1;
			}

	// Block size and number of threads taken in from command line
	const int num_thread = atoi(argv[1]);
	const int block_size = atoi(argv[2]);

	int max_threads = omp_get_max_threads();

	if(!(num_thread <= max_threads))
	{
		printf("This computer has only %d vCPUs, less than %d as requested. Program will now terminate.\n", max_threads, num_thread);
		return -1;
	}

	if(!(((num_thread % 2 == 0) && (num_thread > 0)) || (num_thread == 1)))
	{
		printf("Invalid number of threads requested! Please go through README.md");
		return -1;
	}

	// Checking for only allowed block sizes
	if(!(block_size == 4 || block_size == 8 || block_size == 16 || block_size == 32 || block_size == 64))
	{
		printf("Invalid block size entered! Please go through README.md"); // Enter legit block size!
		return -1;
	}

	// End of command line arguments

	printf("Using %d/%d threads, block size = %d ...\n\n", num_thread, max_threads, block_size);
	omp_set_num_threads(num_thread);

	// vars to store timestamps to calculate exectuion time
	struct timeval start, end_init, end_exec;

	// Function prototypes
	int **get_rand_array(int);
	int **block_matrix_multi(int**, int**, int, int, int);
	void writeToFile(FILE*, int**, int**, int**, int);

	// File handling
	FILE *outFPtr = fopen("GPUmatrixoutput.txt", "w");
	
	// fixed size of matrices
	const int size = 2048;

	// Getting start time for initialisation
	gettimeofday(&start, NULL);

	// Initializing two arrays of order 2048x2048 with random values
	int **A = get_rand_array(size);
	int **B = get_rand_array(size);

	// End of initialisation time
	gettimeofday(&end_init, NULL);

	int **C = block_matrix_multi(A, B, size, block_size, num_thread);

	// Time after executing the block matrix multiplication
	gettimeofday(&end_exec, NULL);

	// Calulating times for initialisation and execution
	float init_time = ((end_init.tv_sec * 1000000 + end_init.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec));
	float exec_time = ((end_exec.tv_sec * 1000000 + end_exec.tv_usec) - (end_init.tv_sec * 1000000 + end_init.tv_usec));

	init_time /= 1000000;
	exec_time /= 1000000;

	// Writing all the matrices to the output file
	writeToFile(outFPtr, A, B, C, size);

	printf("\nTime taken to initialise: %fs\n", init_time);
	printf("Time taken to execute: %fs\n\n", exec_time);

	printf("Both the input matrices and their multiplied matrix is stored in 'DAALab_output1.txt'.\n");
	
	return(0);
}

// Function to convert a matrix into its transpose
void transpose_matrix(int **X, int size)
{
	for(int i = 0; i < size; i++)
		for(int j = 0; j < i; j++)
		{
			int temp = X[i][j];
			X[i][j] = X[j][i];
			X[j][i] = temp;
		}
}

// Function to get matrix with random numbers
int **get_rand_array(int size)
{
	int **arr;

	arr = (int**)malloc(size * sizeof(int*));

	for(int i = 0; i < size; i++)
		arr[i] = (int*)malloc(size * sizeof(int));
	
	// Seeding the random number generator
	srand(0);

	for(int i = 0; i < size; i++)
		for(int j = 0; j < size; j++)
			arr[i][j] = rand() % 10;

	return arr;
}

// Function to get 0 matrix, which can be filled later
int **get_zero_array(int size)
{
	int **arr;

	arr = (int**)malloc(size * sizeof(int*));

	for(int i = 0; i < size; i++)
		arr[i] = (int*)malloc(size * sizeof(int));

	for(int i = 0; i < size; i++)
		for(int j = 0; j < size; j++)
			arr[i][j] = 0;

	return arr;
}

// Doing the actual matrix multi in the smaller block matrix
void sub_matrix_multi(int **A, int **B, int **C, int i_start, int j_start, int k_start, int blk_size, int num_thread)
{
	int i, j, k;

	// Using openMP's pragma for multi-threading
	omp_set_dynamic(0);
	omp_set_num_threads(num_thread);
	#pragma omp parallel for private(i,j,k) shared(A,B,C)
	for(i = i_start; i < i_start + blk_size; i++)
		for(j = j_start; j < j_start + blk_size; j++)
			for(k = k_start; k < k_start + blk_size; k++)
				C[i][j] += A[i][k] * B[j][k];
}

// Block matrix multiplication algorithm
int **block_matrix_multi(int **A, int **B, int size, int blk_size, int num_thread)
{
	int i, j, k;
	int **C = get_zero_array(size);

	transpose_matrix(B, size);

	// Using openMP's pragma for multi-threading
	omp_set_dynamic(0);
	omp_set_num_threads(num_thread);
	#pragma omp parallel for private(i,j,k) shared(A,B,C)
	for(i = 0; i < size; i += blk_size)
		for(j = 0; j < size; j += blk_size)
			for(k = 0; k < size; k += blk_size)
				sub_matrix_multi(A, B, C, i, j, k, blk_size, num_thread);

	return C;
}

// Function to write the matrices into the output file
void writeToFile(FILE *outFPtr, int **A, int **B, int **C, int size)
{
	// Transposing the matrix B back
	transpose_matrix(B, size);

	fprintf(outFPtr, "Matrix A: \n");

	for(int i = 0; i < size; i++)
	{
		for(int j = 0; j < size; j++)
			fprintf(outFPtr, "%d ", A[i][j]);

		fprintf(outFPtr, "\n");
	}

	fprintf(outFPtr, "\n\n");

	fprintf(outFPtr, "Matrix B: \n");

	for(int i = 0; i < size; i++)
	{
		for(int j = 0; j < size; j++)
			fprintf(outFPtr, "%d ", B[i][j]);

		fprintf(outFPtr, "\n");
	}

	fprintf(outFPtr, "\n\n");

	fprintf(outFPtr, "Matrix C: \n");

	for(int i = 0; i < size; i++)
	{
		for(int j = 0; j < size; j++)
			fprintf(outFPtr, "%d ", C[i][j]);

		fprintf(outFPtr, "\n");
	}
}