#include "application_commands.hpp"
#include <stdexcept>
#include "analysis_commands.hpp"
#include "export_commands.hpp"
#include "food_commands.hpp"
#include "help_commands.hpp"
#include "plan_commands.hpp"
#include "profile_commands.hpp"
#include "stats_commands.hpp"

void nabieva::registerCommands(CommandTree& commands)
{
  commands.add("profile", setProfileFromCommand);
  commands.add("show-profile", showProfile);
  commands.add("food-add", addFood);
  commands.add("food-remove", removeFood);
  commands.add("food-load", loadFood);
  commands.add("food-save", saveFood);
  commands.add("food-list", listFood);
  commands.add("weight-log", logWeight);
  commands.add("weight-remove", removeWeight);
  commands.add("weight-list", listWeights);
  commands.add("weight-graph", drawWeightGraph);
  commands.add("eat", recordMeal);
  commands.add("eat-remove", removeMeal);
  commands.add("eat-list", listMeals);
  commands.add("day-stats", showDayStats);
  commands.add("week-stats", showWeekStats);
  commands.add("export", exportReport);
  commands.add("generate-plan", generatePlan);
  commands.add("recommend-meal", recommendMeal);
  commands.add("analyze-day", analyzeDay);
  commands.add("help", printHelp);
  commands.add("exit", exitProgram);
}

nabieva::CommandResult nabieva::executeCommand(const CommandTree& commands,
    const std::string& command, std::istream& input, command_context_t& context,
    std::ostream& output)
{
  const CommandTree::const_iterator found = commands.find(command);
  if (found == commands.end())
  {
    throw std::invalid_argument("неизвестная команда.");
  }
  return found->second(input, context, output);
}
