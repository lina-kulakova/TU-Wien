
#include "my_matrix.h"

typedef double NumericType;

void add_matrices_loop (my_matrix<NumericType> &m1, my_matrix<NumericType> &m2, my_matrix<NumericType> &m3,
                        my_matrix<NumericType> &m4, my_matrix<NumericType> &m5, size_t k_max, size_t s)
{
  double t = clock ();
  for (size_t k = 0; k < k_max; ++k)
    {
      for (size_t i = 0; i < s; ++i)
        for (size_t j = 0; j < s; ++j)
          m1(i,j) = m2(i,j) + m3(i,j) + m4(i,j) + m5(i,j);
    }
  t = clock () - t;
//  cout << m1 << endl;
  printf (Elapsed time ('for' loop): "%.2le\n", t / CLOCKS_PER_SEC / k_max);
}

void add_matrices_template (my_matrix<NumericType> &m1, my_matrix<NumericType> &m2, my_matrix<NumericType> &m3,
                            my_matrix<NumericType> &m4, my_matrix<NumericType> &m5, size_t k_max)
{
  double t = clock ();
  for (size_t k = 0; k < k_max; ++k)
    m1 = m2 + m3 + m4 + m5;
  t = clock () - t;
//      cout << m1 << endl;
  printf ("Elapsed time (my_matrix): %.2le\n", t / CLOCKS_PER_SEC / k_max);
}



int main (int argc, char *argv[])
{
  if (argc < 2)
    return -1;

  // setup:
  size_t s[] = {  10,   20,   30,   40,   50,   60,   70,   80,   90,
                 100,  200,  300,  400,  500,  600,  700,  800,  900,
                1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000};
  size_t k_max[] = { 1000000, 400000, 90000, 50000, 30000, 20000, 15000, 12000, 10000,
                        7000,   1000,   300,   200,   100,    80,    60,    50,    40,
                          30,      8,     4,     2,     1,     1,     1,     1,     1};
  size_t min_it = atoi (argv[1]), max_it = min_it + 1;
  for (size_t it = min_it; it < max_it; ++it)
    {
//      cout << "size = " << s[it] << endl;
      my_matrix<NumericType> m1(s[it], s[it]);
      my_matrix<NumericType> m2(s[it], s[it]);
      my_matrix<NumericType> m3(s[it], s[it]);
      my_matrix<NumericType> m4(s[it], s[it]);
      my_matrix<NumericType> m5(s[it], s[it]);

      /* fill m1, ..., m5 with values here */
      for (size_t i = 0; i < s[it]; i++)
        for (size_t j = 0; j < s[it]; j++)
          {
            m1(i, j) = 0.0;
            m2(i, j) = (i + j) / 2.0;
            m3(i, j) = (i + j) / 3.0;
            m4(i, j) = (i + j) / 4.0;
            m5(i, j) = (i + j) / 5.0;
          }
      add_matrices_loop (m1, m2, m3, m4, m5, k_max[it], s[it]);
      add_matrices_template (m1, m2, m3, m4, m5, k_max[it]);
//      cout << endl;
    }

  return 0;
}
