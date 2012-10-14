
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

template <typename T, size_t size>
class my_matrix_temporary;

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

public:
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

  template<size_t size>
  my_matrix_temporary<T, size> operator+ (const my_matrix<T> &b)
  {
    return my_matrix_temporary<T, size> (*this, b);
  }

  template<size_t size, size_t matrix_size>
  my_matrix<T> &operator= (const my_matrix_temporary<T, size> &a)
  {
    for (size_t i = 0; i < matrix_size; ++i)
      {
        T s = 0;
        for (size_t k = 0; k < size; ++k)
          s += (a.summands[k])[i];
        data[i] = s;
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



////////////////////////////////////////////////
//           class my_matrix_temporary
////////////////////////////////////////////////

template<typename T, size_t size>
class my_matrix_temporary
{
public:
  T *summands[size];
  size_t current_size;

public:
  my_matrix_temporary (const my_matrix<T> &a, const my_matrix<T> &b)
  {
    if (b.n_cols != a.n_cols || b.n_rows != a.n_rows)
      abort ();

    summands[0] = a.data;
    summands[1] = b.data;
    current_size = 2;
  }

  my_matrix_temporary<T, size> operator+ (const my_matrix<T> &b)
  {
    summands[current_size++] = b.data;
    return *this;
  }
};
