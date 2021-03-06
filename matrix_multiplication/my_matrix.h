
#include <iostream>
#include <cstdlib>
#include <cstring>

using std::cerr;
using std::cout;
using std::endl;
using std::ostream;
using std::size_t;

#define BLOCK_SIZE 80
#define BLOCK_MULT 1
#define BLOCK_BLOCK_MULT 1
#define UNROLLED 0

void synchronize_threads (int total_threads)
{
  static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
  static pthread_cond_t condvar_in = PTHREAD_COND_INITIALIZER;  
  static pthread_cond_t condvar_out = PTHREAD_COND_INITIALIZER;
  static int threads_in = 0;
  static int threads_out = 0;
  pthread_mutex_lock (&mutex);
  threads_in++;
  if (threads_in >= total_threads)
    {
      threads_out = 0;
      pthread_cond_broadcast (&condvar_in);
    }
  else
    {
      while (threads_in < total_threads)
        {
          pthread_cond_wait (&condvar_in, &mutex);
        }
    }
  threads_out++;
  if (threads_out >= total_threads)
    {
      threads_in = 0;
      pthread_cond_broadcast (&condvar_out);
    }
  else
    {
      while (threads_out < total_threads)
        {
          pthread_cond_wait (&condvar_out, &mutex);
        }
    }
  pthread_mutex_unlock (&mutex);
}


void calc_thread_own_items (size_t total_threads, size_t thread_num,
                            size_t total_item_count,
                            size_t &first_item, size_t &last_item)
{ 
  first_item = (total_item_count * thread_num) / total_threads;
  last_item = (total_item_count * (thread_num + 1)) / total_threads;
}



class my_matrix
{
private:
  double *data;
  size_t *indices; // indices[i] contains the index
                   // of i-th element of row layout in \code{data}
  size_t n_cols;
  size_t n_rows;

public:
   my_matrix(size_t rows, size_t cols)
     : n_cols(cols), n_rows(rows)
   {
     data = new (std::nothrow) double [n_cols * n_rows];
     indices = new (std::nothrow) size_t [n_cols * n_rows];
     if (!data || !indices)
       {
         cerr << "Not enough memory!" << endl;
         abort();
       }
     set_to_zero();
   }

   ~my_matrix()
   {
     delete [] data;
     delete [] indices;
   }

   double& operator() (size_t i, size_t j)
   {
     return data[indices[i * n_cols + j]];
   }

   double operator() (size_t i, size_t j) const
   {
     return data[indices[i * n_cols + j]];
   }

   size_t get_n_cols() const { return n_cols; }
   size_t get_n_rows() const { return n_rows; }
   double *get_data() const {return data; }
   size_t *get_indices() const { return indices; }

   void set_index (size_t i, size_t j, size_t k)
   {
     indices[i * n_cols + j] = k;
   }
   void set_to_zero ()
   {
     memset(data, 0, n_cols * n_rows * sizeof(double));
     size_t index;
     for (index = 0; index < ((n_cols * n_rows) & 7); ++index)
       indices[index] = index;
     for ( ; index < n_cols * n_rows; index += 8)
       {
         indices[index] = index;
         indices[index + 1] = index + 1;
         indices[index + 2] = index + 2;
         indices[index + 3] = index + 3;
         indices[index + 4] = index + 4;
         indices[index + 5] = index + 5;
         indices[index + 6] = index + 6;
         indices[index + 7] = index + 7;
       }
   }

   // operator= makes row layout for lhs matrix
   my_matrix &operator= (const my_matrix &rhs)
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

   friend ostream& operator<< (ostream& output, const my_matrix &p)
   {
     size_t i, j;
     size_t rows = p.n_rows < 8 ? p.n_rows : 8;
     size_t cols = p.n_cols < 8 ? p.n_cols : 8;
     for (i = 0; i < rows; i++)
       {
         for (j = 0; j < cols; j++)
           output << p(i, j) << " ";
         output << endl;
       }
     return output;
   }
};


