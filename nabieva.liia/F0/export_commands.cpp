#include "export_commands.hpp"
#include <fstream>
#include <ostream>
#include <stdexcept>
#include "input.hpp"
#include "nutrition.hpp"
#include "stats_commands.hpp"

void nabieva::exportReport(const Profile& profile, const WeightHistory& history,
  const FoodDiary& diary, const std::string& line, std::size_t position, std::ostream& output)
{
  std::string firstText = "";
  std::string lastText = "";
  std::string extra = "";

  if (!readToken(line, position, firstText) || !readToken(line, position, lastText)
    || readToken(line, position, extra))
  {
    throw std::invalid_argument("для экспорта нужны две даты.");
  }

  const Date firstDate = parseDate(firstText);
  const Date lastDate = parseDate(lastText);

  if (lastDate < firstDate)
  {
    throw std::invalid_argument("конечная дата не может быть раньше начальной.");
  }

  if (!profile.hasData())
  {
    output << "создайте профиль командой \"profile\".\n";
    return;
  }
  Date currentDate = firstDate;

  while (true)
  {
    const MealList* const meals = diary.find(currentDate);

    if (meals != nullptr)
    {
      calculateNutrition(*meals);
    }

    if (currentDate == lastDate)
    {
      break;
    }

    currentDate = currentDate.getNextDay();
  }
  const std::string fileName = "report_" + firstDate.getText() + "_" + lastDate.getText() + ".txt";
  std::ofstream file(fileName);

  if (!file)
  {
    throw std::invalid_argument("не удалось создать файл отчёта.");
  }

  file << "отчёт трекера питания\n";
  file << "период: " << firstDate.getText() << " - " << lastDate.getText() << "\n\n";
  profile.printProfile(file, getCurrentWeight(profile, history));

  const std::size_t separatorWidth = 60;
  currentDate = firstDate;
  while (true)
  {
    file << '\n' << std::string(separatorWidth, '-') << '\n';

    const std::string dateText = currentDate.getText();
    showDayStats(profile, history, diary, dateText, 0, file);

    const double* const recordedWeight = history.find(currentDate);

    if (recordedWeight != nullptr)
    {
      file << "вес, записанный на эту дату: " << *recordedWeight << " кг\n";
    }
    else
    {
      file << "записи веса на эту дату нет.\n";
    }

    file << '\n';
    listMeals(diary, dateText, 0, file);

    if (!file)
    {
      throw std::invalid_argument("не удалось записать отчёт.");
    }

    if (currentDate == lastDate)
    {
      break;
    }

    currentDate = currentDate.getNextDay();
  }

  file.close();

  if (!file)
  {
    throw std::invalid_argument("не удалось завершить запись отчёта.");
  }

  output << "отчёт сохранён в " << fileName << '\n';
}
