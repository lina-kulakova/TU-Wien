#include "my_matrix.h"
#include <stdio.h>
#include <ctime>

#define PRINT 0

int main (int argc, char *argv[])
{
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
  size_t iters [] = {700, 600, 500, 400, 300, 200, 150, 90, 70,
                     60, 50, 45, 40, 35, 30, 25, 20, 15, 10, 5,
                     4, 3, 2, 1, 1, 1, 1, 1};
                   /* {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};*/

  size_t n = ns[it], m = ms[it], p = ps[it];
  my_matrix<double> A(n,m), B(m,p), C(n,p);

  // fill matrices
  for (size_t i = 0; i < n; ++i)
    for (size_t j = 0; j < m; ++j)
      {
        // on what place in array should A_{i,j} be
        A.set_index(i, j, i * m + j);
        A(i, j) = i + j;
      }
#if PRINT
  cout << A << endl;
#endif

  for (size_t i = 0; i < m; ++i)
    for (size_t j = 0; j < p; ++j)
      {
        B.set_index(i, j, p * m - 1 - (i * p + j));
        B(i, j) = i + j;
      }
#if PRINT
  cout << B << endl;
#endif

  double t = clock();
  // multiply matrices
  for (size_t i = 0; i < iters[it]; ++i)
    C = A * B;
  t = clock() - t;

  // print answer
#if PRINT
  cout << C << endl;
#endif
  FILE *fp = fopen (argv[2], "a");
  fprintf (fp, "%d %lf\n", ns[it], t / CLOCKS_PER_SEC / iters[it]);
  fclose (fp);

  return 0;
}
