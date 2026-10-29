#include "analysis_commands.hpp"
#include "random_choice.hpp"
#include "console.hpp"
#include <cmath>
#include <iomanip>
#include <ostream>
#include <stdexcept>
#include <algorithm>
#include "input.hpp"
#include "nutrition.hpp"
#include "meal_plan.hpp"

namespace nabieva
{
  namespace detail
  {
    struct reduction_t
    {
      std::size_t number_;
      Meal meal_;
      std::size_t removedParts_;
    };

    struct recommendation_t
    {
      const Food* food_;
      int grams_;
      nutrition_totals_t totals_;
      double score_;
      bool repeated_;
    };

    using Reductions = List< reduction_t >;

    const std::size_t portionParts = 10;
  }
  static bool keepsUpperBound(double before, double after, double goal)
  {
    const double upperBound = std::max(before / goal, 1.10);
    const double roundingAllowance = 1e-9;

    return after / goal <= upperBound + roundingAllowance;
  }
  static bool canAddMeal(
    const nutrition_totals_t& before,
    const nutrition_totals_t& after,
    const Profile& profile)
  {
    if (!std::isfinite(after.calories_)
      || !std::isfinite(after.protein_)
      || !std::isfinite(after.fat_)
      || !std::isfinite(after.carbs_))
    {
      return false;
    }

    const double roundingAllowance = 1e-9;

    return (after.calories_ / profile.getDailyCaloriesGoal()
      <= 1.0 + roundingAllowance)
      && keepsUpperBound(
        before.protein_, after.protein_,
        profile.getDailyProteinGoal())
      && keepsUpperBound(
        before.fat_, after.fat_,
        profile.getDailyFatGoal())
      && keepsUpperBound(
        before.carbs_, after.carbs_,
        profile.getDailyCarbsGoal());
  }
  static detail::Reductions copyMeals(const MealList& meals)
  {
    detail::Reductions result;
    detail::Reductions::iterator last = result.end();
    std::size_t number = 0;

    for (MealList::const_iterator current = meals.cbegin();
      current != meals.cend(); ++current)
    {
      ++number;

      const detail::reduction_t entry = {
        number,
        *current,
        0
      };

      if (result.isEmpty())
      {
        result.pushFront(entry);
        last = result.begin();
      }
      else
      {
        last = result.insertAfter(last, entry);
      }
    }

    return result;
  }
  static nutrition_totals_t removeOnePart(
    const nutrition_totals_t& totals, const Meal& meal)
  {
    const double parts = static_cast<double>(detail::portionParts);

    const nutrition_totals_t result = {
      std::max(0.0, totals.calories_ - meal.getCalories() / parts),
      std::max(0.0, totals.protein_ - meal.getProtein() / parts),
      std::max(0.0, totals.fat_ - meal.getFat() / parts),
      std::max(0.0, totals.carbs_ - meal.getCarbs() / parts)
    };

    return result;
  }
  static bool keepsLowerBound(double before, double after, double goal)
  {
    const double lowerBound = goal * 0.90;
    const double roundingAllowance = goal * 1e-9;

    return after + roundingAllowance
      >= std::min(before, lowerBound);
  }

