#include <iostream>
#include <supranode>
#include <string>

namespace hana = boost::hana;
using namespace hana::literals;

struct Fish { std::string name; };
struct Cat  { std::string name; };
struct Dog  { std::string name; };

int main()
{
  auto animals = hana::make_tuple(Fish{"Nemo"}, Cat{"Garfield"}, Dog{"Snoopy"});
  auto Garfield = animals[1_c];
  
  auto names = hana::transform(animals, [](auto a) {
    return a.name;
  });

  assert(hana::reverse(names) == hana::make_tuple("Snoopy", "Garfield", "Nemo"));
  hana::for_each(names, [&](auto name){ std::cout << name << ", ";} );
  std::cout << std::endl;
}