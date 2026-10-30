#ifndef ANALYSIS_COMMANDS_HPP
#define ANALYSIS_COMMANDS_HPP

#include <iosfwd>

namespace nabieva
{
  enum class CommandResult;
  struct command_context_t;

  CommandResult analyzeDay(std::istream& input, command_context_t& context, std::ostream& output);
  CommandResult recommendMeal(std::istream& input, command_context_t& context,
      std::ostream& output);
}

#endif
