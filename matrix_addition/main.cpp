
#include <ctime>

#include "my_matrix.h"

using std::cout;

int main ()
{
  typedef double NumericType;

  // setup:
  size_t s = 6000;
  cout << "s = " << s << endl;
  my_matrix<NumericType> m1(s, s);
  my_matrix<NumericType> m2(s, s);
  my_matrix<NumericType> m3(s, s);
  my_matrix<NumericType> m4(s, s);
  my_matrix<NumericType> m5(s, s);

  /* fill m1, ..., m5 with values here */
  for (size_t i = 0; i < s; i++)
    for (size_t j = 0; j < s; j++)
      {
        m2(i, j) = (i + j) / 2.0;
        m3(i, j) = (i + j) / 3.0;
        m4(i, j) = (i + j) / 4.0;
        m5(i,j) = (i + j) / 5.0;
      }

  double t;
  t = clock ();
  for (size_t i = 0; i < s; ++i)
    for (size_t j = 0; j < s; ++j)
      m1(i,j) = m2(i,j) + m3(i,j) + m4(i,j) + m5(i,j);
  t = clock () - t;
//  cout << m1 << std::endl;
  cout << "Elapsed time: " << t / CLOCKS_PER_SEC << endl;

  t = clock ();
  m1 = m2 + m3 + m4 + m5;
  t = clock () - t;
//  cout << m1 << std::endl;
  cout << "Elapsed time: " << t / CLOCKS_PER_SEC << endl;

  return 0;
}
