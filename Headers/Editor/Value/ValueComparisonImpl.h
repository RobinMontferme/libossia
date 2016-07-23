#pragma once
#include <Editor/Value/SafeValue.h>

#include <Network/Address.h>

namespace OSSIA
{

namespace Comparisons
{
struct Impulse_T
{
  template<typename T>
  friend bool operator==(const T&, Impulse_T) { return true; }
  template<typename T>
  friend bool operator!=(const T&, Impulse_T) { return false; }
  template<typename T>
  friend bool operator<=(const T&, Impulse_T) { return true; }
  template<typename T>
  friend bool operator>=(const T&, Impulse_T) { return true; }
  template<typename T>
  friend bool operator<(const T&, Impulse_T) { return false; }
  template<typename T>
  friend bool operator>(const T&, Impulse_T) { return true; }
};

struct NumericValue
{
  template<typename T, typename Fun>
  static bool apply(const T& lhs, const OSSIA::Value& val, Fun fun)
  {
    struct visitor {
      const T& lhs;
      Fun fun;
    public:
      bool operator()(Impulse) const { return fun(lhs.value, Impulse_T{}); }
      bool operator()(Int v) const { return fun(lhs.value, v.value); }
      bool operator()(Float v) const { return fun(lhs.value, v.value); }
      bool operator()(Bool v) const { return fun(lhs.value, v.value); }
      bool operator()(Char v) const { return fun(lhs.value, v.value); }
      bool operator()(const Tuple& v) const
      { return (v.value.size() == 1) && (fun(lhs, v.value[0])); }
      bool operator()(const Destination& d) const {
        if (const auto& addr = d.value->getAddress())
        {
          return fun(lhs, addr->cloneValue(d.index));
        }
        return false;
      }

      bool operator()(const String& v) const { return false; }
      bool operator()(Vec2f v) const { return false; }
      bool operator()(Vec3f v) const { return false; }
      bool operator()(Vec4f v) const { return false; }
      bool operator()(const Behavior&) const { return false; }

    } vis{lhs, fun};

    return val.valid()
        ? eggs::variants::apply(vis, val.v)
        : false;
  }
};

struct StringValue
{
  template<typename Fun>
  static bool apply(const String& lhs, const OSSIA::Value& val, Fun fun)
  {
    struct visitor {
      const String& lhs;
      Fun fun;
    public:
      bool operator()(Impulse) const { return fun(lhs.value, Impulse_T{}); }
      bool operator()(const String& v) const { return fun(lhs.value, v.value); }
      bool operator()(Int v) const { return false; }
      bool operator()(Float v) const { return false; }
      bool operator()(Bool v) const { return false; }
      bool operator()(Char v) const { return false; }
      bool operator()(const Tuple& v) const
      { return (v.value.size() == 1) && (fun(lhs, v.value[0])); }

      bool operator()(const Destination& d) const {
        if (const auto& addr = d.value->getAddress())
        {
          return fun(lhs, addr->cloneValue(d.index));
        }
        return false;
      }

      bool operator()(Vec2f v) const { return false; }
      bool operator()(Vec3f v) const { return false; }
      bool operator()(Vec4f v) const { return false; }
      bool operator()(const Behavior&) const { return false; }

    } vis{lhs, fun};

    return val.valid()
        ? eggs::variants::apply(vis, val.v)
        : false;
  }
};

template<typename Fun>
struct TupleVisitor
{
  const Tuple& lhs;
  const OSSIA::Value& rhs;
  Fun fun;
public:
  bool operator()(Impulse) const { return fun(lhs.value, Impulse_T{}); }
  bool operator()(const Tuple& t) const
  {
      if (lhs.value.size() != t.value.size())
          return false;

      bool result = true;
      auto tit = t.value.begin();
      for (const auto& val : lhs.value)
      {
          result &= fun(val, *tit);
          if (!result)
              break;
          tit++;
      }

      return result;
  }

