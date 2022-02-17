/*
	program to sort 10000000 (1 crore integers) elements using
	Merge sort technique, parallely.

	(Ideal) Memory usage for i crore integers = 4 crore bytes = 40 MB (in RAM)

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

	if(num_thread > max_threads)
	{
		printf("This computer has only %d vCPUs, less than %d as requested. Program will now terminate.\n", max_threads, num_thread);
		return -1;
	}

	if(!(((num_thread % 2 == 0) && (num_thread > 0) && (num_thread / 2 <= 8)) || (num_thread == 1)))
	{
		printf("Invalid number of threads requested! Please go through README.md");
		return -1;
	}

	// Function prototypes
	int *getBigRandomArray();

	int *arr = getBigRandomArray();

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
		arr[i] = getRandNum(0, 1000);

	return arr;
}