#include "plan_commands.hpp"
#include "console.hpp"
#include <iomanip>
#include <ostream>
#include <stdexcept>
#include "input.hpp"

namespace nabieva
{
  namespace detail
  {
    struct planned_day_t
    {
      Date date_;
      daily_menu_t plan_;
    };
  }

  static double getPlanFactor(const std::string& mode)
  {
    if (mode == "mild")
    {
      return 0.90;
    }

    if (mode == "normal")
    {
      return 0.80;
    }

    if (mode == "aggressive")
    {
      return 0.70;
    }

    throw std::invalid_argument("неизвестный режим составления плана.");
  }
  static void printPlannedDay(
    const detail::planned_day_t& day,
    const nutrition_totals_t& goals,
    std::ostream& output)
  {
    output << "\n" << day.date_.getText() << '\n';

    const char* titles[3] = { "завтрак", "обед", "ужин" };

    for (std::size_t index = 0; index < 3; ++index)
    {
      const meal_plan_t& part = day.plan_.parts_[index];

      output << "\n" << titles[index] << ":\n";

      std::size_t number = 0;

      for (MealList::const_iterator current = part.meals_.cbegin();
        current != part.meals_.cend(); ++current)
      {
        ++number;

        output << number << ". " << current->getFood().getName();
        output << " | " << current->getGrams() << " г";
        output << " | " << current->getCalories() << " ккал\n";
      }

      if (part.meals_.isEmpty())
      {
        output << "подходящие порции не найдены.\n";
      }

      output << "итого за приём пищи: " << part.totals_.calories_ << " ккал";
      output << " | б:" << part.totals_.protein_ << " г";
      output << " ж:" << part.totals_.fat_ << " г";
      output << " у:" << part.totals_.carbs_ << " г\n";

      if (!part.hasMainDish_)
      {
        output << "для этого приёма пищи не подобрано второе блюдо.\n";
      }

      if (part.balanceScore_ > 0.0)
      {
        output << "этот приём пищи соответствует не всем целевым диапазонам.\n";
      }

      if (part.repeatsYesterday_)
      {
        output << "некоторые супы или вторые блюда повторяют предыдущий день.\n";
      }
      if (part.repeatsToday_)
      {
        output << "некоторые супы или вторые блюда уже выбраны на этот день.\n";
      }
      if (part.portionSearchLimited_)
      {
        output << "подбор порций остановлен по ограничению числа шагов.\n";
      }
    }

    const double values[4] = {
      day.plan_.totals_.calories_,
      day.plan_.totals_.protein_,
      day.plan_.totals_.fat_,
      day.plan_.totals_.carbs_
    };

    const double targets[4] = {
      goals.calories_,
      goals.protein_,
      goals.fat_,
      goals.carbs_
    };

    const char* names[4] = {
      "калории, ккал", "белки, г", "жиры, г", "углеводы, г"
    };

    output << "\nитого за день:\n";

    printText(output, "показатель", 15);
    printText(output, "в плане", 14, false);
    printText(output, "цель", 14, false);
    output << "  статус\n";

    for (std::size_t index = 0; index < 4; ++index)
    {
      printText(output, names[index], 15);
      output << std::right << std::setw(14) << values[index];
      output << std::setw(14) << targets[index];

      if (getDeviation(values[index], targets[index]) == 0.0)
      {
        output << "  в норме\n";
      }
      else
      {
        output << ((values[index] < targets[index])
          ? "  недостаток\n" : "  избыток\n");
      }
    }

    if (day.plan_.balanceScore_ > 0.0)
    {
      output << "некоторые дневные цели не достигнуты. проверьте показатели.\n";
    }
    else
    {
      output << "дневные калории и бжу находятся в целевых диапазонах.\n";
    }
  }
}
void nabieva::generatePlan(
  const Profile& profile,
  const FoodCatalog& catalog,
  const FoodDiary& diary,
  const std::string& line,
  std::size_t position,
  std::ostream& output)
{
  const std::size_t argumentCount = 3;
  std::string arguments[argumentCount] = {};

  for (std::size_t index = 0; index < argumentCount; ++index)
  {
    if (!readToken(line, position, arguments[index]))
    {
      throw std::invalid_argument("не хватает параметров плана.");
    }
  }

  std::string extra = "";
  std::size_t days = 0;
  const std::size_t maximumDays = 31;

  if (readToken(line, position, extra)
    || !readPositiveInteger(arguments[1], days)
    || (days > maximumDays))
  {
    throw std::invalid_argument("неверные параметры плана.");
  }

  const Date firstDay = parseDate(arguments[0]);
  const double factor = getPlanFactor(arguments[2]);

  Date lastDay = firstDay;

  for (std::size_t index = 1; index < days; ++index)
  {
    lastDay = lastDay.getNextDay();
  }

  if (!profile.hasData())
  {
    output << "создайте профиль командой \"profile\".\n";
    return;
  }

  if (catalog.isEmpty())
  {
    output << "каталог блюд пуст.\n";
    return;
  }

  const nutrition_totals_t goals = {
    profile.getDailyCaloriesGoal() * factor,
    profile.getDailyProteinGoal() * factor,
    profile.getDailyFatGoal() * factor,
    profile.getDailyCarbsGoal() * factor
  };

  const Date minimumDate(1, 1, 1);
  const MealList* previousMeals = nullptr;

  if (firstDay > minimumDate)
  {
    previousMeals = diary.find(firstDay.getPreviousDay());
  }

  List< detail::planned_day_t > plans;
  List< detail::planned_day_t >::iterator last = plans.end();

  Date date = firstDay;

  for (std::size_t index = 0; index < days; ++index)
  {
    const detail::planned_day_t entry = {
      date,
      buildDailyMenu(catalog, goals, previousMeals)
    };

    if (plans.isEmpty())
    {
      plans.pushFront(entry);
      last = plans.begin();
    }
    else
    {
      last = plans.insertAfter(last, entry);
    }

    previousMeals = &last->plan_.meals_;

    if (index + 1 < days)
    {
      date = date.getNextDay();
    }
  }

  const std::ios::fmtflags originalFlags = output.flags();
  const std::streamsize originalPrecision = output.precision();

  output << std::fixed << std::setprecision(0);

  output << "план: " << firstDay.getText();
  output << " - " << lastDay.getText() << '\n';
  output << "режим: " << getPlanModeText(arguments[2]) << '\n';

  output << "дневные цели: " << goals.calories_ << " ккал";
  output << " | б:" << goals.protein_ << " г";
  output << " ж:" << goals.fat_ << " г";
  output << " у:" << goals.carbs_ << " г\n";
  output << "доли целей: завтрак 30%, обед 40%, ужин 30%.\n";
  output << "допустимое отклонение: +/-10%; "
    << "расчёты выполняются без округления.\n";

  for (List< detail::planned_day_t >::const_iterator current = plans.cbegin();
    current != plans.cend(); ++current)
  {
    printPlannedDay(*current, goals, output);
  }

  output << "для записи выбранных порций используйте команду eat.\n";

  output.flags(originalFlags);
  output.precision(originalPrecision);
}
