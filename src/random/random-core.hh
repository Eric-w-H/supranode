#ifndef RANDOM_HH
#define RANDOM_HH

#include <boost/random/mersenne_twister.hpp>

class RNG
{
  private:
    static RNG* global_rng;
    boost::random::mt19937 m_gen;

    explicit RNG();
  public:
    static boost::random::mt19937 &get_rng();
};

#endif // RANDOM_HH
