#ifndef MEAL_COMMANDS_HPP
#define MEAL_COMMANDS_HPP

#include <cstddef>
#include <iosfwd>
#include <string>
#include "bstree.hpp"
#include "date.hpp"
#include "food_catalog.hpp"
#include "list.hpp"
#include "meal.hpp"

namespace nabieva
{
  using MealList = List< Meal >;
  using FoodDiary = BSTree< Date, MealList >;

  bool recordMeal(const FoodCatalog& catalog, FoodDiary& diary,
    const std::string& line, std::size_t position, std::ostream& output);
  void listMeals(const FoodDiary& diary, const std::string& line,
    std::size_t position, std::ostream& output);
  bool removeMeal(FoodDiary& diary, const std::string& line,
    std::size_t position, std::ostream& output);
}

#endif
