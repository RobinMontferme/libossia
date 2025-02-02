#pragma once
#include <ossia/detail/config.hpp>

#include <ossia/detail/logger_fwd.hpp>

#include <memory>
namespace spdlog
{
// Use it by doing : #include <ossia/detail/logger.hpp>
class logger;
}

namespace ossia
{
namespace net
{
//! Stores custom loggers for the inbound and outbound network messages
class OSSIA_EXPORT network_logger
{
public:
  network_logger();
  ~network_logger();

  network_logger(
      std::shared_ptr<ossia::logger_type> i, std::shared_ptr<ossia::logger_type> o)
      : inbound_logger{std::move(i)}, outbound_logger{std::move(o)}
  {
  }

  /**
   * @brief inbound_logger Set log function for messages coming from
   * outside.
   */
  std::shared_ptr<ossia::logger_type> inbound_logger;

  /**
   * @brief outbound_logger Set log function for messages going outside.
   */
  std::shared_ptr<ossia::logger_type> outbound_logger;

  // Same but will only be active for parameters that are listened to.
  std::shared_ptr<ossia::logger_type> inbound_listened_logger;
  std::shared_ptr<ossia::logger_type> outbound_listened_logger;
};
}
}
