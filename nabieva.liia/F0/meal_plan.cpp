#include "meal_plan.hpp"
#include <cmath>
#include <stdexcept>
#include "random_choice.hpp"

namespace nabieva
{
  namespace detail
  {
    struct knapsack_cell_t
    {
      nutrition_totals_t totals_ = {};
      bool reachable_ = false;
      std::size_t previousMask_ = 0;
      int grams_ = 0;
    };
  }

  static std::size_t getCategoryMask(const Food& food)
  {
    const std::string categories[] = { "main", "soup", "side", "dessert", "drink" };
    const std::size_t count = 5;
    for (std::size_t index = 0; index < count; ++index)
    {
      if (food.getCategory() == categories[index])
      {
        return std::size_t(1) << index;
      }
    }
    throw std::invalid_argument("неизвестная категория блюда.");
  }

  static std::size_t countCategories(std::size_t mask)
  {
    std::size_t count = 0;
    while (mask != 0)
    {
      count += mask % 2;
      mask /= 2;
    }
    return count;
  }

  static nutrition_totals_t getPortionTotals(const Food& food, double grams)
  {
    const double factor = grams / 100.0;
    const nutrition_totals_t totals = {
      food.getCalories() * factor, food.getProtein() * factor,
      food.getFat() * factor, food.getCarbs() * factor
    };
    return totals;
  }

  static nutrition_totals_t addTotals(
    const nutrition_totals_t& first, const nutrition_totals_t& second)
  {
    const nutrition_totals_t result = {
      first.calories_ + second.calories_, first.protein_ + second.protein_,
      first.fat_ + second.fat_, first.carbs_ + second.carbs_
    };
    return result;
  }

  static bool fitsCalories(const nutrition_totals_t& totals, double goal)
  {
    const double allowance = 1e-9;
    return std::isfinite(totals.calories_) && std::isfinite(totals.protein_)
      && std::isfinite(totals.fat_) && std::isfinite(totals.carbs_)
      && (totals.calories_ <= goal + allowance);
  }

  static void assessMealPlan(
    meal_plan_t& plan, const nutrition_totals_t& goals, const MealList* yesterdayMeals)
  {
    plan.totals_ = calculateNutrition(plan.meals_);
    plan.balanceScore_ = getBalanceScore(plan.totals_, goals);
    plan.hasMainDish_ = false;
    plan.repeatsYesterday_ = false;
    for (MealList::const_iterator current = plan.meals_.cbegin();
      current != plan.meals_.cend(); ++current)
    {
      const Food& food = current->getFood();
      plan.hasMainDish_ = plan.hasMainDish_ || (food.getCategory() == "main");
      plan.repeatsYesterday_ = plan.repeatsYesterday_ || wasMainDishInMeals(food, yesterdayMeals);
    }
  }

  static bool canReplaceFood(
    const MealList& meals, MealList::const_iterator replaced, const Food& food)
  {
    if ((replaced->getFood().getCategory() == "main") && (food.getCategory() != "main"))
    {
      return false;
    }
    for (MealList::const_iterator current = meals.cbegin(); current != meals.cend(); ++current)
    {
      if ((current != replaced) && (current->getFood().getCategory() == food.getCategory()))
      {
        return false;
      }
    }
    return true;
  }

  static bool improveMealChoice(meal_plan_t& plan, const FoodCatalog& catalog,
    const nutrition_totals_t& goals)
  {
    const List< std::string > names = getSortedFoodNames(catalog);
    MealList::iterator bestPosition = plan.meals_.end();
    const Food* bestFood = nullptr;
    int bestGrams = 0;
    double bestScore = plan.balanceScore_;
    const double improvementAllowance = 1e-10;
    const int minimumGrams = 50;
    const int maximumGrams = 300;
    const int portionStep = 25;
    for (MealList::iterator current = plan.meals_.begin(); current != plan.meals_.end(); ++current)
    {
      const nutrition_totals_t remaining = {
        std::fmax(0.0, plan.totals_.calories_ - current->getCalories()),
        std::fmax(0.0, plan.totals_.protein_ - current->getProtein()),
        std::fmax(0.0, plan.totals_.fat_ - current->getFat()),
        std::fmax(0.0, plan.totals_.carbs_ - current->getCarbs())
      };
      for (List< std::string >::const_iterator name = names.cbegin(); name != names.cend(); ++name)
      {
        const Food* const food = catalog.find(*name);
        if (!canReplaceFood(plan.meals_, current, *food))
        {
          continue;
        }
        for (int grams = minimumGrams; grams <= maximumGrams; grams += portionStep)
        {
          const nutrition_totals_t candidate = addTotals(remaining, getPortionTotals(*food, grams));
          if (!fitsCalories(candidate, goals.calories_))
          {
            continue;
          }
          const double score = getBalanceScore(candidate, goals);
          if (score + improvementAllowance < bestScore)
          {
            bestScore = score;
            bestFood = food;
            bestGrams = grams;
            bestPosition = current;
          }
        }
      }
    }
    if (bestFood == nullptr)
    {
      return false;
    }
    *bestPosition = Meal(*bestFood, bestGrams);
    plan.totals_ = calculateNutrition(plan.meals_);
    plan.balanceScore_ = getBalanceScore(plan.totals_, goals);
    return true;
  }

