/*
	program to sort 10000000 (1 crore integers) elements using
	Merge sort technique, parallely.

	(Ideal) Memory usage for i crore integers = 4 crore bytes = 40 MB (in RAM)

	TODO: Parallelize it

	Author: Srikar
*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include<sys/time.h>
#include<omp.h>
#include<time.h>

#define ARR_SIZE 10000000

int main(int argc, char **argv)
{
	// Current time being used as a seed for random number generator
	srand(time(0));

	// Initial code to get all command line values
	
	if(argc != 2)
	{
		printf("Incorrect number of arguments! Please go through README.md\n");
		return -1;
	}

	for(int j = 0; j < strlen(argv[1]); j++)
		if(!isdigit(argv[1][j]))
		{
			printf("Invalid arguments! Please go through README.md"); // Enter only numbers!
			return -1;
		}

	// Checking validity of the number of threads we can run on
	const int num_thread = atoi(argv[1]);

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

	printf("Using %d/%d threads...\n\n", num_thread, max_threads);
	omp_set_num_threads(num_thread);

	// Function prototypes
	int *getBigRandomArray();
	void mergeSort(int*, int, int);
	void testSort(int*, int);

	// vars to store timestamps to calculate exectuion time
	struct timeval start, end_init, end_exec;

	// Getting start time for initialisation
	gettimeofday(&start, NULL);

	int *arr = getBigRandomArray();

	// End of initialisation time
	gettimeofday(&end_init, NULL);

	mergeSort(arr, 0, ARR_SIZE - 1);

	// Time after executing the block matrix multiplication
	gettimeofday(&end_exec, NULL);

	// Calulating times for initialisation and execution
	float init_time = ((end_init.tv_sec * 1000000 + end_init.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec));
	float exec_time = ((end_exec.tv_sec * 1000000 + end_exec.tv_usec) - (end_init.tv_sec * 1000000 + end_init.tv_usec));

	init_time /= 1000000;
	exec_time /= 1000000;

	testSort(arr, ARR_SIZE);

	printf("\nTime taken to initialise: %fs\n", init_time);
	printf("Time taken to execute: %fs\n\n", exec_time);

	return(0);
}

int getRandNum(int min, int max)
{
	return (rand() % (max - min + 1)) + min;
}

// Function to return array of size 1 crore with random elements
int *getBigRandomArray()
{
	int *arr = (int*)malloc(ARR_SIZE * sizeof(int));

	// Note: Continuos memeory access is fairly faster.
	for(int i = 0; i < ARR_SIZE; i++)
		arr[i] = getRandNum(0, 10000000);

	return arr;
}

int *getTempArray(int size)
{
	return (int*)malloc(size * sizeof(int));
}

// Merge routine
void merge(int *arr, int l, int m, int r)
{
	int i, j, k;
	int n1 = m - l + 1;
	int n2 = r - m;
  
	/* create temp arrays */
	int *L = getTempArray(n1);
	int *R = getTempArray(n2);

	/* Copy data to temp arrays L[] and R[] */
	for (i = 0; i < n1; i++)
		L[i] = arr[l + i];
	
	for (j = 0; j < n2; j++)
		R[j] = arr[m + 1 + j];

	/* Merge the temp arrays back into arr[l..r]*/
	i = 0; // Initial index of first subarray
	j = 0; // Initial index of second subarray
	k = l; // Initial index of merged subarray

	while (i < n1 && j < n2)
	{
		if (L[i] <= R[j])
		{
			arr[k] = L[i];
			i++;
		}

		else
		{
			arr[k] = R[j];
			j++;
		}
		k++;
	}

	/* Copy the remaining elements of L[], if there
	are any */
	while (i < n1)
	{
		arr[k] = L[i];
		i++;
		k++;
	}
  
	/* Copy the remaining elements of R[], if there
	are any */
	while (j < n2) 
	{
		arr[k] = R[j];
		j++;
		k++;
	}

	free(L);
	free(R);
}

/* l is for left index and r is right index of the
sub-array of arr to be sorted */
void mergeSort(int *arr, int l, int r)
{
	if (l < r)
	{
		// Same as (l+r)/2, but avoids overflow for
		// large l and h
		int m = l + (r - l) / 2;

		// Sort first and second halves
		#pragma omp parallel 
		{
			mergeSort(arr, l, m);
			mergeSort(arr, m + 1, r);
		}
			merge(arr, l, m, r);
		// }
	}
}

// Function to test if array is sorted
void testSort(int *arr, int size)
{
	for(int i = 0; i < size - 1; i++)
	{
		if(arr[i] > arr[i + 1])
		{
			printf("ERROR: Array not sorted - i = %d, a[i] = %d, a[i+1] = %d\n", i, arr[i], arr[i+1]);
			// exit(-1);
			break;
		}
	}

	printf("Test Succeeded! Array sorted successfully!\n");
}