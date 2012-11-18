#include "polynomial.h"

int main()
{
  variable<0> x;
  variable<1> y;
  const scalar<1>  _1_;
  const scalar<2>  _2_;

  // evaluate x^2 + xy + y^2 at (x,y) = (1,2)
  std::cout << evaluate( x * x + x * y + y * y,
                        tuple(_1_, _2_) ) << std::endl;

  // evaluate the derivative of x * y + 2 * x + y
  // with respect to x at (1,2)
  std::cout << evaluate( diff( x * y + _2_ * x + y, x),
                        tuple(_1_, _2_) ) << std::endl;
}

