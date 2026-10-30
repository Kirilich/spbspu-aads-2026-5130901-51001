#ifndef STATS_COMMANDS_HPP
#define STATS_COMMANDS_HPP

#include <iosfwd>
#include "meal_commands.hpp"
#include "weight_commands.hpp"

namespace nabieva
{
  class Profile;
  enum class CommandResult;
  struct command_context_t;

  CommandResult showDayStats(std::istream& input, command_context_t& context, std::ostream& output);
  CommandResult showWeekStats(std::istream& input, command_context_t& context,
      std::ostream& output);

  void printDayStats(const Profile& profile, const WeightHistory& history, const FoodDiary& diary,
      const Date& date, std::ostream& output);
}

#endif
