// omp_reduction.cpp
// compile with: gcc omp_reduction.cpp -fopenmp
#include <stdio.h>
#include <omp.h>

#define NUM_THREADS 4
#define SUM_START   1
#define SUM_END     10
#define FUNC_RETS   {1, 1, 1, 1, 1}

int bRets[5] = FUNC_RETS;
int nSumCalc = ((SUM_START + SUM_END) * (SUM_END - SUM_START + 1)) / 2;

int func1( ) {return bRets[0];}
int func2( ) {return bRets[1];}
int func3( ) {return bRets[2];}
int func4( ) {return bRets[3];}
int func5( ) {return bRets[4];}

int main( )
{
    int nRet = 0,
        nCount = 0,
        nSum = 0,
        i,
        bSucceed = 1;

    omp_set_num_threads(NUM_THREADS);
    // omp_set_num_threads routine affects the number of threads 
    // to be used for subsequent parallel regions that do not 
    // specify a num_threads clause

    // #pragma omp parallel
    // The omp parallel directive explicitly instructs
    // the compiler to parallelize the chosen block of code.

    // reduction([ reduction-modifier,]reduction-identifier : list)
    // For each list item, a private copy is created in each implicit task or
    // SIMD lane and is initialized with the initializer value of the reduction-identifier.
    // After the end of the region, the original list item is updated with the values
    // of the private copies using the combiner associated with the reduction-identifier.
    // SIMD :  single instruction, multiple data
    #pragma omp parallel reduction(+ : nCount)
    {
        nCount += 1;
        // The omp for directive instructs the compiler to distribute loop
        // iterations within the team of threads that encounters this work-sharing construct.
        #pragma omp for reduction(+ : nSum)
        for (i = SUM_START ; i <= SUM_END ; ++i)
            nSum += i;
        // The sections construct is a non-iterative worksharing construct that
        // contains a set of structured blocks that are to be distributed among
        // and executed by the threads in a team. Each structured block is executed
        // once by one of the threads in the team in the context of its implicit task.
        #pragma omp sections reduction(&& : bSucceed)
        {
            #pragma omp section
            {
                bSucceed = bSucceed && func1( );
            }

            #pragma omp section
            {
                bSucceed = bSucceed && func2( );
            }

            #pragma omp section
            {
                bSucceed = bSucceed && func3( );
            }

            #pragma omp section
            {
                bSucceed = bSucceed && func4( );
            }

            #pragma omp section
            {
                bSucceed = bSucceed && func5( );
            }
        }
    }

    printf("The parallel section was executed %d times "
             "in parallel.\n", nCount);
    printf("The sum of the consecutive integers from "
             "%d to %d, is %d\n", 1, 10, nSum);

    if (bSucceed)
        printf("All of the functions, func1 through "
                 "func5 succeeded!\n");
    else
        printf("One or more of the functions, func1 "
                 "through func5 failed!\n");

    if (nCount != NUM_THREADS)
    {
        printf("ERROR: For %d threads, %d were counted!\n",
                 NUM_THREADS, nCount);
        nRet |= 0x1;
   }

    if (nSum != nSumCalc)
    {
        printf("ERROR: The sum of %d through %d should be %d, "
                "but %d was reported!\n",
                SUM_START, SUM_END, nSumCalc, nSum);
        nRet |= 0x10;
    }

    if (bSucceed != (bRets[0] && bRets[1] &&
                     bRets[2] && bRets[3] && bRets[4]))
    {
        printf("ERROR: The sum of %d through %d should be %d, "
                 "but %d was reported!\n",
                 SUM_START, SUM_END, nSumCalc, nSum);
        nRet |= 0x100;
    }
}