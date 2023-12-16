// omp_flush.cpp
// Specifies that all threads have the same view of memory for all shared objects.
// compile with: gcc omp_flush.cpp -fopenmp
#include <stdio.h>
#include <omp.h>

void read(int *data) {
   printf("read data\n");
   *data = 1;
}

void process(int *data) {
   printf("process data\n");
   (*data)++;
}

int main() {
   int data;
   int flag;

   flag = 0;
   // The omp parallel directive explicitly instructs the
   // compiler to parallelize the chosen block of code.
   // The omp sections directive distributes work among 
   // threads bound to a defined parallel region.
   // num_threads
   // specify the number of threads to use when creating parallel regions.
   #pragma omp parallel sections num_threads(2)
   {
      // Each section is executed once by a thread in the team.
      #pragma omp section
      {
         printf("Thread %d: ", omp_get_thread_num( ));
         // The omp_get_thread_num routine returns the thread number,
         // within the current team, of the calling thread
         read(&data);
         #pragma omp flush(data)
         flag = 1;
         #pragma omp flush(flag)
         // Do more work.
      }

      #pragma omp section
      {
         while (!flag) {
            #pragma omp flush(flag)
         }
         #pragma omp flush(data)

         printf("Thread %d: ", omp_get_thread_num( ));
         process(&data);
         printf("data = %d\n", data);
      }
   }
}