#pragma once
#include <ossia-max/src/object_base.hpp>

namespace ossia
{
namespace max
{

class view : public object_base // FIXME why view doesn't inherite from ossia::max::node_base ??
{
public:

  using is_view = std::true_type;

  bool register_node(const std::vector<std::shared_ptr<matcher>>& nodes);
  bool do_registration(const std::vector<std::shared_ptr<matcher>>& nodes);
  bool unregister();

  static void* create(t_symbol*, long, t_atom*);
  static void destroy(ossia::max::view*);

};

} // max namespace
} // ossia namespace


