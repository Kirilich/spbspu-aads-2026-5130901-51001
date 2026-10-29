#ifndef APPLICATION_COMMANDS_HPP
#define APPLICATION_COMMANDS_HPP

#include <cstddef>
#include <iosfwd>
#include <string>
#include "food_catalog.hpp"
#include "meal_commands.hpp"
#include "profile.hpp"
#include "weight_commands.hpp"

namespace nabieva
{
  enum class CommandResult
  {
    unchanged,
    changed,
    profileChanged,
    exit
  };

  struct command_context_t
  {
    Profile& profile_;
    FoodCatalog& catalog_;
    WeightHistory& history_;
    FoodDiary& diary_;
    const std::string& dataFile_;
  };

  CommandResult executeCommand(const std::string& command, const std::string& line,
    std::size_t position, command_context_t& context, std::ostream& output);
}

#endif
