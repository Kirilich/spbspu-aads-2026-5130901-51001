#ifndef MEAL_PLAN_HPP
#define MEAL_PLAN_HPP

#include "food_catalog.hpp"
#include "meal_commands.hpp"
#include "nutrition.hpp"

namespace nabieva
{
  struct meal_plan_t
  {
    MealList meals_;
    nutrition_totals_t totals_ = {};
    double balanceScore_ = 0.0;
    bool hasMainDish_ = false;
    bool repeatsYesterday_ = false;
    bool repeatsToday_ = false;
    bool portionSearchLimited_ = false;
  };

  struct daily_menu_t
  {
    meal_plan_t parts_[3] = {};
    MealList meals_;
    nutrition_totals_t totals_ = {};
    double balanceScore_ = 0.0;
  };

  daily_menu_t buildDailyMenu(
    const FoodCatalog& catalog,
    const nutrition_totals_t& goals,
    const MealList* yesterdayMeals = nullptr);

  bool wasMainDishInMeals(
    const Food& food,
    const MealList* yesterdayMeals);

  meal_plan_t buildMealPlan(
    const FoodCatalog& catalog,
    const nutrition_totals_t& goals,
    const MealList* yesterdayMeals = nullptr);

  MealList buildCaloriePlan(
    const FoodCatalog& catalog,
    const nutrition_totals_t& goals,
    bool requireMain = false);
}

#endif
