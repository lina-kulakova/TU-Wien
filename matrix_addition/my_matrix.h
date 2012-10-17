
#include <iostream>
#include <vector>

using std::cerr;
using std::cin;
using std::cout;
using std::endl;
using std::ostream;
using std::size_t;

template <typename E1, typename E2, typename T>
class matrix_sum;

template <typename E, typename T>
class matrix_expression   // base class for one matrix or matrices sum
{
public:
  T operator[] (size_t i) const { return static_cast<E const&> (*this)[i]; }

  size_t get_n_cols () const { return static_cast<E const&> (*this).get_n_cols (); }
  size_t get_n_rows () const { return static_cast<E const&> (*this).get_n_rows (); }

  operator E& () { return static_cast<E&> (*this); }
  operator E const& () const { return static_cast<const E&> (*this); }
};


template <typename T>
class my_matrix : public matrix_expression<my_matrix<T>, T>
{
private:
  T *data;
  size_t n_cols;
  size_t n_rows;

public:
   my_matrix (size_t cols, size_t rows)
     : n_cols (cols), n_rows (rows)
   {
     data = new (std::nothrow) T [n_cols * n_rows];
     if (!data)
       {
         cerr << "Not enough memory!" << endl;
         abort ();
       }
   }

   ~my_matrix () { delete [] data; }

   T& operator[] (size_t i) { return data[i]; }
   T operator[] (size_t i) const { return data[i]; }
   T& operator() (size_t i, size_t j) { return data[i * n_cols + j]; }
   T operator() (size_t i, size_t j) const { return data[i * n_cols + j]; }

   size_t get_n_cols () const { return n_cols; }
   size_t get_n_rows () const { return n_rows; }

  template <typename E>
  my_matrix<T> &operator= (const matrix_expression<E, T> &matrix_expr)
  {
    if (n_cols != matrix_expr.get_n_cols () || n_rows != matrix_expr.get_n_rows ())
      abort ();

    for (size_t i = 0; i < n_cols * n_rows; ++i)
      data[i] = matrix_expr[i];
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


template <typename E1, typename E2, typename T>
class matrix_sum : public matrix_expression<matrix_sum<E1, E2, T>, T>
{
private:
  E1 const &u;
  E2 const &v;

public:
  matrix_sum (matrix_expression<E1, T> const &u_arg, matrix_expression<E2, T> const &v_arg)
    : u (u_arg), v (v_arg)
  {
    if (u.get_n_cols () != v.get_n_cols () || u.get_n_rows () != v.get_n_rows ())
      abort ();
  }

  size_t get_n_cols () const { return v.get_n_cols (); }
  size_t get_n_rows () const { return v.get_n_rows (); }

  T operator[] (size_t i) const { return u[i] + v[i]; }
};


template <typename E1, typename E2, typename T>
matrix_sum<E1, E2, T> operator+ (matrix_expression<E1, T> const &u,
                                 matrix_expression<E2, T> const &v)
{
   return matrix_sum<E1, E2, T> (u,v);
}
