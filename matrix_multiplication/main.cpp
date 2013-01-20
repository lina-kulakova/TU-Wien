#include <stdio.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <pthread.h>
#include "my_matrix.h"

#define PRINT 0

/*Return astronomical time in seconds*/
double get_full_time()
{
  struct timeval buf;
  gettimeofday(&buf, 0);
  return (double)buf.tv_sec + ((double)buf.tv_usec) / 1000000;
}


// args: 1 -- iteration (0..27), 2 -- nthreads, 3 -- file name
int main (int argc, char *argv[])
{
  if (argc != 4)
    return -1;
  size_t it = atoi(argv[1]);
  size_t ns[] = {300, 400, 500, 600, 700, 800, 900,
                 1000, 1100, 1200, 1300, 1400, 1500,
                 1600, 1700, 1800, 1900, 2000, 2100,
                 2200, 2300, 2400, 2500, 2600, 2700,
                 2800, 2900, 3000};
  size_t ms[] = {300, 400, 500, 600, 700, 800, 900,
                 1000, 1100, 1200, 1300, 1400, 1500,
                 1600, 1700, 1800, 1900, 2000, 2100,
                 2200, 2300, 2400, 2500, 2600, 2700,
                 2800, 2900, 3000};
  size_t ps[] = {300, 400, 500, 600, 700, 800, 900,
                 1000, 1100, 1200, 1300, 1400, 1500,
                 1600, 1700, 1800, 1900, 2000, 2100,
                 2200, 2300, 2400, 2500, 2600, 2700,
                 2800, 2900, 3000};
  size_t iters [] = /*{700, 600, 500, 400, 300, 200, 150, 90, 70,
                     60, 50, 45, 40, 35, 30, 25, 20, 15, 10, 5,
                     4, 3, 2, 1, 1, 1, 1, 1};*/
                    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

  size_t n = ns[it], m = ms[it], p = ps[it];
  my_matrix A(n,m), B(m,p), C(n,p);

  // fill matrices
  for (size_t i = 0; i < n; ++i)
    for (size_t j = 0; j < m; ++j)
      {
        // on what place in array should A_{i,j} be
        A.set_index(i, j, i * m + j);
        A(i, j) = 0.1 * (i + j);
      }
#if PRINT
  cout << "A:" << endl;
  cout << A << endl;
#endif

  for (size_t i = 0; i < m; ++i)
    for (size_t j = 0; j < p; ++j)
      {
        B.set_index(i, j, p * m - 1 - (i * p + j));
        B(i, j) = 0.2 * (i + j);
      }
#if PRINT
  cout << "B:" << endl;
  cout << B << endl;
#endif

  size_t nthreads = atoi(argv[2]);
  args *arguments = new args[nthreads];
  for (size_t i = 0; i < nthreads; ++i)
    {
      arguments[i].a = &A;
      arguments[i].b = &B;
      arguments[i].c = &C;
      arguments[i].total_threads = nthreads;
      arguments[i].thread_num = i;
    }

  pthread_t *threads = new pthread_t[nthreads];
  double t = get_full_time();
  // multiply matrices
  for (size_t i = 0; i < iters[it]; ++i)
    {
      for (i = 0; i < nthreads; i++)
        {
          if (pthread_create(threads + i, 0, mult_matrix_matrix,
              arguments + i))
            {
              printf("Cannot create thread #%d!\n", i);
              return -1;            
            }
        }
      for (i = 0; i < nthreads; i++)
        {
          if (pthread_join(threads[i], 0))
            printf("Cannot wait thread #%d!\n", i);            
        }
    }
  t = get_full_time() - t;

  // print answer
#if PRINT
  cout << "C:" << endl;
  cout << C << endl;
#endif
  FILE *fp = fopen (argv[3], "a");
  fprintf (fp, "%d %d %lf\n", nthreads, ns[it], t / iters[it]);
  fclose (fp);

  delete [] arguments;
  return 0;
}

