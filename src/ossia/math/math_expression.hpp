#pragma once
#include <ossia/detail/config.hpp>

#include <string>
#include <vector>
#include <ossia/network/value/value.hpp>

namespace ossia
{
class OSSIA_EXPORT math_expression
{
public:
  math_expression();
  ~math_expression();

  void add_variable(const std::string& var, double& value);
  void add_constant(const std::string& var, double& value);
  void add_vector(const std::string& var, std::vector<double>& value);
  void remove_vector(const std::string& var);
  void add_constants();
  void register_symbol_table();

  bool set_expression(const std::string& expr);
  bool recompile();

  bool has_variable(std::string_view var) const noexcept;
  std::string error() const;

  double value();

  ossia::value result();

private:
  math_expression(const math_expression&) = delete;
  math_expression(math_expression&&) = delete;
  math_expression& operator=(const math_expression&) = delete;
  math_expression& operator=(math_expression&&) = delete;

  struct impl;
  impl* impl{};
};
}
