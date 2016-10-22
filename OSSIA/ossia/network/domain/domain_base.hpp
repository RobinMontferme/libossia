#pragma once
#include <ossia/editor/value/value_traits.hpp>
#include <boost/container/flat_set.hpp>
#include <boost/optional.hpp>
#include <type_traits>

namespace ossia
{
OSSIA_EXPORT ossia::value clamp(const ossia::value& val, const ossia::value& min, const ossia::value& max);
OSSIA_EXPORT ossia::value wrap(const ossia::value& val, const ossia::value& min, const ossia::value& max);
OSSIA_EXPORT ossia::value fold(const ossia::value& val, const ossia::value& min, const ossia::value& max);
OSSIA_EXPORT ossia::value clamp_min(const ossia::value& val, const ossia::value& min);
OSSIA_EXPORT ossia::value clamp_max(const ossia::value& val, const ossia::value& max);

OSSIA_EXPORT ossia::value clamp(ossia::value&& val, const ossia::value& min, const ossia::value& max);
OSSIA_EXPORT ossia::value wrap(ossia::value&& val, const ossia::value& min, const ossia::value& max);
OSSIA_EXPORT ossia::value fold(ossia::value&& val, const ossia::value& min, const ossia::value& max);
OSSIA_EXPORT ossia::value clamp_min(ossia::value&& val, const ossia::value& min);
OSSIA_EXPORT ossia::value clamp_max(ossia::value&& val, const ossia::value& max);

namespace net
{
template <typename T>
struct OSSIA_EXPORT domain_base
{
  using value_type = typename value_trait<T>::value_type;
  boost::optional<value_type> min;
  boost::optional<value_type> max;
  boost::container::flat_set<value_type> values;

  domain_base() = default;
  domain_base(const domain_base&) = default;
  domain_base(domain_base&&) = default;
  domain_base& operator=(const domain_base&) = default;
  domain_base& operator=(domain_base&&) = default;

  domain_base(value_type v1, value_type v2): min{v1}, max{v2} { }
};

template <>
struct OSSIA_EXPORT domain_base<Impulse>
{
};

template <>
struct OSSIA_EXPORT domain_base<String>
{
  boost::container::flat_set<std::string> values;
};

template <>
struct OSSIA_EXPORT domain_base<Tuple>
{
  using value_type = Tuple;
  boost::optional<value_type> min;
  boost::optional<value_type> max;
  boost::container::flat_set<value_type> values;

  domain_base<value_type>() = default;
  domain_base<value_type>(const domain_base<value_type>&) = default;
  domain_base<value_type>(domain_base<value_type>&&) = default;
  domain_base<value_type>& operator=(const domain_base<value_type>&) = default;
  domain_base<value_type>& operator=(domain_base<value_type>&&) = default;

  domain_base<value_type>(const value_type& v1, const value_type& v2): min{v1}, max{v2} { }
  domain_base<value_type>(value_type&& v1, value_type&& v2): min{std::move(v1)}, max{std::move(v2)} { }
};

template <std::size_t N>
struct OSSIA_EXPORT domain_base<Vec<float, N>>
{
  using value_type = Vec<float, N>;
  boost::optional<Vec<float, N>> min;
  boost::optional<Vec<float, N>> max;
  boost::container::flat_set<Vec<float, N>> values;

  domain_base<value_type>() = default;
  domain_base<value_type>(const domain_base<value_type>&) = default;
  domain_base<value_type>(domain_base<value_type>&&) = default;
  domain_base<value_type>& operator=(const domain_base<value_type>&) = default;
  domain_base<value_type>& operator=(domain_base<value_type>&&) = default;

  domain_base<value_type>(const value_type& v1, const value_type& v2): min{v1}, max{v2} { }
};

template <>
struct OSSIA_EXPORT domain_base<ossia::value>
{
  using value_type = ossia::value;
  boost::optional<value_type> min;
  boost::optional<value_type> max;
  boost::container::flat_set<value_type> values;

  domain_base<value_type>() = default;
  domain_base<value_type>(const domain_base<value_type>&) = default;
  domain_base<value_type>(domain_base<value_type>&&) = default;
  domain_base<value_type>& operator=(const domain_base<value_type>&) = default;
  domain_base<value_type>& operator=(domain_base<value_type>&&) = default;

  domain_base<value_type>(const value_type& v1, const value_type& v2): min{v1}, max{v2} { }
  domain_base<value_type>(value_type&& v1, value_type&& v2): min{std::move(v1)}, max{std::move(v2)} { }
};

/**
 * \typedef domain A domain of values
 *
 * The practical domain may be different according to the value type.
 * For instance, \ref domain_base<Impulse> has no min nor max.
 *
 * Domains for most types have a min, a max and a set of values.
 * Domain for String has a set of values.
 *
 * It is used to restrict a value to the domain if available.
 */
using domain
    = eggs::variant<domain_base<Impulse>, domain_base<Bool>, domain_base<Int>,
                    domain_base<Float>, domain_base<Char>, domain_base<String>,
                    domain_base<Tuple>, domain_base<Vec2f>, domain_base<Vec3f>,
                    domain_base<Vec4f>, domain_base<ossia::value>>;
}
}
