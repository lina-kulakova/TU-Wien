
#include <cstdio>
#include <cstdlib>
#include <iostream>

using std::cerr;
using std::endl;
using std::ostream;
using std::size_t;

template <typename T>
class my_matrix_temporary;

template <typename T>
class my_list;

template <typename T>
class my_list_node;


////////////////////////////////////////////////
//           class my_matrix
////////////////////////////////////////////////

template <typename T>
class my_matrix
{
private:
  size_t n_rows;     // number of rows
  size_t n_cols;     // number of columns
  T *data;           // matrix elements

public:
  my_matrix ()
    : n_rows (0), n_cols (0), data (0) { }

  my_matrix (const my_matrix<T> &b);
  my_matrix (size_t n_rows_arg, size_t n_cols_arg);
  ~my_matrix ();

  inline T& operator() (size_t i, size_t j);
  inline T operator() (size_t i, size_t j) const;
  inline my_matrix<T> &operator= (const my_matrix<T> &b);
  inline my_matrix<T> &operator= (my_matrix_temporary<T> &b);

  my_matrix_temporary<T> operator+ (const my_matrix<T> &b)
  {
    return my_matrix_temporary<T> (*this, b);
  }

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


template <typename T>
inline T& my_matrix<T>::operator() (size_t i, size_t j)
{
  return data[i * n_cols + j];
}


template <typename T>
inline T my_matrix<T>::operator() (size_t i, size_t j) const
{
  return data[i * n_cols + j];
}


template <typename T>
inline my_matrix<T> &my_matrix<T>::operator= (const my_matrix<T> &b)
{
  if (this == &b)
    return *this;

  n_rows = b.n_rows;
  n_cols = b.n_cols;
  *data = *b.data;
  return *this;
}


template <typename T>
inline my_matrix<T> &my_matrix<T>::operator= (my_matrix_temporary<T> &b)
{
  my_list_node< my_matrix<T> > *it;
  for (size_t i = 0; i < n_rows; ++i)
    for (size_t j = 0; j < n_cols; ++j)
      {
        T s = 0;
        for (it = b.matrix_list.begin; it != 0; it = it->next)
          s += it->data.data[i * n_cols + j];
        data[i * n_cols + j] = s;
      }

  return *this;
}



////////////////////////////////////////////////
//         class my_matrix_temporary
////////////////////////////////////////////////

template <typename T>
class my_matrix_temporary
{
private:
  my_list< my_matrix<T> > matrix_list;

public:
  my_matrix_temporary (const my_matrix<T> &a, const my_matrix<T> &b)
  {
    matrix_list.insert (a);
    matrix_list.insert (b);
  }

  my_matrix_temporary<T> &operator+ (const my_matrix<T> &b)
  {
    matrix_list.insert (b);
    return *this;
  }

  friend class my_matrix<T>;
};



////////////////////////////////////////////////
//         class my_list_node
////////////////////////////////////////////////

template <typename T>
class my_list_node
{
public:
  const T &data;
  my_list_node<T> *next;

public:
  my_list_node (const T &data_arg)
    : data (data_arg), next (0) { }
};



////////////////////////////////////////////////
//         class my_list
////////////////////////////////////////////////

template <typename T>
class my_list
{
public:
  my_list_node<T> *begin;

public:
  my_list ()
    : begin (0) { }

  ~my_list ()
  {
    my_list_node<T> *it, *next;
    for (it = begin, next = begin->next; it != 0; it = next)
      {
        next = it->next;
        delete it;
      }
  }

  void insert (const T& new_data)
  {
    my_list_node<T> *new_node = new my_list_node<T> (new_data);
    if (begin)
      {
        my_list_node<T> *next = begin->next;
        begin->next = new_node;
        new_node->next = next;
      }
    else
      begin = new_node;
  }
};
