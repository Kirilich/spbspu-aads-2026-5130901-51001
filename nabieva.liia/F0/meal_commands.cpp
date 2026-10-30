#include "meal_commands.hpp"
#include <cctype>
#include <cmath>
#include <iomanip>
#include <istream>
#include <ostream>
#include <stdexcept>
#include "application_commands.hpp"
#include "console.hpp"
#include "nutrition.hpp"

nabieva::CommandResult nabieva::recordMeal(std::istream& input, command_context_t& context,
    std::ostream& output)
{
  const FoodCatalog& catalog = context.catalog_;
  FoodDiary& diary = context.diary_;
  std::string name = "";
  double grams = 0.0;

  if (!(input >> name >> grams) || !std::isfinite(grams) || (grams <= 0.0)
      || (!input.eof() && !std::isspace(input.peek())))
  {
    throw std::invalid_argument("неверные параметры приёма пищи.");
  }

  std::string dateText = "";
  std::string extra = "";
  const bool usesToday = !(input >> dateText);
  const Date date = usesToday ? getCurrentDate() : parseDate(dateText);
  if (input >> extra)
  {
    throw std::invalid_argument("после даты указаны лишние аргументы.");
  }
  const FoodCatalog::const_iterator food = catalog.find(name);

  if (food == catalog.end())
  {
    output << "блюдо не найдено.\n";
    return CommandResult::unchanged;
  }

  const Meal meal(food->second, grams);
  const FoodDiary::iterator day = diary.find(date);
  nutrition_totals_t totals = {};
  if (day != diary.end())
  {
    totals = calculateNutrition(day->second);
  }
  if (!std::isfinite(totals.calories_ + meal.getCalories())
      || !std::isfinite(totals.protein_ + meal.getProtein())
      || !std::isfinite(totals.fat_ + meal.getFat())
      || !std::isfinite(totals.carbs_ + meal.getCarbs()))
  {
    throw std::invalid_argument("суммарные кбжу за день получились бы слишком большими.");
  }

  if (day == diary.end())
  {
    MealList newMeals;
    newMeals.pushFront(meal);
    diary.add(date, newMeals);
  }
  else if (day->second.isEmpty())
  {
    day->second.pushFront(meal);
  }
  else
  {
    MealList::iterator last = day->second.begin();
    MealList::iterator next = last;
    ++next;

    while (next != day->second.end())
    {
      last = next;
      ++next;
    }

    day->second.insertAfter(last, meal);
  }

  output << "приём пищи записан.\n";
  return CommandResult::changed;
}
nabieva::CommandResult nabieva::removeMeal(std::istream& input, command_context_t& context,
    std::ostream& output)
{
  FoodDiary& diary = context.diary_;
  std::size_t number = 0;

  input >> std::ws;
  if (!std::isdigit(input.peek()) || !(input >> number) || (number == 0)
      || (!input.eof() && !std::isspace(input.peek())))
  {
    throw std::invalid_argument("неверный номер записи о питании.");
  }

  std::string dateText = "";
  std::string extra = "";
  const bool usesToday = !(input >> dateText);
  const Date date = usesToday ? getCurrentDate() : parseDate(dateText);
  if (input >> extra)
  {
    throw std::invalid_argument("после даты указаны лишние аргументы.");
  }
  const FoodDiary::iterator day = diary.find(date);

  if ((day == diary.end()) || day->second.isEmpty())
  {
    output << "запись о приёме пищи не найдена.\n";
    return CommandResult::unchanged;
  }

  const std::size_t firstNumber = 1;
  std::size_t currentNumber = firstNumber;
  MealList::iterator current = day->second.begin();
  MealList::iterator previous = current;

  while ((current != day->second.end()) && (currentNumber < number))
  {
    previous = current;
    ++current;
    ++currentNumber;
  }

  if (current == day->second.end())
  {
    output << "запись о приёме пищи не найдена.\n";
    return CommandResult::unchanged;
  }

  if (number == firstNumber)
  {
    day->second.popFront();
  }
  else
  {
    day->second.eraseAfter(previous);
  }

  if (day->second.isEmpty())
  {
    diary.remove(date);
  }

  output << "запись о приёме пищи удалена.\n";
  return CommandResult::changed;
}

void nabieva::printMeals(const FoodDiary& diary, const Date& date, std::ostream& output)
{
  const FoodDiary::const_iterator day = diary.find(date);

  if ((day == diary.end()) || day->second.isEmpty())
  {
    output << "нет записей о питании за " << date.getText() << ".\n";
    return;
  }

  const std::size_t minimumNameWidth = getTextWidth("блюдо");
  const std::size_t columnGap = 2;
  std::size_t nameWidth = minimumNameWidth;
  const nutrition_totals_t totals = calculateNutrition(day->second);

  for (MealList::const_iterator current = day->second.cbegin(); current != day->second.cend();
      ++current)
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
  for (MealList::const_iterator current = day->second.cbegin(); current != day->second.cend();
      ++current)
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

nabieva::CommandResult nabieva::listMeals(std::istream& input, command_context_t& context,
    std::ostream& output)
{
  std::string dateText = "";
  std::string extra = "";
  const bool usesToday = !(input >> dateText);
  const Date date = usesToday ? getCurrentDate() : parseDate(dateText);
  if (input >> extra)
  {
    throw std::invalid_argument("после даты указаны лишние аргументы.");
  }
  printMeals(context.diary_, date, output);
  return CommandResult::unchanged;
}
