#include "application_commands.hpp"
#include <stdexcept>
#include "analysis_commands.hpp"
#include "export_commands.hpp"
#include "food_commands.hpp"
#include "help_commands.hpp"
#include "input.hpp"
#include "plan_commands.hpp"
#include "profile_commands.hpp"
#include "stats_commands.hpp"

namespace nabieva
{
  static CommandResult getChangeResult(bool changed)
  {
    return changed ? CommandResult::changed : CommandResult::unchanged;
  }
}

nabieva::CommandResult nabieva::executeCommand(const std::string& command,
  const std::string& line, std::size_t position, command_context_t& context, std::ostream& output)
{
  if (command == "profile")
  {
    setProfileFromCommand(context.profile_, line, position);
    return CommandResult::profileChanged;
  }
  if (command == "food-add")
  {
    return getChangeResult(addFood(context.catalog_, line, position, output));
  }
  if (command == "food-remove")
  {
    return getChangeResult(removeFood(context.catalog_, line, position, output));
  }
  if (command == "food-load")
  {
    return getChangeResult(loadFood(context.catalog_, line, position, output));
  }
  if (command == "weight-log")
  {
    return getChangeResult(logWeight(context.profile_, context.history_, line, position, output));
  }
  if (command == "weight-remove")
  {
    return getChangeResult(removeWeight(context.history_, line, position, output));
  }
  if (command == "eat")
  {
    return getChangeResult(recordMeal(context.catalog_, context.diary_, line, position, output));
  }
  if (command == "eat-remove")
  {
    return getChangeResult(removeMeal(context.diary_, line, position, output));
  }
  if (command == "food-save")
  {
    saveFood(context.catalog_, line, position, output);
  }
  else if (command == "eat-list")
  {
    listMeals(context.diary_, line, position, output);
  }
  else if (command == "day-stats")
  {
    showDayStats(context.profile_, context.history_, context.diary_, line, position, output);
  }
  else if (command == "week-stats")
  {
    showWeekStats(context.profile_, context.history_, context.diary_, line, position, output);
  }
  else if (command == "export")
  {
    exportReport(context.profile_, context.history_, context.diary_, line, position, output);
  }
  else if (command == "generate-plan")
  {
    generatePlan(context.profile_, context.catalog_, context.diary_, line, position, output);
  }
  else if (command == "recommend-meal")
  {
    recommendMeal(context.profile_, context.catalog_, context.diary_, line, position, output);
  }
  else if (command == "analyze-day")
  {
    analyzeDay(context.profile_, context.diary_, line, position, output);
  }
  else
  {
    std::string extra = "";
    if (readToken(line, position, extra))
    {
      throw std::invalid_argument("лишние параметры команды.");
    }
    if (command == "help")
    {
      printHelp(output, context.dataFile_);
    }
    else if (command == "show-profile")
    {
      context.profile_.printProfile(output, getCurrentWeight(context.profile_, context.history_));
    }
    else if (command == "food-list")
    {
      listFood(context.catalog_, output);
    }
    else if (command == "weight-list")
    {
      listWeights(context.history_, output);
    }
    else if (command == "weight-graph")
    {
      drawWeightGraph(context.profile_, context.history_, output);
    }
    else if (command == "exit")
    {
      return CommandResult::exit;
    }
    else
    {
      throw std::invalid_argument("неизвестная команда.");
    }
  }
  return CommandResult::unchanged;
}