  template<typename T>
  bool operator()(const T& v) const
  {
    if (lhs.value.size() == 1)
        return fun(lhs.value[0], rhs);

    return false;
  }

};
template<typename Fun> auto make_tuple_visitor(const Tuple& lhs, const OSSIA::Value& val, Fun f)
{ return TupleVisitor<Fun>{lhs, val, f}; }

struct TupleValue
{
  template<typename Fun>
  static bool apply(const Tuple& lhs, const OSSIA::Value& val, Fun fun)
  {
    auto vis = make_tuple_visitor(lhs, val, fun);

    return val.valid()
        ? eggs::variants::apply(vis, val.v)
        : false;
  }
};


template<typename Fun>
struct DestinationVisitor
{
  const Destination& lhs;
  const OSSIA::Value& rhs;
  Fun fun;
public:
  bool operator()(Impulse) const { return fun(lhs.value, Impulse_T{}); }
  bool operator()(const Destination& d) const
  {
    // if there are addresses compare values
    if (lhs.value->getAddress() && d.value->getAddress())
    {
        auto c1 = lhs.value->getAddress()->cloneValue(lhs.index);
        auto c2 = d.value->getAddress()->cloneValue(d.index);
        return fun(c1, c2);
    }

    // if no addresses, compare nodes
    else if (!lhs.value->getAddress() && !d.value->getAddress())
    {
        return fun(lhs.value, d.value);
    }

    return false;
  }

  template<typename T>
  bool operator()(const T& v) const
  {
      if (lhs.value->getAddress())
      {
          auto c = lhs.value->getAddress()->cloneValue(lhs.index);
          return fun(c, rhs);
      }

      return false;
  }

};

template<typename Fun> auto make_destination_visitor(const Destination& lhs, const OSSIA::Value& val, Fun f)
{ return DestinationVisitor<Fun>{lhs, val, f}; }

struct DestinationValue
{
  template<typename Fun>
  static bool apply(const Destination& lhs, const OSSIA::Value& val, Fun fun)
  {
    return val.valid()
        ? eggs::variants::apply(make_destination_visitor(lhs, val, fun), val.v)
        : false;
  }
};

template<int N, typename Fun>
struct VecVisitor
{
  const Vec<float, N>& lhs;
  Fun fun;
public:
  bool operator()(Impulse) const { return fun(lhs.value, Impulse_T{}); }
  bool operator()(const Vec<float, N>& d) const { return fun(lhs.value, d.value); }

  template<typename T>
  bool operator()(const T& v) const
  {
      return false;
  }
};

template<typename Vec_T, typename Fun>
auto make_vec_visitor(const Vec_T& lhs, Fun f)
{ return VecVisitor<Vec_T::size_value, Fun>{lhs, f}; }


struct VecValue
{
  template<typename Vec_T, typename Fun>
  static bool apply(const Vec_T& lhs, const OSSIA::Value& val, Fun fun)
  {
    if(val.valid())
    {
      return eggs::variants::apply(make_vec_visitor(lhs, fun), val.v);
    }
    return false;
  }
};
}

template<typename T, int N>
bool Vec<T, N>::operator== (const Value& v) const
{ return Comparisons::VecValue::apply(*this, v, std::equal_to<>{}); }
template<typename T, int N>
bool Vec<T, N>::operator!= (const Value& v) const
{ return Comparisons::VecValue::apply(*this, v, std::not_equal_to<>{}); }

template<typename T, int N>
bool Vec<T, N>::operator> (const Value& v) const
{ return Comparisons::VecValue::apply(*this, v, std::greater<>{}); }
template<typename T, int N>
bool Vec<T, N>::operator>= (const Value& v) const
{ return Comparisons::VecValue::apply(*this, v, std::greater_equal<>{}); }

template<typename T, int N>
bool Vec<T, N>::operator< (const Value& v) const
{ return Comparisons::VecValue::apply(*this, v, std::less<>{});}
template<typename T, int N>
bool Vec<T, N>::operator<= (const Value& v) const
{ return Comparisons::VecValue::apply(*this, v, std::less_equal<>{}); }

}
