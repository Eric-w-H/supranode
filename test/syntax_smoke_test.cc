#include "supranode"

namespace hana = boost::hana;

int main(void)
{
  auto io_fn = [](const int a, const int b, char *c, char *d){ *c = a; *d = b; };
  auto oi_fn = [](const char a, const char b, int *c, int *d){ *c = a; *d = b; };
  auto ii_fn = [](const int a, const int b, int *c, int *d){ *c = b; *d = a; };
  auto oo_fn = [](const char a, const char b, char *c, char *d){ *c = a; *d = b; };
  
  supra::node::GenericNode<2, 2, int, int, char, char> test_all_instantiated{};
  test_all_instantiated.set_oi_connection(oi_fn);
  test_all_instantiated.set_oo_connection(oo_fn);
  supra::utils::print_tuple(test_all_instantiated.get_connection_state());
  test_all_instantiated.set_io_connection(io_fn); // only this and the next should be set
  test_all_instantiated.set_ii_connection(ii_fn);
  supra::utils::print_tuple(test_all_instantiated.get_connection_state());

  int a, b;
  char c, d;

  a = 'a';
  b = 'b';
  c = 'c';
  d = 'd';

  test_all_instantiated.consume_raw_input(a, b);
  test_all_instantiated.consume_raw_update(c, d);
  std::cout << a << " " << b << " " << c << " " << d << std::endl;
  io_fn(a, b, &c, &d);
  ii_fn(a, b, &a, &b);
  std::cout << a << " " << b << " " << c << " " << d << std::endl;

  supra::utils::print_tuple(test_all_instantiated.get_raw_outputs());
  supra::utils::print_tuple(test_all_instantiated.get_raw_updates());

  auto io = [](const supra::Vector<float> in1, float* out){ *out = in1.sum(); };
  supra::node::GenericNode<1, 1, supra::Vector<float>, float> test_vector_node{};
  test_vector_node.set_io_connection(io);

  supra::Vector<float> vec = supra::Vector<float>::Random(100);

  test_vector_node.consume_raw_input(vec);
  std::cout << "result of vec sum is: ";
  supra::utils::print_tuple(test_vector_node.get_raw_outputs(), "", " ");
  std::cout << "=?= " << vec.sum() << '\n';
}