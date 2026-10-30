#ifndef PROFILE_COMMANDS_HPP
#define PROFILE_COMMANDS_HPP

#include <iosfwd>

namespace nabieva
{
  enum class CommandResult;
  struct command_context_t;

  CommandResult setProfileFromCommand(std::istream& input, command_context_t& context,
      std::ostream& output);
  CommandResult showProfile(std::istream& input, command_context_t& context, std::ostream& output);
}

#endif
