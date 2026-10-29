#ifndef EXPORT_COMMANDS_HPP
#define EXPORT_COMMANDS_HPP

#include <cstddef>
#include <iosfwd>
#include <string>
#include "meal_commands.hpp"
#include "profile.hpp"
#include "weight_commands.hpp"

namespace nabieva
{
  void exportReport(const Profile& profile, const WeightHistory& history,
    const FoodDiary& diary, const std::string& line, std::size_t position, std::ostream& output);
}

#endif
