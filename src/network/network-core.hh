#ifndef NETWORK_CORE_HH
#define NETWORK_CORE_HH

#include "node/node-core.hh"
#include "node/node-stream-core.hh"

namespace supra
{
namespace Network
{

class Network
{
private:

public:
  template<unsigned int num_inputs, unsigned int num_outputs, class... args>
  friend supra::node::NodeStream operator<<(Network &N, supra::node::GenericNode<num_inputs, num_outputs, args...> &&node)
  {
    return supra::node::NodeStream{};
  }
};

} // namespace Network
} // namespace supra

#endif