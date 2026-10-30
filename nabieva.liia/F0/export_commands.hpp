#ifndef EXPORT_COMMANDS_HPP
#define EXPORT_COMMANDS_HPP

#include <iosfwd>

namespace nabieva
{
  enum class CommandResult;
  struct command_context_t;

  CommandResult exportReport(std::istream& input, command_context_t& context, std::ostream& output);
}

#endif
