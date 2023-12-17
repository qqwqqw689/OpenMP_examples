// openmp_private.c
// compile with: gcc openmp_private.c -fopenmp
#include <assert.h>
#include <stdio.h>
#include <omp.h>
#include <unistd.h>

#define NUM_THREADS 4
#define SLEEP_THREAD 1
#define NUM_LOOPS 2

enum Types {
   ThreadPrivate,
   Private,
   FirstPrivate,
   LastPrivate,
   Shared,
   MAX_TYPES
};

int nSave[NUM_THREADS][MAX_TYPES][NUM_LOOPS] = {{0}};
int nThreadPrivate;

#pragma omp threadprivate(nThreadPrivate)
// The threadprivate directive specifies that variables
// are replicated, with each thread having its own copy. 

int main() {
    int nPrivate = NUM_THREADS;
    int nFirstPrivate = NUM_THREADS;
    int nLastPrivate = NUM_THREADS;
    int nShared = NUM_THREADS;
    int nRet = 0;
    int i;
    int j;
    int nLoop = 0;

    nThreadPrivate = NUM_THREADS;
    printf("These are the variables before entry "
            "into the parallel region.\n");
    printf("nThreadPrivate = %d\n", nThreadPrivate);
    printf("      nPrivate = %d\n", nPrivate);
    printf(" nFirstPrivate = %d\n", nFirstPrivate);
    printf("  nLastPrivate = %d\n", nLastPrivate);
    printf("       nShared = %d\n\n", nShared);
    omp_set_num_threads(NUM_THREADS);

    // The omp parallel directive explicitly instructs the
    // compiler to parallelize the chosen block of code.

    // copyin 
    // a mechanism to broadcast the value of a threadprivate variable
    // from the master thread to the threadprivate variable of every other thread
    // in the team executing the parallel region.

    // private
    // The private clause declares the variables in the list to be private
    // to each thread in a team.

    // shared
    // The shared clause declares the variables in the list to be
    // shared among all the threads in a team.
    // All threads within a team access the same storage area for shared variables.
    
    // firstprivate
    // Specifies that each thread should have its own instance of a variable, and
    // that the variable should be initialized with the value of the variable, because
    // it exists before the parallel construct.
    #pragma omp parallel copyin(nThreadPrivate) private(nPrivate) shared(nShared) firstprivate(nFirstPrivate)
    {
        #pragma omp for schedule(static) lastprivate(nLastPrivate)
        for (i = 0 ; i < NUM_THREADS ; ++i) {
           for (j = 0 ; j < NUM_LOOPS ; ++j) {
              int nThread = omp_get_thread_num();
              // The omp_get_thread_num function returns the number of
              // the currently executing thread within the team. 
              assert(nThread < NUM_THREADS);

              if (nThread == SLEEP_THREAD)
                  sleep(3);
                nSave[nThread][ThreadPrivate][j] = nThreadPrivate;
                nSave[nThread][Private][j] = nPrivate;
                nSave[nThread][Shared][j] = nShared;
                nSave[nThread][FirstPrivate][j] = nFirstPrivate;
                nSave[nThread][LastPrivate][j] = nLastPrivate;
                nThreadPrivate = nThread;
                nPrivate = nThread;
                nShared = nThread;
                nLastPrivate = nThread;
                --nFirstPrivate;
            }
        }
    }

    for (i = 0 ; i < NUM_LOOPS ; ++i) {
        for (j = 0 ; j < NUM_THREADS ; ++j) {
            printf("These are the variables at entry of "
                    "loop %d of thread %d.\n", i + 1, j);
            printf("nThreadPrivate = %d\n",
                    nSave[j][ThreadPrivate][i]);
            printf("      nPrivate = %d\n",
                    nSave[j][Private][i]);
            printf(" nFirstPrivate = %d\n",
                    nSave[j][FirstPrivate][i]);
            printf("  nLastPrivate = %d\n",
                    nSave[j][LastPrivate][i]);
            printf("       nShared = %d\n\n",
                    nSave[j][Shared][i]);
        }
    }

    printf("These are the variables after exit from "
             "the parallel region.\n");
    printf("nThreadPrivate = %d (The last value in the "
              "main thread)\n", nThreadPrivate);
    printf("      nPrivate = %d (The value prior to "
            "entering parallel region)\n", nPrivate);
    printf(" nFirstPrivate = %d (The value prior to "
              "entering parallel region)\n", nFirstPrivate);
    printf("  nLastPrivate = %d (The value from the "
             "last iteration of the loop)\n", nLastPrivate);
    printf("       nShared = %d (The value assigned, "
            "from the delayed thread, %d)\n\n",
              nShared, SLEEP_THREAD);
}