  static bool keepsLowerBounds(
    const nutrition_totals_t& before,
    const nutrition_totals_t& after,
    const Profile& profile)
  {
    return keepsLowerBound(
      before.calories_, after.calories_,
      profile.getDailyCaloriesGoal())
      && keepsLowerBound(
        before.protein_, after.protein_,
        profile.getDailyProteinGoal())
      && keepsLowerBound(
        before.fat_, after.fat_,
        profile.getDailyFatGoal())
      && keepsLowerBound(
        before.carbs_, after.carbs_,
        profile.getDailyCarbsGoal());
  }
  static bool improvePlan(
    detail::Reductions& reductions,
    nutrition_totals_t& totals,
    const Profile& profile)
  {
    detail::Reductions::iterator best = reductions.end();
    nutrition_totals_t bestTotals = totals;
    double bestScore = getBalanceScore(totals, profile);

    for (detail::Reductions::iterator current = reductions.begin();
      current != reductions.end(); ++current)
    {
      if (current->removedParts_ == detail::portionParts)
      {
        continue;
      }

      const nutrition_totals_t candidate =
        removeOnePart(totals, current->meal_);

      if (!keepsLowerBounds(totals, candidate, profile))
      {
        continue;
      }

      const double score = getBalanceScore(candidate, profile);

      if (score < bestScore)
      {
        best = current;
        bestTotals = candidate;
        bestScore = score;
      }
    }

    if (best == reductions.end())
    {
      return false;
    }

    ++best->removedParts_;
    totals = bestTotals;

    return true;
  }
  static void printIndicator(
    const char* name, double value, double goal, std::ostream& output)
  {
    printText(output, name, 15);
    output << std::right << std::setw(14) << value;
    output << std::setw(14) << goal << "  ";

    if (getDeviation(value, goal) == 0.0)
    {
      output << "в норме\n";
    }
    else if (value < goal)
    {
      output << "недостаток\n";
    }
    else
    {
      output << "избыток\n";
    }
  }
  static void printNutrition(
    const nutrition_totals_t& totals,
    const Profile& profile,
    std::ostream& output)
  {
    const std::ios::fmtflags originalFlags = output.flags();
    const std::streamsize originalPrecision = output.precision();

    output << std::fixed << std::setprecision(0);

    printText(output, "показатель", 15);
    printText(output, "записано", 14, false);
    printText(output, "цель", 14, false);
    output << "  статус\n";

    printIndicator(
      "калории, ккал", totals.calories_,
      profile.getDailyCaloriesGoal(), output);

    printIndicator(
      "белки, г", totals.protein_,
      profile.getDailyProteinGoal(), output);

    printIndicator(
      "жиры, г", totals.fat_,
      profile.getDailyFatGoal(), output);

    printIndicator(
      "углеводы, г", totals.carbs_,
      profile.getDailyCarbsGoal(), output);

    output.flags(originalFlags);
    output.precision(originalPrecision);
  }
  static void printReductions(
    const detail::Reductions& reductions,
    std::ostream& output)
  {
    const std::ios::fmtflags originalFlags = output.flags();
    const std::streamsize originalPrecision = output.precision();

    output << std::defaultfloat << std::setprecision(6);

    for (detail::Reductions::const_iterator current = reductions.cbegin();
      current != reductions.cend(); ++current)
    {
      if (current->removedParts_ > 0)
      {
        const double fraction =
          static_cast<double>(current->removedParts_)
          / detail::portionParts;

        const double grams = current->meal_.getGrams() * fraction;

        output << current->number_ << ". ";
        output << current->meal_.getFood().getName();
        output << " | уменьшить на " << grams << " г\n";
      }
    }

    output.flags(originalFlags);
    output.precision(originalPrecision);
  }
}

