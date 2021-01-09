#include "supranode"
#include <string_view>

namespace hana = boost::hana;

// Test constructing GenericNodes with lots of things. And check properties of the
// nodes.
void test_case_1_construction()
{
  std::cout << __FUNCTION__ << ' ';
  supra::node::GenericNode<0,0> no_params;
  supra::node::GenericNode<1,1, int> few_params;
  supra::node::GenericNode<10, 10, supra::Vector<double>> some_params;
  supra::node::GenericNode<100,100, float> many_params;
  supra::node::GenericNode<10, 50, int> mismatched_params;


  BOOST_HANA_ASSERT(no_params.get_connection_state() == hana::make_tuple(false, false, false, false));
  BOOST_HANA_ASSERT(few_params.get_connection_state() == hana::make_tuple(false, false, false, false));
  BOOST_HANA_ASSERT(some_params.get_connection_state() == hana::make_tuple(false, false, false, false));
  BOOST_HANA_ASSERT(many_params.get_connection_state() == hana::make_tuple(false, false, false, false));
  BOOST_HANA_ASSERT(mismatched_params.get_connection_state() == hana::make_tuple(false, false, false, false));

  BOOST_HANA_ASSERT(hana::length(no_params.get_raw_outputs()) == hana::ulong_c<0>);
  BOOST_HANA_ASSERT(hana::length(no_params.get_raw_updates()) == hana::ulong_c<0>);
  
  BOOST_HANA_ASSERT(hana::length(few_params.get_raw_outputs()) == hana::ulong_c<1>);
  BOOST_HANA_ASSERT(hana::length(few_params.get_raw_updates()) == hana::ulong_c<1>);
  
  BOOST_HANA_ASSERT(hana::length(some_params.get_raw_outputs()) == hana::ulong_c<10>);
  BOOST_HANA_ASSERT(hana::length(some_params.get_raw_updates()) == hana::ulong_c<10>);
  
  BOOST_HANA_ASSERT(hana::length(many_params.get_raw_outputs()) == hana::ulong_c<100>);
  BOOST_HANA_ASSERT(hana::length(many_params.get_raw_updates()) == hana::ulong_c<100>);
  
  BOOST_HANA_ASSERT(hana::length(mismatched_params.get_raw_outputs()) == hana::ulong_c<50>);
  BOOST_HANA_ASSERT(hana::length(mismatched_params.get_raw_updates()) == hana::ulong_c<10>);
  
  std::cout << "passed\n";
}

// Test the input->output connection.
void test_case_2_io()
{
  std::cout << __FUNCTION__ << ' ';

  auto lambda = [](int a, int* b){ *b = a; };
  supra::node::GenericNode<1,1,int> test_node;
  
  test_node.set_io_connection(lambda);

  BOOST_HANA_ASSERT(test_node.get_connection_state() == hana::make_tuple(true, false, false, false));

  int a, b;
  a = 15;
  b = 10;

  lambda(a, &b);
  test_node.consume_raw_input(a);

  BOOST_HANA_ASSERT(b == test_node.get_raw_outputs()[hana::ulong_c<0>]);

  int c = 5;
  lambda(a, &c);
  auto tuple_ans = hana::make_tuple(c);
  test_node.consume_input(tuple_ans);

  BOOST_HANA_ASSERT(tuple_ans == test_node.get_raw_outputs());

  std::cout << "passed\n";
}

// Test the update->update connection.
void test_case_3_oi()
{
  std::cout << __FUNCTION__ << ' ';

  auto lambda = [](int a, int* b){ *b = a; };
  supra::node::GenericNode<1,1,int> test_node;
  
  test_node.set_oi_connection(lambda);

  BOOST_HANA_ASSERT(test_node.get_connection_state() == hana::make_tuple(false, true, false, false));

  int a, b;
  a = 15;
  b = 10;

  lambda(a, &b);
  test_node.consume_raw_update(a);

  BOOST_HANA_ASSERT(b == test_node.get_raw_updates()[hana::ulong_c<0>]);

  int c = 5;
  lambda(a, &c);
  auto tuple_ans = hana::make_tuple(c);
  test_node.consume_update(tuple_ans);

  BOOST_HANA_ASSERT(tuple_ans == test_node.get_raw_updates());

  std::cout << "passed\n";
}

