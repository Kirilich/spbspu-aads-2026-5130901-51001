#include "meal_commands.hpp"
#include "console.hpp"
#include "nutrition.hpp"
#include <cmath>
#include <iomanip>
#include <ostream>
#include <stdexcept>
#include "input.hpp"

bool nabieva::recordMeal(const FoodCatalog& catalog, FoodDiary& diary,
  const std::string& line, std::size_t position, std::ostream& output)
{
  std::string name = "";
  std::string gramsText = "";
  double grams = 0.0;

  if (!readToken(line, position, name) || !readToken(line, position, gramsText)
    || !readNumber(gramsText, grams) || (grams <= 0.0))
  {
    throw std::invalid_argument("неверные параметры приёма пищи.");
  }

  const Date date = readOptionalDate(line, position);
  const Food* const food = catalog.find(name);

  if (food == nullptr)
  {
    output << "блюдо не найдено.\n";
    return false;
  }

  const Meal meal(*food, grams);
  MealList* const meals = diary.find(date);
  nutrition_totals_t totals = {};
  if (meals != nullptr)
  {
    totals = calculateNutrition(*meals);
  }
  if (!std::isfinite(totals.calories_ + meal.getCalories())
    || !std::isfinite(totals.protein_ + meal.getProtein())
    || !std::isfinite(totals.fat_ + meal.getFat())
    || !std::isfinite(totals.carbs_ + meal.getCarbs()))
  {
    throw std::invalid_argument("суммарные кбжу за день получились бы слишком большими.");
  }

  if (meals == nullptr)
  {
    MealList newMeals;
    newMeals.pushFront(meal);
    diary.add(date, newMeals);
  }
  else if (meals->isEmpty())
  {
    meals->pushFront(meal);
  }
  else
  {
    MealList::iterator last = meals->begin();
    MealList::iterator next = last;
    ++next;

    while (next != meals->end())
    {
      last = next;
      ++next;
    }

    meals->insertAfter(last, meal);
  }

  output << "приём пищи записан.\n";
  return true;
}
bool nabieva::removeMeal(FoodDiary& diary, const std::string& line,
  std::size_t position, std::ostream& output)
{
  std::string text = "";
  std::size_t number = 0;

  if (!readToken(line, position, text) || !readPositiveInteger(text, number))
  {
    throw std::invalid_argument("неверный номер записи о питании.");
  }

  const Date date = readOptionalDate(line, position);
  MealList* const meals = diary.find(date);

  if ((meals == nullptr) || meals->isEmpty())
  {
    output << "запись о приёме пищи не найдена.\n";
    return false;
  }

  const std::size_t firstNumber = 1;
  std::size_t currentNumber = firstNumber;
  MealList::iterator current = meals->begin();
  MealList::iterator previous = current;

  while ((current != meals->end()) && (currentNumber < number))
  {
    previous = current;
    ++current;
    ++currentNumber;
  }

  if (current == meals->end())
  {
    output << "запись о приёме пищи не найдена.\n";
    return false;
  }

  if (number == firstNumber)
  {
    meals->popFront();
  }
  else
  {
    meals->eraseAfter(previous);
  }

  if (meals->isEmpty())
  {
    diary.remove(date);
  }

  output << "запись о приёме пищи удалена.\n";
  return true;
}

void nabieva::listMeals(const FoodDiary& diary, const std::string& line,
  std::size_t position, std::ostream& output)
{
  const Date date = readOptionalDate(line, position);
  const MealList* const meals = diary.find(date);

  if ((meals == nullptr) || meals->isEmpty())
  {
    output << "нет записей о питании за " << date.getText() << ".\n";
    return;
  }

  const std::size_t minimumNameWidth = getTextWidth("блюдо");
  const std::size_t columnGap = 2;
  std::size_t nameWidth = minimumNameWidth;
  const nutrition_totals_t totals = calculateNutrition(*meals);

  for (MealList::const_iterator current = meals->cbegin(); current != meals->cend(); ++current)
  {
    const std::size_t length = getTextWidth(current->getFood().getName());

    if (length > nameWidth)
    {
      nameWidth = length;
    }
  }
  nameWidth += columnGap;
  const int numberWidth = 5;
  const int numericWidth = 14;
  const std::ios::fmtflags originalFlags = output.flags();

  output << "питание за " << date.getText() << ":\n";
  printText(output, "№", numberWidth, false);
  output << "  ";
  printText(output, "блюдо", nameWidth);
  printText(output, "граммы", numericWidth, false);
  printText(output, "ккал", numericWidth, false);
  printText(output, "белки, г", numericWidth, false);
  printText(output, "жиры, г", numericWidth, false);
  printText(output, "углеводы, г", numericWidth, false);
  output << '\n';

  std::size_t number = 0;
  for (MealList::const_iterator current = meals->cbegin(); current != meals->cend(); ++current)
  {
    ++number;

    output << std::right << std::setw(numberWidth) << number << "  ";
    printText(output, current->getFood().getName(), nameWidth);
    output << std::right << std::setw(numericWidth) << current->getGrams();
    output << std::setw(numericWidth) << current->getCalories();

    output << std::setw(numericWidth) << current->getProtein();
    output << std::setw(numericWidth) << current->getFat();
    output << std::setw(numericWidth) << current->getCarbs();

    output << '\n';
  }

  output.flags(originalFlags);
  output << "итого: " << totals.calories_ << " ккал\n";

  output << "б:" << totals.protein_ << "г ж:" << totals.fat_ << "г у:" << totals.carbs_ << "г\n";
}
