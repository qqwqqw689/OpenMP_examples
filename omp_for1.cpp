#include <stdio.h>
#include "omp.h"

int main()
{
    omp_set_num_threads(1000);
	#pragma omp parallel for
	    for(int i=0; i < 100; i++)
	    {
	    	printf("i: %d, threadid: %d\n", i, omp_get_thread_num());
	    }
    
    printf("max_threads: %d\n", omp_get_max_threads());
    
	return 0;
}