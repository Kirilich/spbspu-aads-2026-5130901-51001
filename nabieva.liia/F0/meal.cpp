#include "meal.hpp"
#include <cmath>
#include <stdexcept>

nabieva::Meal::Meal(const Food& food, double grams):
  food_(food),
  grams_(grams)
{
  if (!std::isfinite(grams_) || (grams_ <= 0.0))
  {
    throw std::invalid_argument("масса порции должна быть положительным конечным числом.");
  }

  if (!std::isfinite(getCalories()) || !std::isfinite(getProtein()) || !std::isfinite(getFat())
      || !std::isfinite(getCarbs()))
  {
    throw std::invalid_argument("значения кбжу порции слишком велики.");
  }
}
const nabieva::Food& nabieva::Meal::getFood() const
{
  return food_;
}

double nabieva::Meal::getGrams() const
{
  return grams_;
}

double nabieva::Meal::getCalories() const
{
  return calculatePortion(food_.getCalories());
}

double nabieva::Meal::getProtein() const
{
  return calculatePortion(food_.getProtein());
}

double nabieva::Meal::getFat() const
{
  return calculatePortion(food_.getFat());
}

double nabieva::Meal::getCarbs() const
{
  return calculatePortion(food_.getCarbs());
}

double nabieva::Meal::calculatePortion(double value) const
{
  const double referenceGrams = 100.0;
  const double portionFactor = grams_ / referenceGrams;
  return value * portionFactor;
}