// Test the input->update connection
void test_case_4_ii()
{
  std::cout << __FUNCTION__ << ' ';

  auto lambda = [](int a, int* b){ *b = a; };
  supra::node::GenericNode<1,1,int> test_node;
  
  test_node.set_ii_connection(lambda);

  BOOST_HANA_ASSERT(test_node.get_connection_state() == hana::make_tuple(false, false, true, false));

  int a, b;
  a = 15;
  b = 10;

  lambda(a, &b);
  test_node.consume_raw_input(a);

  BOOST_HANA_ASSERT(b == test_node.get_raw_updates()[hana::ulong_c<0>]);

  int c = 5;
  lambda(a, &c);
  auto tuple_ans = hana::make_tuple(c);
  test_node.consume_input(tuple_ans);

  BOOST_HANA_ASSERT(tuple_ans == test_node.get_raw_updates());

  std::cout << "passed\n";
}

// Test the update->output connection.
void test_case_5_oo()
{
  std::cout << __FUNCTION__ << ' ';

  auto lambda = [](int a, int* b){ *b = a; };
  supra::node::GenericNode<1,1,int> test_node;
  
  test_node.set_oo_connection(lambda);

  BOOST_HANA_ASSERT(test_node.get_connection_state() == hana::make_tuple(false, false, false, true));

  int a, b;
  a = 15;
  b = 10;

  lambda(a, &b);
  test_node.consume_raw_update(a);

  BOOST_HANA_ASSERT(b == test_node.get_raw_outputs()[hana::ulong_c<0>]);

  int c = 5;
  lambda(a, &c);
  auto tuple_ans = hana::make_tuple(c);
  test_node.consume_update(tuple_ans);

  BOOST_HANA_ASSERT(tuple_ans == test_node.get_raw_outputs());

  std::cout << "passed\n";
}

// Test input->output and update->update connections.
void test_case_5_io_oi()
{
  std::cout << __FUNCTION__ << ' ';
  
  auto lambda1 = [](int a, int* b){ *b = a; };
  auto lambda2 = [](int a, int* b){ *b = 2*a; };
  supra::node::GenericNode<1,1,int> test_node;
  
  test_node.set_io_connection(lambda1);
  test_node.set_oi_connection(lambda2);

  BOOST_HANA_ASSERT(test_node.get_connection_state() == hana::make_tuple(true, true, false, false));

  int a, b, c;
  a = 15;
  b = 10;
  c = 5;

  lambda1(a, &b);
  lambda2(a, &c);
  
  test_node.consume_raw_input(a);
  test_node.consume_raw_update(a);

  BOOST_HANA_ASSERT(b == test_node.get_raw_outputs()[hana::ulong_c<0>]);
  BOOST_HANA_ASSERT(c == test_node.get_raw_updates()[hana::ulong_c<0>]);

  int d = 5;
  int e = 0;
  
  lambda1(c, &d);
  lambda2(c, &e);

  auto tuple_d = hana::make_tuple(d);
  auto tuple_e = hana::make_tuple(e);

  test_node.consume_input(c);
  test_node.consume_update(c);

  BOOST_HANA_ASSERT(tuple_d == test_node.get_raw_outputs());
  BOOST_HANA_ASSERT(tuple_e == test_node.get_raw_updates());

  std::cout << "passed\n";
}

