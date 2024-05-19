// omp_for.cpp
// Causes the work done in a for loop inside a parallel region to be divided among threads.
// compile with: gcc omp_for.cpp -fopenmp
#include <stdio.h>
#include <math.h>
#include <omp.h>

#define NUM_THREADS 4
#define NUM_START 1
#define NUM_END 10

int main() {
   int i, nRet = 0, nSum = 0, nStart = NUM_START, nEnd = NUM_END;
   int nThreads = 0, nTmp = nStart + nEnd;
   unsigned uTmp = (unsigned((abs(nStart - nEnd) + 1)) *
                               unsigned(abs(nTmp))) / 2;
   int nSumCalc = uTmp;

   if (nTmp < 0)
      nSumCalc = -nSumCalc;
   // omp_set_num_threads 
   // routine affects the number of threads to be used for
   // subsequent parallel regions that do not specify a num_threads clause
   omp_set_num_threads(NUM_THREADS);

   // #pragma omp parallel directive in OpenMP is used to create a parallel region 
   // Within a parallel region, OpenMP creates multiple threads (processes)
   // to execute the code concurrently.

   // default
   // sets the data-sharing policy to apply to variables that have not
   // been explicitly assigned one
   // none: no policy will be applied by default

   // private
   // variables that each thread in the OpenMP parallel region will have a copy of. 

   // shared
   // variables shared across the threads belonging to the OpenMP parallel region concerned.
   #pragma omp parallel default(none) private(i) shared(nSum, nThreads, nStart, nEnd)
   {
      #pragma omp master
      nThreads = omp_get_num_threads();
      // The omp master directive identifies a section of code that
      // must be run only by the master thread.

      // OpenMP distribute the iterations of the loop across multiple threads
      #pragma omp for
      for (i=nStart; i<=nEnd; ++i) {
            // the code following this directive should be treated as an atomic operation.
            #pragma omp atomic
            nSum += i;
      }
   }

   if  (nThreads == NUM_THREADS) {
      printf("%d OpenMP threads were used.\n", NUM_THREADS);
      nRet = 0;
   }
   else {
      printf("Expected %d OpenMP threads, but %d were used.\n",
               NUM_THREADS, nThreads);
      nRet = 1;
   }

   if (nSum != nSumCalc) {
      printf("The sum of %d through %d should be %d, "
               "but %d was reported!\n",
               NUM_START, NUM_END, nSumCalc, nSum);
      nRet = 1;
   }
   else
      printf("The sum of %d through %d is %d\n",
               NUM_START, NUM_END, nSum);
}
