
#include <iostream>
#include <cstdlib>
#include <cstring>

using std::cerr;
using std::cout;
using std::endl;
using std::ostream;
using std::size_t;


template <typename E, typename T>
class matrix_expression
{
public:
  T operator() (size_t i, size_t j) const
  {
    return static_cast<E const&> (*this)(i,j);
  }

  size_t get_n_cols() const
  {
    return static_cast<E const&> (*this).get_n_cols ();
  }

  size_t get_n_rows() const
  {
    return static_cast<E const&> (*this).get_n_rows ();
  }

  operator E& () { return static_cast<E&> (*this); }
  operator E const& () const { return static_cast<const E&> (*this); }
};


template <typename T>
class my_matrix : public matrix_expression<my_matrix<T>, T>
{
private:
  T *data;
  size_t *indices; // indices[i] contains the index
                   // of i-th element of row layout in \code{data}
  size_t n_cols;
  size_t n_rows;

public:
   my_matrix(size_t rows, size_t cols)
     : n_cols(cols), n_rows(rows)
   {
     data = new (std::nothrow) T [n_cols * n_rows];
     indices = new (std::nothrow) size_t [n_cols * n_rows];
     if (!data || !indices)
       {
         cerr << "Not enough memory!" << endl;
         abort();
       }
     memset(data, 0, n_cols * n_rows * sizeof (T));
     memset(indices, -1, n_cols * n_rows * sizeof (size_t));
   }

   ~my_matrix()
   {
     delete [] data;
     delete [] indices;
   }

   T& operator() (size_t i, size_t j)
   {
     return data[indices[i * n_cols + j]];
   }

   T operator() (size_t i, size_t j) const
   {
     return data[indices[i * n_cols + j]];
   }

   size_t get_n_cols() const { return n_cols; }
   size_t get_n_rows() const { return n_rows; }

   void set_index (size_t i, size_t j, size_t k)
   {
     indices[i * n_cols + j] = k;
   }

   // operator= makes row layout for lhs matrix
   template <typename E>
   my_matrix<T> &operator= (const matrix_expression<E, T> &matrix_expr)
   {
     if (   n_cols != matrix_expr.get_n_cols()
         || n_rows != matrix_expr.get_n_rows())
       abort ();

     for (size_t i = 0; i < n_rows; ++i)
       for (size_t j = 0; j < n_cols; ++j)
         {
           size_t index = i * n_cols + j;
           data[index] = matrix_expr(i,j);
           indices[index] = index;
         }
     return *this;
   }

   friend ostream& operator<< (ostream& output, const my_matrix<T> &p)
   {
     size_t i, j;
     for (i = 0; i < p.n_rows; i++)
       {
         for (j = 0; j < p.n_cols; j++)
           output << p(i, j) << " ";
         output << endl;
       }
     return output;
   }
};


template <typename E1, typename E2, typename T>
class matrix_prod : public matrix_expression<matrix_prod<E1, E2, T>, T>
{
private:
  E1 const &u;
  E2 const &v;

public:
  matrix_prod (matrix_expression<E1, T> const &u_arg,
               matrix_expression<E2, T> const &v_arg)
    : u(u_arg), v(v_arg)
  {
    if (u.get_n_cols() != v.get_n_rows())
      abort();
  }

  size_t get_n_cols() const { return v.get_n_cols(); }
  size_t get_n_rows() const { return u.get_n_rows(); }

  T operator() (size_t i, size_t j) const
  {
    T res = 0;
    for (size_t k = 0; k < u.get_n_cols(); ++k)
      res += u(i,k) * v(k,j);
    return res;
  }
};


template <typename E1, typename E2, typename T>
matrix_prod<E1, E2, T> operator* (matrix_expression<E1, T> const &u,
                                  matrix_expression<E2, T> const &v)
{
   return matrix_prod<E1, E2, T> (u,v);
}

