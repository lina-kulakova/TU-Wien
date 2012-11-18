#include <iostream>
#include <tuple>

using std::cout;
using std::endl;
using std::ostream;


template<class... Types>
  std::tuple<Types...>tuple (Types&&... args)
{
  return std::make_tuple (args...);
}

template <typename E>
class general_expression
{
public:
  operator E& () { return static_cast<E&> (*this); }
  operator E const& () const { return static_cast<const E&> (*this); }
};


template <int N>
class scalar : public general_expression<scalar<N> >
{
public:
  static const int value = N;

public:
  scalar () { }

  friend ostream& operator<< (ostream& output, const scalar<N> &p)
  {
    output << p.value;
    return output;
  }
};


template <int N>
class variable : public general_expression<variable<N> >
{
public:
  static const int index = N;

public:
  variable () { }

  friend ostream& operator<< (ostream& output, const variable<N> &p)
  {
    output << p.index;
    return output;
  }
};


template <typename E1, typename E2>
class polynomial_sum :
  public general_expression<polynomial_sum<E1, E2> >
{
public:
  E1 const &u;
  E2 const &v;

public:
  polynomial_sum (general_expression<E1> const &u_arg,
                  general_expression<E2> const &v_arg)
    : u (u_arg), v (v_arg) { }
};


template <typename E1, typename E2>
polynomial_sum<E1, E2> operator+ (general_expression<E1> const &u,
                                  general_expression<E2> const &v)
{
  return polynomial_sum<E1, E2> (u, v);
}

template <typename E1, typename E2>
class polynomial_prod :
  public general_expression<polynomial_prod<E1, E2> >
{
public:
  E1 const &u;
  E2 const &v;

public:
  polynomial_prod (general_expression<E1> const &u_arg,
                   general_expression<E2> const &v_arg)
    : u (u_arg), v (v_arg) { }
};

template <typename E1, typename E2>
polynomial_prod<E1, E2> operator* (general_expression<E1> const &u,
                                   general_expression<E2> const &v)
{
  return polynomial_prod<E1, E2> (u, v);
}

template <typename E1, typename E2, int N>
class diff_sum :
  public general_expression<diff_sum<E1, E2, N> >
{
public:
  E1 const &u;
  E2 const &v;
  variable<N> const &var;

public:
  diff_sum (general_expression<E1> const &u_arg,
            general_expression<E2> const &v_arg,
            variable<N> const &var_arg) :
    u (u_arg), v (v_arg), var (var_arg) { }
};

template <typename E1, typename E2, int N>
class diff_prod :
  public general_expression<diff_prod<E1, E2, N> >
{
public:
  E1 const &u;
  E2 const &v;
  variable<N> const &var;

public:
  diff_prod (general_expression<E1> const &u_arg,
             general_expression<E2> const &v_arg,
             variable<N> const &var_arg) :
    u (u_arg), v (v_arg), var (var_arg) { }
};

template <int N1, int N2>
scalar<(N1 == N2)> diff (variable<N1> var1, variable<N2> var2)
{
  (void) var1;
  (void) var2;
  return scalar<(N1 == N2)> ();
}

template <int N1, int N2>
scalar<0> diff (scalar<N1> s, variable<N2> var)
{
  (void) s;
  (void) var;
  return scalar<0> ();
}

template <typename E1, typename E2, int N>
diff_sum<E1, E2, N>
diff (polynomial_sum<E1, E2> sum, variable<N> var)
{
  return diff_sum<E1, E2, N> (sum.u, sum.v, var);
}

template <typename E1, typename E2, int N>
diff_prod<E1, E2, N>
diff (polynomial_prod<E1, E2> prod, variable<N> var)
{
  return diff_prod<E1, E2, N> (prod.u, prod.v, var);
}

template <int N, class... Types>
int evaluate (variable<N> var, std::tuple<Types...> t)
{
  (void) var;
  return (std::get<N> (t)).value;
}

template <int N, class... Types>
int evaluate (scalar<N> s, std::tuple<Types...> t)
{
  (void) t;
  return s.value;
}

template <typename E1, typename E2, class... Types>
int evaluate (polynomial_sum<E1, E2> sum, std::tuple<Types...> t)
{
  return evaluate (sum.u, t) + evaluate (sum.v, t);
}

template <typename E1, typename E2, class... Types>
int evaluate (polynomial_prod<E1, E2> prod, std::tuple<Types...> t)
{
  return evaluate (prod.u, t) * evaluate (prod.v, t);
}

template <typename E1, typename E2, int N, class... Types>
int evaluate (diff_sum<E1, E2, N> d_sum, std::tuple<Types...> t)
{
  return evaluate (diff (d_sum.u, d_sum.var), t) +
         evaluate (diff (d_sum.v, d_sum.var), t);
}

template <typename E1, typename E2, int N, class... Types>
int evaluate (diff_prod<E1, E2, N> d_prod, std::tuple<Types...> t)
{
  return evaluate (diff (d_prod.u, d_prod.var), t) *
         evaluate (d_prod.v, t) +
         evaluate (diff (d_prod.v, d_prod.var), t) *
         evaluate (d_prod.u, t);
}

