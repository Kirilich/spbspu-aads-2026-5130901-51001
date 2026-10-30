#ifndef CONSOLE_HPP
#define CONSOLE_HPP

#include <cstddef>
#include <iosfwd>
#include <string>

namespace nabieva
{
  std::size_t getTextWidth(const std::string& text);
  void printText(std::ostream& output, const std::string& text, std::size_t width,
      bool leftAligned = true);
  std::string getCategoryText(const std::string& category);
  std::string getPlanModeText(const std::string& mode);
}

#endif