  static void improveMealPortions(meal_plan_t& plan, const FoodCatalog& catalog,
    const nutrition_totals_t& goals)
  {
    const std::size_t maximumSteps = 40;
    std::size_t steps = 0;
    while ((steps < maximumSteps) && (plan.balanceScore_ > 0.0)
      && improveMealChoice(plan, catalog, goals))
    {
      ++steps;
    }
    plan.portionSearchLimited_ = (steps == maximumSteps) && (plan.balanceScore_ > 0.0);
  }
}

nabieva::MealList nabieva::buildCaloriePlan(
  const FoodCatalog& catalog, const nutrition_totals_t& goals, bool requireMain)
{
  const double calorieStep = 10.0;
  const double maximumGoal = 10000.0;
  const std::size_t maximumCells = 2000000;
  const std::size_t maskCount = 32;
  const std::size_t mainMask = 1;
  const std::size_t maximumDishes = 3;
  const int minimumGrams = 50;
  const int maximumGrams = 300;
  const int portionStep = 25;
  const nutrition_totals_t empty = {};
  getBalanceScore(empty, goals);
  if (goals.calories_ > maximumGoal)
  {
    throw std::invalid_argument("цель приёма пищи слишком велика.");
  }
  const std::size_t capacityLimit = static_cast< std::size_t >(goals.calories_ / calorieStep);
  if (catalog.isEmpty() || (capacityLimit == 0))
  {
    return MealList();
  }
  const std::size_t columns = capacityLimit + 1;
  const std::size_t count = catalog.getSize();
  if (count > maximumCells / maskCount / columns - 1)
  {
    throw std::length_error("таблица подбора рациона слишком велика.");
  }
  const List< std::string > names = getSortedFoodNames(catalog);
  const Food** foods = nullptr;
  detail::knapsack_cell_t* table = nullptr;
  MealList result;
  try
  {
    foods = new const Food*[count]();
    std::size_t index = 0;
    for (List< std::string >::const_iterator current = names.cbegin(); current != names.cend(); ++current)
    {
      foods[index] = catalog.find(*current);
      ++index;
    }
    for (std::size_t remaining = count; remaining > 1; --remaining)
    {
      const std::size_t chosen = getRandomIndex(remaining);
      const Food* const temporary = foods[remaining - 1];
      foods[remaining - 1] = foods[chosen];
      foods[chosen] = temporary;
    }
    const std::size_t cells = (count + 1) * columns * maskCount;
    table = new detail::knapsack_cell_t[cells]();
    for (std::size_t capacity = 0; capacity <= capacityLimit; ++capacity)
    {
      table[capacity * maskCount].reachable_ = true;
    }
    for (std::size_t row = 1; row <= count; ++row)
    {
      const Food& food = *foods[row - 1];
      const std::size_t category = getCategoryMask(food);
      for (std::size_t capacity = 0; capacity <= capacityLimit; ++capacity)
      {
        for (std::size_t mask = 0; mask < maskCount; ++mask)
        {
          const std::size_t here = (row * columns + capacity) * maskCount + mask;
          const std::size_t before = ((row - 1) * columns + capacity) * maskCount + mask;
          table[here] = table[before];
          table[here].grams_ = 0;
          table[here].previousMask_ = mask;
        }
        for (std::size_t mask = 0; mask < maskCount; ++mask)
        {
          if (((mask & category) != 0) || (countCategories(mask) >= maximumDishes))
          {
            continue;
          }
          for (int grams = minimumGrams; grams <= maximumGrams; grams += portionStep)
          {
            const nutrition_totals_t portion = getPortionTotals(food, grams);
            if ((portion.calories_ <= 0.0) || !fitsCalories(portion, goals.calories_))
            {
              continue;
            }
            const std::size_t weight =
              static_cast< std::size_t >(std::ceil(portion.calories_ / calorieStep));
            if (weight > capacity)
            {
              continue;
            }
            const detail::knapsack_cell_t& previous =
              table[((row - 1) * columns + capacity - weight) * maskCount + mask];
            if (!previous.reachable_)
            {
              continue;
            }
            const nutrition_totals_t candidate = addTotals(previous.totals_, portion);
            if (!fitsCalories(candidate, goals.calories_))
            {
              continue;
            }
            detail::knapsack_cell_t& cell =
              table[(row * columns + capacity) * maskCount + (mask | category)];
            bool better = !cell.reachable_ || (candidate.calories_ > cell.totals_.calories_);
            if (cell.reachable_ && (candidate.calories_ == cell.totals_.calories_))
            {
              const double score = getBalanceScore(candidate, goals);
              const double previousScore = getBalanceScore(cell.totals_, goals);
              better = (score < previousScore)
                || ((score == previousScore) && (getRandomIndex(2) == 0));
            }
            if (better)
            {
              cell.totals_ = candidate;
              cell.reachable_ = true;
              cell.previousMask_ = mask;
              cell.grams_ = grams;
            }
          }
        }
      }
    }
    std::size_t bestMask = 0;
    std::size_t bestCapacity = 0;
    double bestScore = getBalanceScore(empty, goals);
    double bestCalories = 0.0;
    for (std::size_t capacity = 1; capacity <= capacityLimit; ++capacity)
    {
      for (std::size_t mask = 1; mask < maskCount; ++mask)
      {
        const detail::knapsack_cell_t& cell =
          table[(count * columns + capacity) * maskCount + mask];
        if (!cell.reachable_ || (requireMain && ((mask & mainMask) == 0)))
        {
          continue;
        }
        const double score = getBalanceScore(cell.totals_, goals);
        const bool betterCalories = cell.totals_.calories_ > bestCalories;
        const bool moreCategories = (cell.totals_.calories_ == bestCalories)
          && (countCategories(mask) > countCategories(bestMask));
        if ((bestMask == 0) || (score < bestScore)
          || ((score == bestScore) && (betterCalories || moreCategories)))
        {
          bestMask = mask;
          bestCapacity = capacity;
          bestScore = score;
          bestCalories = cell.totals_.calories_;
        }
      }
    }
    for (std::size_t row = count; (row > 0) && (bestMask != 0); --row)
    {
      const detail::knapsack_cell_t& cell =
        table[(row * columns + bestCapacity) * maskCount + bestMask];
      if (cell.grams_ != 0)
      {
        const Food& food = *foods[row - 1];
        result.pushFront(Meal(food, cell.grams_));
        const double calories = getPortionTotals(food, cell.grams_).calories_;
        bestCapacity -= static_cast< std::size_t >(std::ceil(calories / calorieStep));
      }
      bestMask = cell.previousMask_;
    }
  }
  catch (...)
  {
    delete[] table;
    delete[] foods;
    throw;
  }
  delete[] table;
  delete[] foods;
  return result;
}

