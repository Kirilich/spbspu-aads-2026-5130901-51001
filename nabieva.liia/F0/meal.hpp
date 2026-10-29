#ifndef MEAL_HPP
#define MEAL_HPP

#include "food.hpp"

namespace nabieva
{
  class Meal
  {
  public:
    Meal(const Food& food, double grams);

    const Food& getFood() const;
    double getGrams() const;
    double getCalories() const;
    double getProtein() const;
    double getFat() const;
    double getCarbs() const;

  private:
    Food food_;
    double grams_;

    double calculatePortion(double value) const;
  };
}

#endif
