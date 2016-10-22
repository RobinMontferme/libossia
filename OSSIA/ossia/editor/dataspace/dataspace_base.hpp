#pragma once
#include <ossia/detail/algorithms.hpp>
#include <ossia/editor/value/float.hpp>
#include <ossia/editor/value/vec.hpp>

#include <ossia/detail/math.hpp>
#include <brigand/algorithms/transform.hpp>
#include <ratio>
#include <type_traits>
namespace ossia
{
// These algorithms are a more statically typed version
// of the ones found in the Jamoma dataspace library.
// Credits : Tim Place, Nils Peters, Trond Lossius, and certainly others.
// This library also tries to avoid dynamic allocation whenever possible.

template<typename T, typename U>
using enable_if_same_dataspace = std::enable_if_t<
std::is_same<
 typename T::dataspace_type,
 typename U::dataspace_type>::value>;

template<typename T, typename U>
using enable_if_different_dataspace = std::enable_if_t<
!std::is_same<
 typename T::dataspace_type,
 typename U::dataspace_type>::value>;

template<typename Unit>
struct strong_value
{
  using unit_type = Unit;
  using value_type = typename Unit::value_type;
  using dataspace_type = typename Unit::dataspace_type;
  using neutral_unit = typename Unit::neutral_unit;
  value_type dataspace_value;

  constexpr strong_value() = default;

  // Constructor that takes anyything able to initialize val
/* Sadly does not work on MSVC........
  template<typename... Args,
           typename = std::enable_if<
             std::is_constructible<value_type, Args...>::value>::type>
      OSSIA_DECL_RELAXED_CONSTEXPR strong_value(Args&&... other):
      value{std::forward<Args>(other)...}
  {
  }
*/
  constexpr strong_value(float other) : dataspace_value{ other } { }
  constexpr strong_value(double other) : dataspace_value{ other } { }
  constexpr strong_value(int other) : dataspace_value{ other } { }
  constexpr strong_value(char other) : dataspace_value{ other } { }
  constexpr strong_value(bool other) : dataspace_value{ other } { }
  constexpr strong_value(std::array<float, 2> other) : dataspace_value{ other } { }
  constexpr strong_value(std::array<float, 3> other) : dataspace_value{ other } { }
  constexpr strong_value(std::array<float, 4> other) : dataspace_value{ other } { }
  constexpr strong_value(std::array<double, 2> other) : dataspace_value{ other[0], other[1] } { }
  constexpr strong_value(std::array<double, 3> other) : dataspace_value{ other[0], other[1], other[2] } { }
  constexpr strong_value(std::array<double, 4> other) : dataspace_value{ other[0], other[1], other[2], other[3] } { }
  constexpr strong_value(float f0, float f1) : dataspace_value{ f0, f1 } { }
  constexpr strong_value(float f0, float f1, float f2) : dataspace_value{ f0, f1, f2 } { }
  constexpr strong_value(float f0, float f1, float f2, float f3) : dataspace_value{ f0, f1, f2, f3} { }

  // Conversion constructor
  template<typename U>
  constexpr strong_value(strong_value<U> other):
    dataspace_value{unit_type::from_neutral(U::to_neutral(other))}
  {
    static_assert(std::is_same<dataspace_type, typename U::dataspace_type>::value, "Trying to convert between different dataspaces");
  }

  // Copy constructor
  constexpr strong_value<Unit>(const strong_value<Unit>& other):
      dataspace_value{other.dataspace_value}
  {
  }

  friend bool operator==(
      const strong_value& lhs,
      const strong_value& rhs)
  { return lhs.dataspace_value == rhs.dataspace_value; }
  friend bool operator!=(
      const strong_value& lhs,
      const strong_value& rhs)
  { return lhs.dataspace_value != rhs.dataspace_value; }
};

template<typename T, typename Ratio_T>
struct linear_unit : public T
{
  static OSSIA_DECL_RELAXED_CONSTEXPR strong_value<typename T::neutral_unit>
    to_neutral(strong_value<typename T::concrete_type> self)
  {
    return {self.dataspace_value * ratio()};
  }

  static OSSIA_DECL_RELAXED_CONSTEXPR typename T::value_type
    from_neutral(strong_value<typename T::neutral_unit> self)
  {
    return self.dataspace_value / ratio();
  }

  static OSSIA_DECL_RELAXED_CONSTEXPR double ratio()
  {
    return double(Ratio_T::num) / double(Ratio_T::den);
  }
};

template<typename T>
struct unit_traits
{
  static OSSIA_DECL_RELAXED_CONSTEXPR auto text() { return T::text(); }
};

template<typename T>
struct unit_traits<strong_value<T>>
{
  static OSSIA_DECL_RELAXED_CONSTEXPR auto text() { return unit_traits<T>::text(); }
};

template<typename T>
struct dataspace_traits : public std::false_type {};
}
