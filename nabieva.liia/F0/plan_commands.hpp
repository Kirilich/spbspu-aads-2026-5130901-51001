#ifndef PLAN_COMMANDS_HPP
#define PLAN_COMMANDS_HPP

#include <cstddef>
#include <iosfwd>
#include <string>
#include "meal_plan.hpp"

namespace nabieva
{
  void generatePlan(
    const Profile& profile,
    const FoodCatalog& catalog,
    const FoodDiary& diary,
    const std::string& line,
    std::size_t position,
    std::ostream& output);
}

#endif
