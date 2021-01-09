#ifndef NODE_CORE_HH
#define NODE_CORE_HH

#include <iostream>

#include <concepts>
#include <boost/hana.hpp>
#include <functional>

#include "utils/utils.hh"

namespace supra
{
namespace node
{

//======================================================================
// GenericNode
//======================================================================
// A generic node takes any number of inputs and supplies any number of
// outputs. It can propogate inputs to outputs, and accept (or
// discard) updates from its outputs. Therefore, every GenericNode
// has at least two connections, one which accepts inputs and supplies 
// outputs, and one which accepts updates and supplies updates. 
// There are an additional two connections; for turning inputs into updates 
// and updates into outputs, respectively. If any connection is 
// not supplied, it is assumed to be a default "broken" connection.
// Each connection is a custom functor, see ```Connection```.
// GenericNodes are stateless.

template<unsigned int num_inputs, unsigned int num_outputs, class... types> requires
  requires { num_inputs + num_outputs == sizeof...(types); } || // Specify a type for each of the inputs and outputs
  requires { sizeof...(types) == 1; }                           // or have a single type for all inputs and outputs
class GenericNode
{
private:
  //======================================================================
  // Connection
  //======================================================================
  // Connections are internal to Nodes. They handle the dataflow between
  // inputs and outputs. Even though Nodes are technically directed, the
  // GenericNode provides mechanisms to pass data in all four combos.
  // A default Connection is 'disconnected' -- i.e. the call operator
  // ```operator()()``` does nothing with its parameters.
  // Connections are stateless, but can be used with stateful objects by
  // currying the desired function with the object pointer.

  template<class... connection_types>
  struct Connection
  {
    std::function<void(connection_types...)> m_func;

    // default broken connection (does nothing)
    Connection()
      : m_func( [](connection_types... connection_inputs){ } )
    { }

    // custom connection constructor.
    Connection(std::function<void(connection_types...)> func)
      : m_func(std::move(func))
    { }

    // copy constructor
    Connection(const Connection& other) = default;

    inline void operator()(connection_types... connection_inputs...)
    {
      m_func(connection_inputs...);
    }

    // Default, empty function.
    static void broken(connection_types... xs) { }
  }; // end connection class


  // Macros to save on typing
  #define pack_to_front_tuple(pack) hana::take_front_c<num_inputs>( supra::utils::dupe_pack< num_inputs , pack >( ) )
  #define pack_to_back_tuple(pack)  hana::take_back_c<num_outputs>( supra::utils::dupe_pack< num_outputs, pack >( ) )

  using io_connection = decltype( hana::unpack( hana::concat( pack_to_front_tuple(const types&... ), pack_to_back_tuple( types*... )  ), hana::template_<Connection>) )::type;
  using oi_connection = decltype( hana::unpack( hana::concat( pack_to_back_tuple(const types&... ) , pack_to_front_tuple( types*... ) ), hana::template_<Connection>) )::type;
  using ii_connection = decltype( hana::unpack( hana::concat( pack_to_front_tuple(const types&... ), pack_to_front_tuple( types*... ) ), hana::template_<Connection>) )::type;
  using oo_connection = decltype( hana::unpack( hana::concat( pack_to_back_tuple(const types&... ) , pack_to_back_tuple( types*... )  ), hana::template_<Connection>) )::type;

  // Partial instantiation to make sure we declare a void function
  template<class... xs> using void_fn = std::function<void(xs...)>;
  using io_func_type = decltype( hana::unpack( hana::concat( pack_to_front_tuple(const types&... ), pack_to_back_tuple( types*... )  ), hana::template_<void_fn> ) )::type;
  using oi_func_type = decltype( hana::unpack( hana::concat( pack_to_back_tuple(const types&... ) , pack_to_front_tuple( types*... ) ), hana::template_<void_fn> ) )::type;
  using ii_func_type = decltype( hana::unpack( hana::concat( pack_to_front_tuple(const types&... ), pack_to_front_tuple( types*... ) ), hana::template_<void_fn> ) )::type;
  using oo_func_type = decltype( hana::unpack( hana::concat( pack_to_back_tuple(const types&... ) , pack_to_back_tuple( types*... )  ), hana::template_<void_fn> ) )::type;

  // Internal connection style:
  // io_connected conflicts with oo_connected -- overwriting outputs.
  // oi_connected conflicts with ii_connected -- overwriting updates.
  bool io_connected, oi_connected, ii_connected, oo_connected;

  // Internal ownership of outputs and updates via tuples:
  using input_types           = decltype( hana::unpack( pack_to_front_tuple(const types&...), hana::template_<hana::tuple> ) )::type;
  using incoming_update_types = decltype( hana::unpack( pack_to_back_tuple (const types&...), hana::template_<hana::tuple> ) )::type;
  using update_types          = decltype( hana::unpack( pack_to_front_tuple(types*...)      , hana::template_<hana::tuple> ) )::type;
  using output_types          = decltype( hana::unpack( pack_to_back_tuple (types*...)      , hana::template_<hana::tuple> ) )::type;
  using raw_update_types      = decltype( hana::unpack( pack_to_front_tuple(types...)       , hana::template_<hana::tuple> ) )::type;
  using raw_output_types      = decltype( hana::unpack( pack_to_back_tuple (types...)       , hana::template_<hana::tuple> ) )::type;


