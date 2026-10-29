#include "stats_commands.hpp"
#include "console.hpp"
#include <cmath>
#include <iomanip>
#include <ostream>
#include "input.hpp"
#include "nutrition.hpp"
#include <stdexcept>

void nabieva::showDayStats(const Profile& profile, const WeightHistory& history,
  const FoodDiary& diary, const std::string& line, std::size_t position, std::ostream& output)
{
  const Date date = readOptionalDate(line, position);

  if (!profile.hasData())
  {
    output << "создайте профиль командой \"profile\".\n";
    return;
  }

  nutrition_totals_t totals = { 0.0, 0.0, 0.0, 0.0};
  const MealList* const meals = diary.find(date);

  if (meals != nullptr)
  {
    totals = calculateNutrition(*meals);
  }

  const double goal = profile.getDailyCaloriesGoal();
  const double remaining = goal - totals.calories_;
  const double weight = getWeightForDate(profile, history, date);
  const double target = profile.getTargetWeight();
  const int labelWidth = 22;
  const std::ios::fmtflags originalFlags = output.flags();

  output << "статистика за " << date.getText() << ":\n";
  printText(output, "съедено калорий:", labelWidth);
  output << totals.calories_ << " ккал\n";
  printText(output, "суточная цель:", labelWidth);
  output << goal << " ккал\n";

  if (remaining >= 0.0)
  {
    printText(output, "осталось:", labelWidth);
    output << remaining << " ккал\n";
  }
  else
  {
    printText(output, "превышение:", labelWidth);
    output << -remaining << " ккал\n";
  }

  printText(output, "белки:", labelWidth);
  output << totals.protein_ << " г\n";
  printText(output, "жиры:", labelWidth);
  output << totals.fat_ << " г\n";
  printText(output, "углеводы:", labelWidth);
  output << totals.carbs_ << " г\n";

  printText(output, "вес на дату:", labelWidth);
  output << weight << " кг\n";
  printText(output, "желаемый вес:", labelWidth);
  output << target << " кг\n";
  printText(output, "до желаемого веса:", labelWidth);
  output << std::abs(weight - target) << " кг\n";
  output.flags(originalFlags);
}

void nabieva::showWeekStats(const Profile& profile, const WeightHistory& history,
  const FoodDiary& diary, const std::string& line, std::size_t position, std::ostream& output)
{
  const Date endDate = readOptionalDate(line, position);

  if (!profile.hasData())
  {
    output << "создайте профиль командой \"profile\".\n";
    return;
  }

  const int dayCount = 7;
  const Date firstSupportedDate(1, 1, 1);
  Date firstDate = endDate;

  for (int index = 1; index < dayCount; ++index)
  {
    if (firstDate == firstSupportedDate)
    {
      throw std::invalid_argument("начало недели выходит за допустимый диапазон дат.");
    }

    firstDate = firstDate.getPreviousDay();
  }

  nutrition_totals_t totals[dayCount] = {};
  double weights[dayCount] = {};
  Date currentDate = firstDate;

  for (int index = 0; index < dayCount; ++index)
  {
    const MealList* const meals = diary.find(currentDate);

    if (meals != nullptr)
    {
      totals[index] = calculateNutrition(*meals);
    }

    weights[index] = getWeightForDate(profile, history, currentDate);

    if (index + 1 < dayCount)
    {
      currentDate = currentDate.getNextDay();
    }
  }
  const int dateWidth = 12;
  const int numericWidth = 14;
  const std::ios::fmtflags originalFlags = output.flags();

  output << "статистика за 7 дней: " << firstDate.getText() << " - " << endDate.getText() << '\n';
  printText(output, "дата", dateWidth);
  printText(output, "съедено, ккал", numericWidth, false);
  printText(output, "белки, г", numericWidth, false);
  printText(output, "жиры, г", numericWidth, false);
  printText(output, "углеводы, г", numericWidth, false);
  printText(output, "вес, кг", numericWidth, false);
  output << '\n';

  currentDate = firstDate;

  for (int index = 0; index < dayCount; ++index)
  {
    output << std::left << std::setw(dateWidth) << currentDate.getText();
    output << std::right << std::setw(numericWidth) << totals[index].calories_;
    output << std::setw(numericWidth) << totals[index].protein_;
    output << std::setw(numericWidth) << totals[index].fat_;
    output << std::setw(numericWidth) << totals[index].carbs_;
    output << std::setw(numericWidth) << weights[index] << '\n';

    if (index + 1 < dayCount)
    {
      currentDate = currentDate.getNextDay();
    }
  }

  output.flags(originalFlags);
  output << "используется последний вес на дату; до первой записи берётся вес из профиля.\n";
}
