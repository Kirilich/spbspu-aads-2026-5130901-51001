#include "stats_commands.hpp"
#include <cmath>
#include <iomanip>
#include <istream>
#include <ostream>
#include <stdexcept>
#include "application_commands.hpp"
#include "console.hpp"
#include "nutrition.hpp"

namespace nabieva
{
  namespace detail
  {
    constexpr int firstCalendarValue = 1;
    constexpr int nextDayOffset = 1;
  }
}

void nabieva::printDayStats(const Profile& profile, const WeightHistory& history,
    const FoodDiary& diary, const Date& date, std::ostream& output)
{

  if (!profile.hasData())
  {
    output << "создайте профиль командой \"profile\".\n";
    return;
  }

  nutrition_totals_t totals = {0.0, 0.0, 0.0, 0.0};
  const FoodDiary::const_iterator day = diary.find(date);

  if (day != diary.end())
  {
    totals = calculateNutrition(day->second);
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

nabieva::CommandResult nabieva::showWeekStats(std::istream& input, command_context_t& context,
    std::ostream& output)
{
  const Profile& profile = context.profile_;
  const WeightHistory& history = context.history_;
  const FoodDiary& diary = context.diary_;
  std::string dateText = "";
  std::string extra = "";
  const bool usesToday = !(input >> dateText);
  const Date endDate = usesToday ? getCurrentDate() : parseDate(dateText);
  if (input >> extra)
  {
    throw std::invalid_argument("после даты указаны лишние аргументы.");
  }

  if (!profile.hasData())
  {
    output << "создайте профиль командой \"profile\".\n";
    return CommandResult::unchanged;
  }

  const int dayCount = 7;
  const Date firstSupportedDate(detail::firstCalendarValue, detail::firstCalendarValue,
      detail::firstCalendarValue);
  Date firstDate = endDate;

  for (int index = detail::nextDayOffset; index < dayCount; ++index)
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
    const FoodDiary::const_iterator day = diary.find(currentDate);

    if (day != diary.end())
    {
      totals[index] = calculateNutrition(day->second);
    }

    weights[index] = getWeightForDate(profile, history, currentDate);

    if (index + detail::nextDayOffset < dayCount)
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

    if (index + detail::nextDayOffset < dayCount)
    {
      currentDate = currentDate.getNextDay();
    }
  }

  output.flags(originalFlags);
  output << "используется последний вес на дату; до первой записи берётся вес из профиля.\n";
  return CommandResult::unchanged;
}

nabieva::CommandResult nabieva::showDayStats(std::istream& input, command_context_t& context,
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
  printDayStats(context.profile_, context.history_, context.diary_, date, output);
  return CommandResult::unchanged;
}