  update_types m_updates;
  output_types m_outputs;

  // Internal connections
  io_connection m_io_func;
  oi_connection m_oi_func;
  ii_connection m_ii_func;
  oo_connection m_oo_func;

public:
  // Initialize a node to a fully disconnected state.
  GenericNode( )
    : io_connected(false), oi_connected(false), ii_connected(false), oo_connected(false),
      m_updates(hana::transform( m_updates, [](auto x){ return new std::remove_reference<decltype(*x)>::type; } )), 
      m_outputs(hana::transform( m_outputs, [](auto x){ return new std::remove_reference<decltype(*x)>::type; } )),
      m_io_func(io_connection::broken), m_oi_func(oi_connection::broken), 
      m_ii_func(ii_connection::broken), m_oo_func(oo_connection::broken)
  { }

  // Copy constructor
  GenericNode( const GenericNode& other ) = default;
  
  // Delete the things the node owns
  ~GenericNode( )
  {
    hana::for_each( m_updates, [](auto x){ delete x; } );
    hana::for_each( m_outputs, [](auto x){ delete x; } );
  }

  //----------------------------------------------------------------------
  // Utility functions
  //----------------------------------------------------------------------

  // Return a tuple representing which connections are set, in the order:
  // io, oi, ii, oo
  hana::tuple<bool, bool, bool, bool> get_connection_state()
  {
    return hana::make_tuple(io_connected, oi_connected, ii_connected, oo_connected);
  }

  // Process an input tuple.
  void consume_input(const input_types& inputs)
  {
    hana::unpack( hana::concat(inputs, m_outputs), m_io_func );
    hana::unpack( hana::concat(inputs, m_updates), m_ii_func );
  }

  // Process an update tuple.
  void consume_update(const incoming_update_types& updates)
  {
    hana::unpack( hana::concat(updates, m_updates), m_oi_func );
    hana::unpack( hana::concat(updates, m_outputs), m_oo_func );
  }

  // Convenience function to call a node's input function(s) without
  // manually constructing a tuple.
  template<class... Ts>
  inline void consume_raw_input(const Ts&... args)
  {
    consume_input(hana::make_tuple(args...));
  }

  // Convenience function to call a node's update function(s) without
  // manually constructing a tuple.
  template<class... Ts>
  inline void consume_raw_update(const Ts&... args)
  {
    consume_update(hana::make_tuple(args...));
  }

  //----------------------------------------------------------------------
  // Getters for internal state
  //----------------------------------------------------------------------
  
  // Return a const ref to tuple of refs to output types, referencing
  // the outputs
  const raw_output_types get_raw_outputs()
  {
    return hana::ap(hana::make_tuple(supra::utils::ptr_to_val), m_outputs);
  }

  // Return a const ref to tuple of refs to input types, referencing
  // the updates.
  const raw_update_types get_raw_updates()
  {
    return hana::ap(hana::make_tuple(supra::utils::ptr_to_val), m_updates);
  }

  //----------------------------------------------------------------------
  // Connection breakers
  //----------------------------------------------------------------------
  // Set the given connection to a function that does nothing with the
  // inputs or outputs.
  
  // Breaks an input->output connection.
  void break_io_connection( )
  {
    io_connected = false;
    m_io_func = io_connection(io_connection::broken);
  }
  // Breaks an update->update connection.
  void break_oi_connection( )
  {
    oi_connected = false;
    m_oi_func = oi_connection(oi_connection::broken);
  }
  // Breaks an input->update connection.
  void break_ii_connection( )
  {
    ii_connected = false;
    m_ii_func = ii_connection(ii_connection::broken);
  }
  // Breaks an update->output connection.
  void break_oo_connection( )
  {
    oo_connected = false;
    m_oo_func = oo_connection(oo_connection::broken);
  }

  //----------------------------------------------------------------------
  // Connection setters
  //----------------------------------------------------------------------
  // Set a given connection to execute the provided function. Because a
  // node is limited to output/update, some connections conflict. If a
  // connnection conflicts with a connection already set, the older
  // connection is broken.

  // Sets an input->output connection. Breaks an update->output connection
  // if it is set.
  void set_io_connection( io_func_type func )
  {
    m_io_func.m_func = func;
    io_connected = true;
    if(oo_connected) break_oo_connection();
  }

  // Sets an update->update connection. Breaks an input->update connection
  // if it is set.
  void set_oi_connection( oi_func_type func )
  {
    m_oi_func.m_func = func;
    oi_connected = true;
    if(ii_connected) break_ii_connection();
  }
  
  // Sets an input->update connection. Breaks an update->update connection
  // if it is set.
  void set_ii_connection( ii_func_type func )
  {
    m_ii_func.m_func = func;
    ii_connected = true;
    if(oi_connected) break_oi_connection();
  }
  
  // Sets an update->output connection. Breaks an input->output connection
  // if it is set.
  void set_oo_connection( oo_func_type func )
  {
    m_oo_func.m_func = func;
    oo_connected = true;
    if(io_connected) break_io_connection();
  }

};

} // namespace node
} // namespace supra

#endif