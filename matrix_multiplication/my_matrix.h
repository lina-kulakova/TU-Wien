
#include <iostream>
#include <cstdlib>
#include <cstring>

using std::cerr;
using std::cout;
using std::endl;
using std::ostream;
using std::size_t;

#define BLOCK_SIZE 80
#define TEST 0
#define MULT1 0

template <typename T>
class my_matrix
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
     for (size_t index = 0; index < n_cols * n_rows; ++index)
       indices[index] = index;
//     memset(indices, -1, n_cols * n_rows * sizeof (size_t));
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
   T *get_data() const {return data; }
   T *get_indices() const { return indices; }

   void set_index (size_t i, size_t j, size_t k)
   {
     indices[i * n_cols + j] = k;
   }

   // operator= makes row layout for lhs matrix
   my_matrix<T> &operator= (const my_matrix<T> &rhs)
   {
     if (   n_cols != rhs.get_n_cols()
         || n_rows != rhs.get_n_rows())
       abort ();

     for (size_t i = 0; i < n_rows; ++i)
       for (size_t j = 0; j < n_cols; ++j)
         {
           size_t index = i * n_cols + j;
           data[index] = rhs(i,j);
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


template <typename T>
my_matrix<T> operator* (my_matrix<T> const &a,
                        my_matrix<T> const &b)
{
  if (a.get_n_cols() != b.get_n_rows())
    abort();

  size_t a_rows = a.get_n_rows();
  size_t b_cols = b.get_n_cols();
  my_matrix<T> c (a_rows, b_cols);

  size_t a_cols = a.get_n_cols();
#if !TEST
  size_t m = BLOCK_SIZE;
  size_t i, j, k, m1, m2, m3;
  // loop over matrix rows
  for(i = 0; i < a_rows; i += m)
    {
      m1 = (a_rows - i < m) ? a_rows - i : m;
      // block a_{i,k} with m1 rows

      // loop over matrix cols
      for (j = 0; j < b_cols; j += m)
        {
          m3 = (b_cols - j < m) ? b_cols - j : m;
          // block b_{k,j} with m3 columns

          // make a \sum_{k=0}^{K} a_{i,k}*b_{k,j}
          for (k = 0; k < a_cols; k += m)
            {
              m2 = (a_cols - k < m) ? a_cols - k : m;
              // cols(a_{i,k}) = rows(b_{k,j}) = m2
              multiply_and_add_blocks(a, b, c, i, j, k, m1, m2, m3);
            }
        }
    }
#else
  for (size_t i = 0; i < a_rows; ++i)
    for (size_t j = 0; j < b_cols; ++j)
      for (size_t k = 0; k < a_cols; ++k)
        c(i,j) += a(i,k) * b(k,j);
#endif
  return c;
}

template <typename T>
void multiply_and_add_blocks(
  my_matrix<T> const &a, my_matrix<T> const &b, my_matrix<T> &c,
  size_t mi, size_t mj, size_t mk,
  size_t m, size_t n, size_t p
)
{
  size_t inda1 = mi;
  size_t inda2 = mk;
  size_t indb1 = mk;
  size_t indb2 = mj;
  size_t indc1 = mi;
  size_t indc2 = mj;
#if MULT1
  for (size_t i = 0; i < m; ++i)
    for (size_t j = 0; j < p; ++j)
      for (size_t k = 0; k < n; ++k)
        c(indc1 + i, indc2 + j) += a(inda1 + i, inda2 + k) *
                                   b(indb1 + k, indb2 + j);
#else
  size_t i, j, k;
  T s00 = 0., s01 = 0., s10 = 0., s11 = 0.;

  // count c_ij
  for (i = 0; i < m; i += 2)  // loop over rows of a
    {
      if (m - i == 1)   // m1 = 1
        {
          for (j = 0; j < p; j += 2)  // loop over columns of b
            {
              s00 = 0.;
              s01 = 0.;
              s10 = 0.;
              s11 = 0.;

              if (p - j == 1)   // m3 = 1
                {
                  for (k = 0; k < n; k ++)
                    {
                      s00 += a(inda1 + i, inda2 + k)
                           * b(indb1 + k, indb2 + j);
                    }
                  c(indc1 + i, indc2 + j) += s00;
                }
              else   // m3 = 2
                {
                  for (k = 0; k < n; ++k)
                    {
                      s00 += a(inda1 + i, inda2 + k)
                           * b(indb1 + k, indb2 + j);
                      s01 += a(inda1 + i, inda2 + k)
                           * b(indb1 + k, indb2 + j + 1);
                    }
                  c(indc1 + i, indc2 + j) += s00;
                  c(indc1 + i, indc2 + j + 1) += s01;
                }
            }
        }
      else   // m1 = 2
        {
          for (j = 0; j < p; j += 2)  // loop over columns of b
            {
              s00 = 0.;
              s01 = 0.;
              s10 = 0.;
              s11 = 0.;

              if (p - j == 1)   // m3 = 1
                {
                  for (k = 0; k < n; ++k)
                    {
                      s00 += a(inda1 + i, inda2 + k)
                           * b(indb1 + k, indb2 + j);
                      s10 += a(inda1 + i, inda2 + k + n)
                           * b(indb1 + k, indb2 + j);
                    }
                  c(indc1 + i, indc2 + j) += s00;
                  c(indc1 + i, indc2 + j + p) += s10;
                }
              else   // m3 = 2
                {
                  for (k = 0; k < n; ++k)
                    {
                      s00 += a(inda1 + i, inda2 + k)
                           * b(indb1 + k, indb2 + j);
                      s01 += a(inda1 + i, inda2 + k)
                           * b(indb1 + k, indb2 + j + 1);
                      s10 += a(inda1 + i, inda2 + k + n)
                           * b(indb1 + k, indb2 + j);
                      s11 += a(inda1 + i, inda2 + k + n)
                           * b(indb1 + k, indb2 + j + 1);
                    }
                  c(indc1 + i, indc2 + j) += s00;
                  c(indc1 + i, indc2 + j + 1) += s01;
                  c(indc1 + i, indc2 + j + p) += s10;
                  c(indc1 + i, indc2 + j + p + 1) += s11;
                }
            }
        }
    }
#endif
}

