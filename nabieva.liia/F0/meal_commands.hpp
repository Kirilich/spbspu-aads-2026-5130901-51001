#ifndef MEAL_COMMANDS_HPP
#define MEAL_COMMANDS_HPP

#include <iosfwd>
#include "bstree.hpp"
#include "date.hpp"
#include "list.hpp"
#include "meal.hpp"

namespace nabieva
{
  enum class CommandResult;
  struct command_context_t;

  using MealList = List< Meal >;
  using FoodDiary = BSTree< Date, MealList >;

  CommandResult recordMeal(std::istream& input, command_context_t& context, std::ostream& output);
  CommandResult removeMeal(std::istream& input, command_context_t& context, std::ostream& output);
  CommandResult listMeals(std::istream& input, command_context_t& context, std::ostream& output);

  void printMeals(const FoodDiary& diary, const Date& date, std::ostream& output);
}

#endif
