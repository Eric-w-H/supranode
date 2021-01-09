#ifndef NODE_STREAM_HH
#define NODE_STREAM_HH

#include "node-core.hh"

namespace supra
{
namespace node
{

class NodeStream
{
private:

public:
  
  template<unsigned int num_inputs, unsigned int num_outputs, class... args>
  NodeStream& operator<<(GenericNode<num_inputs, num_outputs, args...> &&N)
  {
    return *this;
  }

};

} // namespace node
} // namespace supra

#endif