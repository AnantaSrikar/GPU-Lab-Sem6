# -*- MakeFile -*-

all: multi-threadded_matrix_multi merge-sort

multi-threadded_matrix_multi: multi-threadded_matrix_multi.c
	gcc multi-threadded_matrix_multi.c -o multi-threadded_matrix_multi -fopenmp

merge-sort: merge-sort.c
	gcc merge-sort.c -o merge-sort -fopenmp

clean:
	rm -fr *.o multi-threadded_matrix_multi merge-sort *output*