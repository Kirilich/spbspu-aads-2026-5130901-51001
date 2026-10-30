#ifndef NUTRITION_HPP
#define NUTRITION_HPP

#include "list.hpp"
#include "meal.hpp"

namespace nabieva
{
  class Profile;

  struct nutrition_totals_t
  {
    double calories_;
    double protein_;
    double fat_;
    double carbs_;
  };

  nutrition_totals_t calculateNutrition(const List< Meal >& meals);

  double getDeviation(double value, double goal);

  double getBalanceScore(const nutrition_totals_t& totals, const nutrition_totals_t& goals);

  double getBalanceScore(const nutrition_totals_t& totals, const Profile& profile);
}

#endif