void nabieva::analyzeDay(const Profile& profile, const FoodDiary& diary,
  const std::string& line, std::size_t position, std::ostream& output)
{
  const Date date = readOptionalDate(line, position);

  if (!profile.hasData())
  {
    output << "создайте профиль командой \"profile\".\n";
    return;
  }

  const MealList* const meals = diary.find(date);

  if ((meals == nullptr) || meals->isEmpty())
  {
    output << "нет записей о питании за " << date.getText() << ".\n";
    return;
  }

  const nutrition_totals_t totals = calculateNutrition(*meals);
  const double originalScore = getBalanceScore(totals, profile);

  nutrition_totals_t planned = totals;
  detail::Reductions reductions;
  bool changed = false;

  if (originalScore > 0.0)
  {
    reductions = copyMeals(*meals);

    while ((getBalanceScore(planned, profile) > 0.0)
      && improvePlan(reductions, planned, profile))
    {
      changed = true;
    }
  }

  output << "анализ питания за " << date.getText() << ":\n";
  output << "допустимое отклонение от каждой цели: +/-10%.\n";

  printNutrition(totals, profile, output);

  output << "при сравнении используются неокруглённые значения.\n";

  if (originalScore == 0.0)
  {
    output << "отлично! калории и все показатели бжу "
      << "находятся в пределах ваших целей.\n";
    return;
  }

  if (!changed)
  {
    output << "не найдено уменьшающих порции изменений, улучшающих баланс, "
      << "при шаге 10% от исходной порции.\n";

    output << "возможно, нужно добавить или заменить блюда; "
      << "проверьте статусы показателей выше.\n";

    return;
  }

  output << "предлагаемые уменьшения (номера исходных записей):\n";
  printReductions(reductions, output);

  output << "расчётные итоги после уменьшения порций:\n";
  printNutrition(planned, profile, output);

  if (getBalanceScore(planned, profile) == 0.0)
  {
    output << "предложенные порции позволят достичь "
      << "всех четырёх целевых диапазонов.\n";
  }
  else
  {
    output << "баланс улучшился, но некоторые показатели "
      << "остаются за пределами целевых диапазонов.\n";

    output << "дальнейших уменьшений, улучшающих баланс, не найдено "
      << "при шаге 10% от исходной порции.\n";
  }
}
void nabieva::recommendMeal(
  const Profile& profile,
  const FoodCatalog& catalog,
  const FoodDiary& diary,
  const std::string& line,
  std::size_t position,
  std::ostream& output)
{
  std::string extra = "";

  if (readToken(line, position, extra))
  {
    throw std::invalid_argument("указан лишний аргумент.");
  }

  if (!profile.hasData())
  {
    output << "создайте профиль командой \"profile\".\n";
    return;
  }

  const Date date = getCurrentDate();
  const MealList* const meals = diary.find(date);

  nutrition_totals_t totals = {};

  if (meals != nullptr)
  {
    totals = calculateNutrition(*meals);
  }

  const double goal = profile.getDailyCaloriesGoal();

  if (totals.calories_ > goal)
  {
    output << "суточная цель по калориям уже превышена.\n";
    return;
  }

  if (totals.calories_ == goal)
  {
    output << "суточная цель по калориям уже достигнута.\n";
    return;
  }

  const double originalScore = getBalanceScore(totals, profile);

  if (originalScore == 0.0)
  {
    output << "калории и все показатели бжу "
      << "уже находятся в пределах ваших целей.\n";
    return;
  }

  if (catalog.isEmpty())
  {
    output << "каталог блюд пуст.\n";
    return;
  }
  const Date minimumDate(1, 1, 1);
  const MealList* yesterdayMeals = nullptr;
  if (date > minimumDate)
  {
    yesterdayMeals = diary.find(date.getPreviousDay());
  }
  const List< std::string > names = catalog.getKeys();

  const Food* bestFood = nullptr;
  double bestGrams = 0.0;
  nutrition_totals_t bestTotals = totals;
  double bestScore = originalScore;
  bool bestRepeated = false;
  List< detail::recommendation_t > candidates;
  const int minimumGrams = 50;
  const int maximumGrams = 300;
  const int portionStep = 25;

  for (List< std::string >::const_iterator current = names.cbegin();
    current != names.cend(); ++current)
  {
    const Food* const food = catalog.find(*current);
    const bool repeated =
      wasMainDishInMeals(*food, yesterdayMeals);
    detail::recommendation_t foodChoice = { nullptr, 0, {}, originalScore, repeated };
    for (int grams = minimumGrams;
      grams <= maximumGrams;
      grams += portionStep)
    {
      const double factor = grams / 100.0;

      const nutrition_totals_t candidate = {
        totals.calories_ + food->getCalories() * factor,
        totals.protein_ + food->getProtein() * factor,
        totals.fat_ + food->getFat() * factor,
        totals.carbs_ + food->getCarbs() * factor
      };

      if (!canAddMeal(totals, candidate, profile))
      {
        continue;
      }

      const double score = getBalanceScore(candidate, profile);

      if (score >= originalScore)
      {
        continue;
      }

      if ((foodChoice.food_ == nullptr) || (score < foodChoice.score_)
        || ((score == foodChoice.score_) && (candidate.calories_ < foodChoice.totals_.calories_)))
      {
        foodChoice = { food, grams, candidate, score, repeated };
      }
    }
    if (foodChoice.food_ == nullptr)
    {
      continue;
    }
    candidates.pushFront(foodChoice);
    if ((bestFood == nullptr) || (bestRepeated && !repeated)
      || ((repeated == bestRepeated) && (foodChoice.score_ < bestScore)))
    {
      bestFood = food;
      bestScore = foodChoice.score_;
      bestRepeated = repeated;
    }
  }
  if (bestFood == nullptr)
  {
    output << "подходящее блюдо не найдено для порций "
      << "от 50 до 300 г (шаг 25 г).\n";
    return;
  }

  const double relativeAllowance = 0.10;
  const double maximumAllowance = 0.02;
  const double scoreAllowance = std::min(maximumAllowance, bestScore * relativeAllowance);
  const double scoreLimit = bestScore + scoreAllowance;
  std::size_t suitableChoices = 0;
  for (List< detail::recommendation_t >::const_iterator current = candidates.cbegin();
    current != candidates.cend(); ++current)
  {
    if ((current->repeated_ != bestRepeated) || (current->score_ > scoreLimit))
    {
      continue;
    }
    ++suitableChoices;
    if (getRandomIndex(suitableChoices) == 0)
    {
      bestFood = current->food_;
      bestGrams = current->grams_;
      bestTotals = current->totals_;
    }
  }
  bestScore = getBalanceScore(bestTotals, profile);

  const Meal recommendation(*bestFood, bestGrams);

  const std::ios::fmtflags originalFlags = output.flags();
  const std::streamsize originalPrecision = output.precision();

  output << std::fixed << std::setprecision(0);

  output << "рекомендуемое блюдо на " << date.getText() << ":\n";
  output << "осталось: " << goal - totals.calories_ << " ккал\n";

  output << bestFood->getName() << " | " << bestGrams << " г";
  output << " | " << recommendation.getCalories() << " ккал";
  output << " | б:" << recommendation.getProtein() << " г";
  output << " ж:" << recommendation.getFat() << " г";
  output << " у:" << recommendation.getCarbs() << " г\n";

  if (bestRepeated)
  {
    output << "это блюдо было записано вчера. "
      << "подходящей замены не найдено.\n";
  }
  output << "расчётные итоги дня после этого блюда:\n";
  printNutrition(bestTotals, profile, output);

  if (bestScore == 0.0)
  {
    output << "эта порция приведёт все четыре показателя "
      << "в целевые диапазоны.\n";
  }
  else
  {
    output << "эта порция улучшает баланс; "
      << "некоторые показатели остаются за пределами целей.\n";
  }

  output << "для записи: eat " << bestFood->getName();
  output << ' ' << bestGrams << '\n';

  output.flags(originalFlags);
  output.precision(originalPrecision);
}
