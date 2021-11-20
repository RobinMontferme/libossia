#pragma once
#include <ossia/detail/config.hpp>
#if defined(OSSIA_PROTOCOL_ARTNET)
#include <ossia/protocols/artnet/dmx_buffer.hpp>
#include <ossia/network/base/protocol.hpp>
#include <ossia/network/common/complex_type.hpp>
#include <ossia/network/domain/domain.hpp>
#include <ossia/network/context.hpp>
#include <ossia/detail/timer.hpp>
#include <array>
#include <cstdint>

using artnet_node = void*;

namespace ossia::net
{
struct dmx_config;
class OSSIA_EXPORT artnet_protocol final
    : public ossia::net::protocol_base
{
public:
  artnet_protocol(ossia::net::network_context_ptr, const dmx_config& conf);
  ~artnet_protocol();

  void set_device(ossia::net::device_base& dev) override;

  bool pull(ossia::net::parameter_base& param) override;
  bool push(const ossia::net::parameter_base& param, const ossia::value& v) override;
  bool push_raw(const ossia::net::full_parameter_data&) override;
  bool observe(ossia::net::parameter_base& param, bool enable) override;

  bool update(ossia::net::node_base&) override;

  dmx_buffer& buffer() noexcept { return m_buffer; }
private:
  void update_function();

  ossia::net::network_context_ptr m_context;

  ossia::timer m_timer;
  dmx_buffer m_buffer;

  ossia::net::device_base* m_device{};
  artnet_node m_node;
  uint8_t m_universe{};

  bool m_autocreate{};
};

}

#endif
