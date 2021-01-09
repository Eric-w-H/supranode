#ifndef NEURAVIS_UTILS_HH
#define NEURAVIS_UTILS_HH

#include <boost/hana.hpp>

#include <type_traits>
#include <typeinfo>
#ifndef _MSC_VER
#   include <cxxabi.h>
#endif
#include <memory>
#include <string>
#include <cstdlib>
#include <string_view>

namespace supra
{
// Don't want to have to remember these all the time.
namespace hana = boost::hana;

namespace utils
{

//======================================================================
// dupe_pack
//======================================================================
// Free function to generate a hana::tuple that guarantees a return
// tuple at least as large as the template parameter count, generated
// from the parameter pack to_replicate.

template< unsigned int count, class... to_replicate >
constexpr auto dupe_pack()
{
  auto tuple = hana::if_( hana::less(hana::int_<sizeof...(to_replicate)>{}, hana::int_<1>{}),
                            [](){return hana::tuple_t<void>;},
                            [](){return hana::tuple_t<to_replicate...>;}
                        )();
  auto type = +hana::back(tuple);
  auto num = hana::int_<count>{};

  auto func = hana::if_( hana::greater(num, hana::int_<sizeof...(to_replicate)>{} ), 
    [&](){ return hana::replicate<hana::tuple_tag>(type, num); },
    [&](){ return tuple; }
  );
  return func();
}

//======================================================================
// type_name
//======================================================================
// pretty print function for type names.
template <typename T>
constexpr auto type_name() noexcept {
  std::string_view name = "Error: unsupported compiler", prefix, suffix;
#ifdef __clang__
  name = __PRETTY_FUNCTION__;
  prefix = "auto type_name() [T = ";
  suffix = "]";
#elif defined(__GNUC__)
  name = __PRETTY_FUNCTION__;
  prefix = "constexpr auto supra::utils::type_name() [with T = ";
  suffix = "]";
#elif defined(_MSC_VER)
  name = __FUNCSIG__;
  prefix = "auto __cdecl type_name<";
  suffix = ">(void) noexcept";
#endif
  name.remove_prefix(prefix.size());
  name.remove_suffix(suffix.size());
  return name;
}

//======================================================================
// ptr_to_val
//======================================================================
// generic lambda to return the data a pointer points to. Seems stupid,
// but is useful for Boost::Hana
auto ptr_to_val = [](auto arg){ return *arg; };

//======================================================================
// print_iterable
//======================================================================
// Print the data inside hana iterables, in order. An optional separator
// may also be provided.
template<class T>
void print_tuple(T iterable, std::string_view sep = " ", std::string_view end = "\n")
{
  hana::for_each(iterable, [&](auto val){ std::cout << val << sep; });
  std::cout << end;
}

} // namespace utils
} // namespace neuravis

#endif // NEURAVIS_UTILS_HH