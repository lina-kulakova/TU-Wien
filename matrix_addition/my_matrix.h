
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
  T *data;           // matrix elements
  vector<const my_matrix<T>*> matrices; // vector with pointers to summands

public:
  my_matrix () : n_rows (0), n_cols (0), data (0) { }
  my_matrix (const my_matrix<T> &b);
  my_matrix (const my_matrix<T> &a, const my_matrix<T> &b);
  my_matrix (size_t n_rows_arg, size_t n_cols_arg);
  ~my_matrix ();

  inline T& operator() (size_t i, size_t j);
  inline T operator() (size_t i, size_t j) const;
  inline my_matrix<T> &operator= (const my_matrix<T> &b);
  inline my_matrix<T> &operator= (const my_matrix<T> &&a);

  friend ostream& operator<< (ostream& output, const my_matrix<T> &p)
  {
    size_t i, j;
    for (i = 0; i < p.n_rows; i++)
      {
        T *pdata = p.data + i * p.n_cols;
        for (j = 0; j < p.n_cols; j++)
          output << pdata[j] << " ";
        output << endl;
      }
    return output;
  }
};


template <typename T>
my_matrix<T>::my_matrix (const my_matrix<T> &b)
  : n_rows (b.n_rows), n_cols (b.n_cols)
{
  data = new (std::nothrow) T [n_rows * n_cols];
  if (!data)
    {
      cerr << "Error: not enough memory." << endl;
      abort ();
    }
  memcpy (data, b.data, n_rows * n_cols * sizeof (T));
}


template <typename T>
my_matrix<T>::my_matrix (const my_matrix<T> &a, const my_matrix<T> &b)
{
  matrices.push_back (&a);
  matrices.push_back (&b);
}


template <typename T>
my_matrix<T>::my_matrix (size_t n_rows_arg, size_t n_cols_arg)
  : n_rows (n_rows_arg), n_cols (n_cols_arg)
{
  data = new (std::nothrow) T [n_rows * n_cols];
  if (!data)
    {
      cerr << "Error: not enough memory." << endl;
      abort ();
    }
}


template <typename T>
my_matrix<T>::~my_matrix ()
{
  delete [] data;
  data = 0;
  n_rows = 0;
  n_cols = 0;
}


template <typename T> inline
T& my_matrix<T>::operator() (size_t i, size_t j)
{
  return data[i * n_cols + j];
}


template <typename T> inline
T my_matrix<T>::operator() (size_t i, size_t j) const
{
  return data[i * n_cols + j];
}


template <typename T> inline
my_matrix<T> &my_matrix<T>::operator= (const my_matrix<T> &b)
{
  if (this == &b)
    return *this;

  n_rows = b.n_rows;
  n_cols = b.n_cols;
  memcpy (data, b.data, n_rows * n_cols * sizeof (T));
  return *this;
}

template <typename T> inline
my_matrix<T> &my_matrix<T>::operator= (const my_matrix<T> &&a)
{
  vector <const my_matrix<T>*> matrices = a.matrices;
  for (size_t i = 0; i < n_rows; ++i)
    for (size_t j = 0; j < n_cols; ++j)
      {
        T s = 0;
        for (size_t k = 0; k < matrices.size (); ++k)
          s += matrices[k]->data[i * n_cols + j];
        data[i * n_cols + j] = s;
      }

  return *this;
}


template <typename T> inline
my_matrix<T> operator+ (const my_matrix<T> &a, const my_matrix<T> &b)
{
  return my_matrix<T> (a, b);
}

template <typename T> inline
my_matrix<T> operator+ (my_matrix<T> &&a, const my_matrix<T> &b)
{
  a.matrices.push_back (&b);
  return a;
}
