// omp_critical.cpp
// Specifies that code is only be executed on one thread at a time.
// compile with: gcc opm_atomic.cpp -fopenmp
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#define SIZE 10

int main()
{
    int i;
    int max;
    int a[SIZE];

    for (i = 0; i < SIZE; i++)
    {
        a[i] = rand();
        printf("%d\n", a[i]);
    }

    max = a[0];
    // parallel : The parallel construct creates a team of OpenMP threads that execute the region.
    // for directive instructs the compiler to distribute loop iterations within the team
    // of threads that encounters this work-sharing construct.
    // num_threads() : specify the number of threads to use when creating parallel regions
    #pragma omp parallel for num_threads(4)
        for (i = 1; i < SIZE; i++)
        {
            if (a[i] > max)
            {
                #pragma omp critical
                {
                    // compare a[i] and max again because max
                    // could have been changed by another thread after
                    // the comparison outside the critical section
                    if (a[i] > max)
                        max = a[i];
                }
            }
        }

    printf("max = %d\n", max);
}