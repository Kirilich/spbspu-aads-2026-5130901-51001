#ifndef APPLICATION_COMMANDS_HPP
#define APPLICATION_COMMANDS_HPP

#include <iosfwd>
#include <string>
#include "bstree.hpp"
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

  using CommandHandler = CommandResult (*)(std::istream&, command_context_t&, std::ostream&);
  using CommandTree = BSTree< std::string, CommandHandler >;

  void registerCommands(CommandTree& commands);

  CommandResult executeCommand(const CommandTree& commands, const std::string& command,
      std::istream& input, command_context_t& context, std::ostream& output);
}

#endif
