
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <vector>

using std::cerr;
using std::cin;
using std::cout;
using std::endl;
using std::ostream;
using std::size_t;
using std::vector;


////////////////////////////////////////////////
//           class my_matrix
////////////////////////////////////////////////

template <typename T>
class my_matrix
{
public:
  size_t n_rows;     // number of rows
  size_t n_cols;     // number of columns
  T *data;
  vector<T*> summands;

public:
  my_matrix (const my_matrix<T> &a, const my_matrix<T> &b)
  {
    if (b.n_cols != a.n_cols || b.n_rows != a.n_rows)
      abort ();

    data = 0;
    summands.push_back (a.data);
    summands.push_back (b.data);
  }

  my_matrix (size_t n_rows_arg, size_t n_cols_arg)
    : n_rows (n_rows_arg), n_cols (n_cols_arg)
  {
    data = new (std::nothrow) T [n_rows * n_cols];
    if (!data)
      {
        cerr << "Error: not enough memory." << endl;
        abort ();
      }
  }

  ~my_matrix ()
  {
    delete [] data;
  }

  T& operator() (size_t i, size_t j)
  {
    return data[i * n_cols + j];
  }

  T operator() (size_t i, size_t j) const
  {
    return data[i * n_cols + j];
  }

  my_matrix<T> &operator= (const my_matrix<T> &&a)
  {
    if (n_cols * n_rows < 40000)
      {
        for (size_t i = 0; i < n_rows * n_cols; ++i)
          data[i] = (a.summands[0])[i];
        for (size_t k = 1; k < a.summands.size (); ++k)
          for (size_t i = 0; i < n_rows * n_cols; ++i)
            data[i] += (a.summands[k])[i];
      }
    else
      {
        for (size_t i = 0; i < n_rows * n_cols; ++i)
          {
            T s = 0;
            for (size_t k = 0; k < a.summands.size (); ++k)
              s += (a.summands[k])[i];
            data[i] = s;
          }
      }
    return *this;
  }

  friend ostream& operator<< (ostream& output, const my_matrix<T> &p)
  {
    size_t i, j;
    for (i = 0; i < p.n_rows; i++)
      {
        for (j = 0; j < p.n_cols; j++)
          output << p.data[i * p.n_cols + j] << " ";
        output << endl;
      }
    return output;
  }
};




template <typename T> inline
my_matrix<T> operator+ (const my_matrix<T> &a, const my_matrix<T> &b)
{
  return my_matrix<T> (a, b);
}

template <typename T> inline
my_matrix<T> operator+ (my_matrix<T> &&a, const my_matrix<T> &b)
{
  a.summands.push_back (b.data);
  return a;
}
