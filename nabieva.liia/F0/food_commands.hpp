#ifndef FOOD_COMMANDS_HPP
#define FOOD_COMMANDS_HPP

#include <iosfwd>

namespace nabieva
{
  enum class CommandResult;
  struct command_context_t;

  CommandResult addFood(std::istream& input, command_context_t& context, std::ostream& output);
  CommandResult removeFood(std::istream& input, command_context_t& context, std::ostream& output);
  CommandResult saveFood(std::istream& input, command_context_t& context, std::ostream& output);
  CommandResult loadFood(std::istream& input, command_context_t& context, std::ostream& output);
  CommandResult listFood(std::istream& input, command_context_t& context, std::ostream& output);
}

#endif
