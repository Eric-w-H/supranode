#ifndef DTYPES_HH
#define DTYPES_HH

#include <concepts>
#include <Eigen/Core>
#include <unsupported/Eigen/CXX11/Tensor>

namespace supra
{

template<typename S> concept addable = 
  requires (S first, S second) {
    {first + second} -> std::convertible_to<S>; // an addable type can add to itself
    {first - second} -> std::convertible_to<S>; // or subtract from itself
  };

template<typename S> concept multiplicable = 
  requires (S first, S second) {
    {first * second} -> std::convertible_to<S>; // multiplicable type can multiply with itself
    {first / second} -> std::convertible_to<S>; // or divide by itself
  };

template<typename S> concept scalar = 
  addable<S> && multiplicable<S>;

// snag Eigen dtypes, cram them into this namespace,
// but using the concepts
template<scalar S> using Matrix = Eigen::Matrix<S, -1, -1>;
template<scalar S> using Vector = Eigen::Matrix<S, -1, 1>;

template<scalar S> using RowVector = Eigen::Matrix<S, 1, -1>;
template<scalar S> using ColVector = Vector<S>;

template<scalar S, int rank> using Tensor = Eigen::Tensor<S, rank>;

} // namespace supra

#endif // dtypes_hh
