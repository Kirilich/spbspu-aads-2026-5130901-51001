#ifndef INPUT_HPP
#define INPUT_HPP

#include <cstddef>
#include <string>
#include "date.hpp"

namespace nabieva
{
  bool readToken(const std::string& line, std::size_t& position, std::string& token);
  bool readNumber(const std::string& token, double& value);
  Date readOptionalDate(const std::string& line, std::size_t position);
  bool readPositiveInteger(const std::string& token, std::size_t& value);
}

#endif
