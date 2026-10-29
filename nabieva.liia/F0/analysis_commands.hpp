#ifndef ANALYSIS_COMMANDS_HPP
#define ANALYSIS_COMMANDS_HPP

#include <cstddef>
#include <iosfwd>
#include <string>
#include "meal_commands.hpp"
#include "food_commands.hpp"
#include "profile.hpp"

namespace nabieva
{
  void analyzeDay(const Profile& profile, const FoodDiary& diary,
    const std::string& line, std::size_t position, std::ostream& output);
  void recommendMeal(const Profile& profile, const FoodCatalog& catalog,
    const FoodDiary& diary, const std::string& line,
    std::size_t position, std::ostream& output);
}

#endif
