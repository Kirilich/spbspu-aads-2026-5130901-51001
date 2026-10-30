#include "export_commands.hpp"
#include <fstream>
#include <istream>
#include <ostream>
#include <stdexcept>
#include "application_commands.hpp"
#include "nutrition.hpp"
#include "stats_commands.hpp"

nabieva::CommandResult nabieva::exportReport(std::istream& input, command_context_t& context,
    std::ostream& output)
{
  const Profile& profile = context.profile_;
  const WeightHistory& history = context.history_;
  const FoodDiary& diary = context.diary_;
  std::string firstText = "";
  std::string lastText = "";
  std::string extra = "";

  if (!(input >> firstText) || !(input >> lastText) || (input >> extra))
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
    return CommandResult::unchanged;
  }
  Date currentDate = firstDate;

  while (true)
  {
    const FoodDiary::const_iterator day = diary.find(currentDate);

    if (day != diary.end())
    {
      calculateNutrition(day->second);
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

    printDayStats(profile, history, diary, currentDate, file);

    const WeightHistory::const_iterator recordedWeight = history.find(currentDate);

    if (recordedWeight != history.end())
    {
      file << "вес, записанный на эту дату: " << recordedWeight->second << " кг\n";
    }
    else
    {
      file << "записи веса на эту дату нет.\n";
    }

    file << '\n';
    printMeals(diary, currentDate, file);

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
  return CommandResult::unchanged;
}