// Test the input->output and input->update connections.
void test_case_6_io_ii()
{
  std::cout << __FUNCTION__ << ' ';

  auto lambda1 = [](int a, int* b){ *b = a; };
  auto lambda2 = [](int a, int* b){ *b = 2*a; };
  supra::node::GenericNode<1,1,int> test_node;
  
  test_node.set_io_connection(lambda1);
  test_node.set_ii_connection(lambda2);

  BOOST_HANA_ASSERT(test_node.get_connection_state() == hana::make_tuple(true, false, true, false));

  int a, b, c;
  a = 15;
  b = 10;
  c = 5;

  lambda1(a, &b);
  lambda2(a, &c);
  
  test_node.consume_raw_input(a);

  BOOST_HANA_ASSERT(b == test_node.get_raw_outputs()[hana::ulong_c<0>]);
  BOOST_HANA_ASSERT(c == test_node.get_raw_updates()[hana::ulong_c<0>]);

  int d = 5;
  int e = 0;
  
  lambda1(c, &d);
  lambda2(c, &e);

  auto tuple_d = hana::make_tuple(d);
  auto tuple_e = hana::make_tuple(e);

  test_node.consume_input(c);

  BOOST_HANA_ASSERT(tuple_d == test_node.get_raw_outputs());
  BOOST_HANA_ASSERT(tuple_e == test_node.get_raw_updates());

  std::cout << "passed\n";
}

// Test the update->update and update->output connections.
void test_case_7_oi_oo()
{
  std::cout << __FUNCTION__ << ' ';

  auto lambda1 = [](int a, int* b){ *b = a; };
  auto lambda2 = [](int a, int* b){ *b = 2*a; };
  supra::node::GenericNode<1,1,int> test_node;
  
  test_node.set_oi_connection(lambda1);
  test_node.set_oo_connection(lambda2);

  BOOST_HANA_ASSERT(test_node.get_connection_state() == hana::make_tuple(false, true, false, true));

  int a, b, c;
  a = 15;
  b = 10;
  c = 5;

  lambda1(a, &b);
  lambda2(a, &c);
  
  test_node.consume_raw_update(a);

  BOOST_HANA_ASSERT(b == test_node.get_raw_updates()[hana::ulong_c<0>]);
  BOOST_HANA_ASSERT(c == test_node.get_raw_outputs()[hana::ulong_c<0>]);

  int d = 5;
  int e = 0;
  
  lambda1(c, &d);
  lambda2(c, &e);

  auto tuple_d = hana::make_tuple(d);
  auto tuple_e = hana::make_tuple(e);

  test_node.consume_update(c);

  BOOST_HANA_ASSERT(tuple_d == test_node.get_raw_updates());
  BOOST_HANA_ASSERT(tuple_e == test_node.get_raw_outputs());

  std::cout << "passed\n";
}

// Test the input->update and update->output connections
void test_case_8_ii_oo()
{
  std::cout << __FUNCTION__ << ' ';

  auto lambda1 = [](int a, int* b){ *b = a; };
  auto lambda2 = [](int a, int* b){ *b = 2*a; };
  supra::node::GenericNode<1,1,int> test_node;
  
  test_node.set_ii_connection(lambda1);
  test_node.set_oo_connection(lambda2);

  BOOST_HANA_ASSERT(test_node.get_connection_state() == hana::make_tuple(false, false, true, true));

  int a, b, c;
  a = 15;
  b = 10;
  c = 5;

  lambda1(a, &b);
  lambda2(a, &c);
  
  test_node.consume_raw_input(a);
  test_node.consume_raw_update(a);

  BOOST_HANA_ASSERT(b == test_node.get_raw_updates()[hana::ulong_c<0>]);
  BOOST_HANA_ASSERT(c == test_node.get_raw_outputs()[hana::ulong_c<0>]);

  int d = 5;
  int e = 0;
  
  lambda1(c, &d);
  lambda2(c, &e);

  auto tuple_d = hana::make_tuple(d);
  auto tuple_e = hana::make_tuple(e);

  test_node.consume_input(c);
  test_node.consume_update(c);

  BOOST_HANA_ASSERT(tuple_d == test_node.get_raw_updates());
  BOOST_HANA_ASSERT(tuple_e == test_node.get_raw_outputs());

  std::cout << "passed\n";
}