inline
void multiply_and_add_blocks(
  const double *a, const double *b, double *c,
  size_t m, size_t n, size_t p
)
{
  memset(c, 0, m * p * sizeof(double));
#if !BLOCK_BLOCK_MULT
  for (size_t i = 0; i < m; ++i)
    for (size_t j = 0; j < p; ++j)
      for (size_t k = 0; k < n; ++k)
        c[i * p + j] += a[i * n + k] * b[k * p + j];
#else
  size_t i, j, k, rest;
  double s00 = 0, s01 = 0, s10 = 0, s11 = 0;

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
#if UNROLLED
                  rest = n & 3;
#else
                  rest = n;
#endif
                  for (k = 0; k < rest; ++k)
                    {
                      s00 += a[i * n + k] * b[k * p + j];
                    }
                  for ( ; k < n; k += 4)
                    {
                      s00 += a[i * n + k    ] * b[ k      * p + j]
                           + a[i * n + k + 1] * b[(k + 1) * p + j]
                           + a[i * n + k + 2] * b[(k + 2) * p + j]
                           + a[i * n + k + 3] * b[(k + 3) * p + j];
                    }
                  c[i * p + j] += s00;
                }
              else   // m3 = 2
                {
#if UNROLLED
                  rest = n & 1;
#else
                  rest = n;
#endif
                  for (k = 0; k < rest; ++k)
                    {
                      s00 += a[i * n + k] * b[k * p + j    ];
                      s01 += a[i * n + k] * b[k * p + j + 1];
                    }
                  for ( ; k < n; k += 2)
                    {
                      s00 += a[i * n + k    ] * b[ k      * p + j    ]
                           + a[i * n + k + 1] * b[(k + 1) * p + j    ];
                      s01 += a[i * n + k    ] * b[ k      * p + j + 1]
                           + a[i * n + k + 1] * b[(k + 1) * p + j + 1];
                    }
                  c[i * p + j    ] += s00;
                  c[i * p + j + 1] += s01;
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
#if UNROLLED
                  rest = n & 1;
#else
                  rest = n;
#endif
                  for (k = 0; k < rest; ++k)
                    {
                      s00 += a[i * n + k    ] * b[k * p + j];
                      s10 += a[i * n + k + n] * b[k * p + j];
                    }
                  for ( ; k < n; k += 2)
                    {
                      s00 += a[i * n + k        ] * b[ k      * p + j]
                           + a[i * n + k + 1    ] * b[(k + 1) * p + j];
                      s10 += a[i * n + k + n    ] * b[ k      * p + j]
                           + a[i * n + k + 1 + n] * b[(k + 1) * p + j];
                    }
                  c[i * p + j    ] += s00;
                  c[i * p + j + p] += s10;
                }
              else   // m3 = 2
                {
                  for (k = 0; k < n; ++k)
                    {
                      s00 += a[i * n + k    ] * b[k * p + j    ];
                      s01 += a[i * n + k    ] * b[k * p + j + 1];
                      s10 += a[i * n + k + n] * b[k * p + j    ];
                      s11 += a[i * n + k + n] * b[k * p + j + 1];
                    }
                  c[i * p + j        ] += s00;
                  c[i * p + j + 1    ] += s01;
                  c[i * p + j + p    ] += s10;
                  c[i * p + j + p + 1] += s11;
                }
            }
        }
    }
#endif
}



struct args
{
  const my_matrix *a;
  const my_matrix *b;
  my_matrix *c;
  size_t total_threads;
  size_t thread_num;
};


void *mult_matrix_matrix (void *pargs)
{
  args *arguments = (args *) pargs;
  my_matrix const &a = *(arguments->a);
  my_matrix const &b = *(arguments->b);
  my_matrix &c = *(arguments->c);
  size_t total_threads = arguments->total_threads;
  size_t thread_num = arguments->thread_num;

  if (a.get_n_cols() != b.get_n_rows())
    return 0; 

  size_t a_rows = a.get_n_rows();
  size_t a_cols = a.get_n_cols();
  size_t b_cols = b.get_n_cols();

  if (thread_num == 0)
    c.set_to_zero();
  synchronize_threads(total_threads);

#if BLOCK_MULT
  size_t m = BLOCK_SIZE, n_blocks = (a_rows + m - 1) / m;
  size_t i, j, k, m1, m2, m3, first_block, last_block, i_block;

  double *my_a = new double [m * m];
  double *my_b = new double [m * m];
  double *my_c = new double [m * m];

  // loop over matrix rows
  calc_thread_own_items (total_threads, thread_num, n_blocks,
                         first_block, last_block);
  for (i_block = first_block; i_block < last_block; ++i_block)
    {
      i = i_block * m;
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
              size_t p, q;
              // fill my_a (m1 x m2)
              for (p = 0; p < m1; ++p)
                for (q = 0; q < m2; ++q)
                  my_a[p * m2 + q] = a(i + p, k + q);
              // fill my_b (m2 x m3)
              for (p = 0; p < m2; ++p)
                for (q = 0; q < m3; ++q)
                  my_b[p * m3 + q] = b(k + p, j + q);

              multiply_and_add_blocks(my_a, my_b, my_c, m1, m2, m3);

              // write answer in c
              for (p = 0; p < m1; ++p)
                for (q = 0; q < m3; ++q)
                  c(i + p, j + q) += my_c[p * m3 + q];
            }
        }
    }

  delete [] my_a;
  delete [] my_b;
  delete [] my_c;
#else
  size_t first_row, last_row; 
  calc_thread_own_items (total_threads, thread_num, a_rows,
                         first_row, last_row);
  for (size_t i = first_row; i < last_row; ++i)
    for (size_t j = 0; j < b_cols; ++j)
      for (size_t k = 0; k < a_cols; ++k)
        c(i,j) += a(i,k) * b(k,j);
#endif
  return 0;
}


