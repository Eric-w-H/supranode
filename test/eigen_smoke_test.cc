#include <iostream>
#include <complex>

#include "supranode"
using supra::Vector;

int vector_vector()
{
  Vector<double> vec1 = Vector<double>::Zero(10000);
  Vector<double> vec2 = Vector<double>::Ones(10000);
  Vector<double> res;

  for(int i = 0; i < 1000; ++i)
  {
    res = vec1 + vec2; 
    vec1 = res;
  }
  std::cout << res.sum() << '\n';
  return 0;
}

int i_vector()
{
  using std::complex_literals::operator""i;
  Vector<std::complex<double>> cmplx(5);
  cmplx << 1i,
           -2,
           3i,
           4,
           5i;

  std::cout << cmplx.dot(cmplx) << '\n';
  return 0;
}

int main()
{
  return vector_vector() + i_vector();
}