// Test constructing GenericNodes with different types and number of parameters.
// It even works with an internal GenericNode (though I don't know why you'd want to do that).
void test_case_9_types()
{
  std::cout << __FUNCTION__ << ' ';

  supra::node::GenericNode<1, 1, std::string, int> sview_int;
  supra::node::GenericNode<2, 4, int, int, char, char, char, char> int_char_uneven;
  supra::node::GenericNode<0, 3, supra::Vector<float>> vectors;

  supra::node::GenericNode<1, 0, supra::node::GenericNode<1, 0, int>> node_of_node;

  sview_int.set_io_connection([](std::string a, int* b){ *b = stoi(a); });
  int_char_uneven.set_oi_connection([](char a, char b, char c, char d, int* e, int* f){ *e = a<<b; *f = c<<d; });
  vectors.set_oo_connection(
    []( supra::Vector<float> a, supra::Vector<float> b, supra::Vector<float> c, 
        supra::Vector<float> *d, supra::Vector<float> *e, supra::Vector<float> *f )
    {
      *d = a; *e = c.array() + a.sum(); *f = b.array() - c.sum();
    }
  );

  node_of_node.set_oi_connection([](supra::node::GenericNode<1,0,int> *b){ b->set_ii_connection([](int a, int* b){ *b = a/2; }); });
  node_of_node.set_ii_connection([](supra::node::GenericNode<1,0,int> a, supra::node::GenericNode<1,0,int> *b){  } );

  std::cout << "passed\n";
}

// Test for a "split node" idea
// confirming whether the basic data structure can be a chain of node dependencies
void test_case_10_split_node()
{
  std::cout << __FUNCTION__ << ' ';

  supra::node::GenericNode<2, 2, int, char, int, char> split_node;

  supra::node::GenericNode<1,1, int> branch_1;
  supra::node::GenericNode<1,1, char> branch_2;

  auto b1_io = [](int in, int* out){ *out = 2*in; };
  auto b1_oi = [](int in, int* out){ *out = in/2; };

  auto b2_io = [](char in, char* out){ *out = in + 32; };
  auto b2_oi = [](char in, char* out){ *out = in - 32; };

  branch_1.set_io_connection(b1_io);
  branch_1.set_oi_connection(b1_oi);

  branch_2.set_io_connection(b2_io);
  branch_2.set_oi_connection(b2_oi);

  auto split_io = [&](int in1, char in2, int* out1, char* out2)
  {
    branch_1.consume_raw_input(in1);
    branch_2.consume_raw_input(in2);

    auto out1_t = branch_1.get_raw_outputs();
    auto out2_t = branch_2.get_raw_outputs();

    auto out_t = hana::flatten(hana::make_tuple(out1_t, out2_t));
    auto out_ptrs_t = hana::make_tuple(out1, out2);

    auto assigner = [](auto in, auto* out){ *out = in; };
    auto applier = [&](auto io_seq){ hana::unpack(io_seq, assigner); };
    hana::for_each(hana::zip(out_t, out_ptrs_t), applier);
  };

  split_node.set_io_connection(split_io);

  int b1_input = 10;
  char b2_input = 'A';

  split_node.consume_raw_input(b1_input, b2_input);
  auto branch_node_result = split_node.get_raw_outputs();

  branch_1.consume_raw_input(b1_input);
  branch_2.consume_raw_input(b2_input);
  auto branch_individual_results = hana::flatten(hana::make_tuple(branch_1.get_raw_outputs(), branch_2.get_raw_outputs()));

  BOOST_HANA_ASSERT( branch_individual_results == branch_node_result );

  std::cout << "passed\n";
}


int main(int argc, char** argv)
{
  int test_num = 0;
  if(argc > 1) test_num = atoi(argv[1]);
  if(argc > 2)
  {
    std::cout << "Test program takes 0 or 1 command-line arguments.\n";
     return -1;
  }

  if(test_num == 0 || test_num == 1) test_case_1_construction();
  if(test_num == 0 || test_num == 2) test_case_2_io();
  if(test_num == 0 || test_num == 3) test_case_3_oi();
  if(test_num == 0 || test_num == 4) test_case_4_ii();
  if(test_num == 0 || test_num == 5) test_case_5_oo();
  if(test_num == 0 || test_num == 5) test_case_5_io_oi();
  if(test_num == 0 || test_num == 6) test_case_6_io_ii();
  if(test_num == 0 || test_num == 7) test_case_7_oi_oo();
  if(test_num == 0 || test_num == 8) test_case_8_ii_oo();
  if(test_num == 0 || test_num == 9) test_case_9_types();
  if(test_num == 0 || test_num == 10) test_case_10_split_node();
}
