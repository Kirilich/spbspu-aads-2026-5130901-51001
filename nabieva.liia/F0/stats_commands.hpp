#ifndef STATS_COMMANDS_HPP
#define STATS_COMMANDS_HPP

#include <cstddef>
#include <iosfwd>
#include <string>
#include "meal_commands.hpp"
#include "profile.hpp"
#include "weight_commands.hpp"

namespace nabieva
{
  void showDayStats(const Profile& profile, const WeightHistory& history,
    const FoodDiary& diary, const std::string& line, std::size_t position, std::ostream& output);
  void showWeekStats(const Profile& profile, const WeightHistory& history,
    const FoodDiary& diary, const std::string& line, std::size_t position, std::ostream& output);
}

#endif
