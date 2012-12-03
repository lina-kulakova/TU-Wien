#include "my_matrix.h"
#include <ctime>

#define PRINT 0

int main ()
{
  size_t n = 1000, m = 1000, p = 1000;
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
  C = A * B;
  t = clock() - t;

  // print answer
#if PRINT
  cout << C << endl;
#endif
  cout << "Elapsed time: " << t / CLOCKS_PER_SEC << endl;

  return 0;
}
