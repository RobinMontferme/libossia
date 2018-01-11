// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/dataflow/graph/graph_utils.hpp>
#include <ossia/dataflow/execution_state.hpp>
#include <ossia/dataflow/port.hpp>
#include <ossia/dataflow/dataflow.hpp>
#include <ossia/network/generic/generic_parameter.hpp>
namespace ossia
{

void outlet::write(execution_state& e)
{
  apply_to_destination(address, e.allDevices, [&] (ossia::net::parameter_base* addr, bool unique) {
    if(unique)
    {
      if (scope & port::scope_t::local)
      {
        graph_util::copy_to_local(std::move(data), *addr, e);
      }
      else if (scope & port::scope_t::global)
      {
        graph_util::copy_to_global(std::move(data), *addr, e);
      }
    }
    else
    {
      if (scope & port::scope_t::local)
      {
        graph_util::copy_to_local(data, *addr, e);
      }
      else if (scope & port::scope_t::global)
      {
        graph_util::copy_to_global(data, *addr, e);
      }
    }
  });
}
}
