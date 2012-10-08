
#include <ctime>

#include "my_matrix.h"

using std::cout;

int main ()
{
  typedef double NumericType;

  // setup:
  size_t iters = 23;
  size_t s[] = {  10,   20,   30,   40,   50,  60,  70,  80,  90,
                 100,  200,  300,  400,  500, 600, 700, 800, 900,
                1000, 2000, 3000, 4000, 5000};
  size_t k_max[] = { 500000, 200000, 90000, 50000, 30000, 20000, 15000, 12000, 10000,
                       7000,   1000,   300,   200,   100,    80,    60,    50,    40,
                         30,      8,     4,     2,     1};
  size_t k;
  double t;
  for (size_t it = 0; it < iters; ++it)
    {
      my_matrix<NumericType> m1(s[it], s[it]);
      my_matrix<NumericType> m2(s[it], s[it]);
      my_matrix<NumericType> m3(s[it], s[it]);
      my_matrix<NumericType> m4(s[it], s[it]);
      my_matrix<NumericType> m5(s[it], s[it]);

      /* fill m1, ..., m5 with values here */
      for (size_t i = 0; i < s[it]; i++)
        for (size_t j = 0; j < s[it]; j++)
          {
            m2(i, j) = (i + j) / 2.0;
            m3(i, j) = (i + j) / 3.0;
            m4(i, j) = (i + j) / 4.0;
            m5(i, j) = (i + j) / 5.0;
          }

      cout << "size = " << s[it] << endl;
      t = clock ();
      for (k = 0; k < k_max[it]; ++k)
        {
          for (size_t i = 0; i < s[it]; ++i)
            for (size_t j = 0; j < s[it]; ++j)
              m1(i,j) = m2(i,j) + m3(i,j) + m4(i,j) + m5(i,j);
        }
      t = clock () - t;
      //  cout << m1 << endl;
      cout << "Elapsed time ('for' loop): " << t / CLOCKS_PER_SEC << endl;

      t = clock ();
      for (k = 0; k < k_max[it]; ++k)
        m1 = m2 + m3 + m4 + m5;
      t = clock () - t;
      //  cout << m1 << endl;
      cout << "Elapsed time (my_matrix):  " << t / CLOCKS_PER_SEC << endl;

      cout << endl;
    }

  return 0;
}