bool nabieva::wasMainDishInMeals(
  const Food& food, const MealList* yesterdayMeals)
{
  if ((food.getCategory() != "main")
    && (food.getCategory() != "soup"))
  {
    return false;
  }

  if (yesterdayMeals == nullptr)
  {
    return false;
  }

  for (MealList::const_iterator current = yesterdayMeals->cbegin();
    current != yesterdayMeals->cend(); ++current)
  {
    if (current->getFood().getName() == food.getName())
    {
      return true;
    }
  }

  return false;
}

nabieva::meal_plan_t nabieva::buildMealPlan(
  const FoodCatalog& catalog,
  const nutrition_totals_t& goals,
  const MealList* yesterdayMeals)
{
  const nutrition_totals_t emptyTotals = {};
  getBalanceScore(emptyTotals, goals);

  const std::size_t bucketCount = 16;
  FoodCatalog preferredCatalog(bucketCount);

  const List< std::string > names = catalog.getKeys();

  bool excludedFood = false;
  bool freshMainFits = false;
  bool anyMainFits = false;

  for (List< std::string >::const_iterator current = names.cbegin();
    current != names.cend(); ++current)
  {
    const Food* const food = catalog.find(*current);
    const double portionUnits =
      std::max(1.0, std::ceil(food->getCalories() * 0.5 / 10.0));
    const double goalUnits = std::floor(goals.calories_ / 10.0);
    const bool mainFits = (food->getCategory() == "main")
      && (food->getCalories() > 0.0) && (portionUnits <= goalUnits);

    anyMainFits = anyMainFits || mainFits;

    if (wasMainDishInMeals(*food, yesterdayMeals))
    {
      excludedFood = true;
    }
    else
    {
      preferredCatalog.add(food->getName(), *food);
      freshMainFits = freshMainFits || mainFits;
    }
  }

  meal_plan_t result = {};

  result.meals_ = buildCaloriePlan(
    preferredCatalog, goals, freshMainFits);

  assessMealPlan(result, goals, yesterdayMeals);
  improveMealPortions(result, preferredCatalog, goals);
  assessMealPlan(result, goals, yesterdayMeals);

  if (result.hasMainDish_ || freshMainFits || !excludedFood)
  {
    return result;
  }

  meal_plan_t fallback = {};

  fallback.meals_ = buildCaloriePlan(
    catalog, goals, anyMainFits);

  assessMealPlan(fallback, goals, yesterdayMeals);
  improveMealPortions(fallback, catalog, goals);
  assessMealPlan(fallback, goals, yesterdayMeals);

  if (fallback.hasMainDish_ || result.meals_.isEmpty())
  {
    return fallback;
  }

  return result;
}
namespace nabieva
{
  static meal_plan_t buildNextMeal(
    const FoodCatalog& catalog,
    const nutrition_totals_t& goals,
    const MealList* yesterdayMeals,
    const MealList& todayMeals)
  {
    const std::size_t bucketCount = 16;
    FoodCatalog preferredCatalog(bucketCount);

    const List< std::string > names = catalog.getKeys();

    bool excludedFood = false;

    for (List< std::string >::const_iterator current = names.cbegin();
      current != names.cend(); ++current)
    {
      const Food* const food = catalog.find(*current);

      if (wasMainDishInMeals(*food, &todayMeals))
      {
        excludedFood = true;
      }
      else
      {
        preferredCatalog.add(food->getName(), *food);
      }
    }

    meal_plan_t result = buildMealPlan(
      preferredCatalog, goals, yesterdayMeals);

    if (excludedFood
      && (!result.hasMainDish_ || result.meals_.isEmpty()))
    {
      meal_plan_t fallback = buildMealPlan(
        catalog, goals, yesterdayMeals);

      if (fallback.hasMainDish_ || result.meals_.isEmpty())
      {
        result = fallback;
      }
    }

    result.repeatsToday_ = false;

    for (MealList::const_iterator current = result.meals_.cbegin();
      current != result.meals_.cend(); ++current)
    {
      if (wasMainDishInMeals(current->getFood(), &todayMeals))
      {
        result.repeatsToday_ = true;
      }
    }

    return result;
  }
}
nabieva::daily_menu_t nabieva::buildDailyMenu(
  const FoodCatalog& catalog,
  const nutrition_totals_t& goals,
  const MealList* yesterdayMeals)
{
  const double shares[3] = { 0.30, 0.40, 0.30 };
  daily_menu_t result = {};
  MealList::iterator last = result.meals_.end();

  for (std::size_t index = 0; index < 3; ++index)
  {
    const nutrition_totals_t partGoals = {
      goals.calories_ * shares[index],
      goals.protein_ * shares[index],
      goals.fat_ * shares[index],
      goals.carbs_ * shares[index]
    };

    const std::size_t bucketCount = 16;
    FoodCatalog availableCatalog(bucketCount);

    const List< std::string > names = catalog.getKeys();

    for (List< std::string >::const_iterator current = names.cbegin();
      current != names.cend(); ++current)
    {
      const Food* const food = catalog.find(*current);

      if ((food->getCategory() == "soup") && (index != 1))
      {
        continue;
      }

      availableCatalog.add(food->getName(), *food);
    }

    result.parts_[index] = buildNextMeal(
      availableCatalog, partGoals, yesterdayMeals, result.meals_);

    const MealList& portions = result.parts_[index].meals_;

    for (MealList::const_iterator current = portions.cbegin();
      current != portions.cend(); ++current)
    {
      if (result.meals_.isEmpty())
      {
        result.meals_.pushFront(*current);
        last = result.meals_.begin();
      }
      else
      {
        last = result.meals_.insertAfter(last, *current);
      }
    }
  }

  result.totals_ = calculateNutrition(result.meals_);
  result.balanceScore_ = getBalanceScore(result.totals_, goals);

  return result;
}
