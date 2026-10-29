#include "nutrition.hpp"
#include <cmath>
#include <stdexcept>

nabieva::nutrition_totals_t nabieva::calculateNutrition(const List< Meal >& meals)
{
  nutrition_totals_t totals = { 0.0, 0.0, 0.0, 0.0 };

  for (List< Meal >::const_iterator current = meals.cbegin(); current != meals.cend(); ++current)
  {
    totals.calories_ += current->getCalories();
    totals.protein_ += current->getProtein();
    totals.fat_ += current->getFat();
    totals.carbs_ += current->getCarbs();
  }

  if (!std::isfinite(totals.calories_))
  {
    throw std::overflow_error("сумма калорий за день слишком велика.");
  }

  if (!std::isfinite(totals.protein_)
    || !std::isfinite(totals.fat_) || !std::isfinite(totals.carbs_))
  {
    throw std::overflow_error("суммарные бжу за день слишком велики.");
  }

  return totals;
}

double nabieva::getDeviation(double value, double goal)
{
  if (!std::isfinite(value) || !std::isfinite(goal)
    || (value < 0.0) || (goal <= 0.0))
  {
    throw std::invalid_argument("неверное значение кбжу или цели.");
  }

  const double tolerance = 0.10;
  const double roundingAllowance = 1e-9;
  const double relative = value / goal;

  if (!std::isfinite(relative))
  {
    throw std::overflow_error("отклонение кбжу слишком велико.");
  }

  const double distance = std::abs(relative - 1.0);

  if (distance <= tolerance + roundingAllowance)
  {
    return 0.0;
  }

  return distance - tolerance;
}

double nabieva::getBalanceScore(
  const nutrition_totals_t& totals,
  const nutrition_totals_t& goals)
{
  const double calories = getDeviation(
    totals.calories_, goals.calories_);

  const double protein = getDeviation(
    totals.protein_, goals.protein_);

  const double fat = getDeviation(
    totals.fat_, goals.fat_);

  const double carbs = getDeviation(
    totals.carbs_, goals.carbs_);

  const double score = calories * calories
    + protein * protein
    + fat * fat
    + carbs * carbs;

  if (!std::isfinite(score))
  {
    throw std::overflow_error("оценка отклонения кбжу слишком велика.");
  }

  return score;
}
double nabieva::getBalanceScore(
  const nutrition_totals_t& totals,
  const Profile& profile)
{
  const nutrition_totals_t goals = {
    profile.getDailyCaloriesGoal(),
    profile.getDailyProteinGoal(),
    profile.getDailyFatGoal(),
    profile.getDailyCarbsGoal()
  };

  return getBalanceScore(totals, goals);
}
