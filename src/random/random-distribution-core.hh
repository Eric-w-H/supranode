#ifndef DISTRIBUTION_HH
#define DISTRIBUTION_HH

#include "random-core.hh"
#include "dtypes/dtypes.hh"

namespace supra
{

template<typename D, typename S> concept distribution = scalar<S> &&
  requires (D distro, S scl) {
    { distro.sample() } -> scalar;
    { distro.sample(scl) } -> scalar;
  };

} // namespace supra

#endif
