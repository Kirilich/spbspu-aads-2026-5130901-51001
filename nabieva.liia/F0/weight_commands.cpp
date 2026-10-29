#include "weight_commands.hpp"
#include <cmath>
#include <ostream>
#include <stdexcept>
#include <iomanip>
#include "input.hpp"

namespace nabieva
{
  static int getGraphRow(double weight, double maximum, double range, int intervals)
  {
    if (range == 0.0)
    {
      return 0;
    }

    const double position = (maximum - weight) / range;
    return static_cast<int>(std::round(position * intervals));
  }
}

double nabieva::getCurrentWeight(const Profile& profile, const WeightHistory& history)
{
  return getWeightForDate(profile, history, getCurrentDate());
}
double nabieva::getWeightForDate(const Profile& profile, const WeightHistory& history, const Date& date)
{
  double weight = profile.getCurrentWeight();

  for (WeightHistory::const_iterator position = history.begin();
    position != history.end(); ++position)
  {
    if (date < position->first)
    {
      break;
    }

    weight = position->second;
  }

  return weight;
}
bool nabieva::logWeight(const Profile& profile, WeightHistory& history,
  const std::string& line, std::size_t position, std::ostream& output)
{
  std::string text = "";
  double weight = 0.0;

  if (!readToken(line, position, text) || !readNumber(text, weight) || (weight <= 0.0))
  {
    throw std::invalid_argument("вес должен быть положительным конечным числом.");
  }

  const std::string separators = " \t\r\n";
  const bool usesToday = line.find_first_not_of(separators, position) == std::string::npos;
  const Date date = readOptionalDate(line, position);

  if (!profile.hasData())
  {
    output << "создайте профиль командой \"profile\".\n";
    return false;
  }

  const double previousWeight = getCurrentWeight(profile, history);

  if (!history.add(date, weight))
  {
    if (usesToday)
    {
      output << "запись веса за сегодня уже существует.\n";
    }
    else
    {
      output << "запись веса уже существует за " << date.getText() << ".\n";
    }

    return false;
  }

  const double currentWeight = getCurrentWeight(profile, history);
  const double target = profile.getTargetWeight();
  const double improvement = std::abs(previousWeight - target) - std::abs(currentWeight - target);

  if (improvement > 0.0)
  {
    output << "отлично! вы стали на " << improvement << " кг ближе к цели!\n";
  }
  else
  {
    output << "вес записан.\n";
  }
  return true;
}
bool nabieva::removeWeight(WeightHistory& history, const std::string& line,
  std::size_t position, std::ostream& output)
{
  std::string text = "";
  std::string extra = "";

  if (!readToken(line, position, text) || readToken(line, position, extra))
  {
    throw std::invalid_argument("неверные параметры удаления записи веса.");
  }

  const Date date = parseDate(text);

  if (history.remove(date))
  {
    output << "запись веса удалена.\n";
    return true;
  }
  else
  {
    output << "нет записи веса за " << date.getText() << '\n';
    return false;
  }
}
void nabieva::listWeights(const WeightHistory& history, std::ostream& output)
{
  if (history.isEmpty())
  {
    output << "история веса пуста.\n";
    return;
  }

  output << "история веса:\n";

  for (WeightHistory::const_iterator position = history.begin();
    position != history.end(); ++position)
  {
    output << position->first.getText() << " | " << position->second << " кг\n";
  }
}

void nabieva::drawWeightGraph(const Profile& profile, const WeightHistory& history,
  std::ostream& output)
{
  if (!profile.hasData())
  {
    output << "создайте профиль командой \"profile\".\n";
    return;
  }

  if (history.isEmpty())
  {
    output << "история веса пуста.\n";
    return;
  }

  const double target = profile.getTargetWeight();
  double minimum = target;
  double maximum = target;

  for (WeightHistory::const_iterator position = history.begin();
    position != history.end(); ++position)
  {
    if (position->second < minimum)
    {
      minimum = position->second;
    }

    if (position->second > maximum)
    {
      maximum = position->second;
    }
  }

  const double range = maximum - minimum;
  const int intervals = 10;
  const int goalRow = getGraphRow(target, maximum, range, intervals);
  int lastRow = intervals;

  if (range == 0.0)
  {
    lastRow = 0;
  }

  const std::size_t recordsPerPage = 7;
  const std::size_t columnWidth = 12;
  const int labelWidth = 14;
  const int displayPrecision = 6;
  const std::ios::fmtflags originalFlags = output.flags();
  const std::streamsize originalPrecision = output.precision();

  output << std::defaultfloat << std::setprecision(displayPrecision);
  output << "график веса (кг):\n";
  output << "* — измеренный вес; - — желаемый вес\n";
  output << "точки округлены до ближайшего уровня графика.\n";

  WeightHistory::const_iterator pageBegin = history.begin();
  while (pageBegin != history.end())
  {
    WeightHistory::const_iterator pageEnd = pageBegin;
    std::size_t count = 0;

    while ((pageEnd != history.end()) && (count < recordsPerPage))
    {
      ++pageEnd;
      ++count;
    }

    for (int row = 0; row <= lastRow; ++row)
    {
      double level = maximum;

      if (range != 0.0)
      {
        level = maximum - range * (static_cast<double>(row) / intervals);
      }

      output << std::right << std::setw(labelWidth) << level << " |";

      for (WeightHistory::const_iterator position = pageBegin;
        position != pageEnd; ++position)
      {
        char background = ' ';

        if (row == goalRow)
        {
          background = '-';
        }

        std::string cell(columnWidth, background);
        const int weightRow = getGraphRow(position->second, maximum, range, intervals);

        if (row == weightRow)
        {
          cell[columnWidth / 2] = '*';
        }

        output << cell;
      }

      if (row == goalRow)
      {
        output << " <- цель: " << target << " кг";
      }

      output << '\n';
    }

    output << std::setw(labelWidth) << "" << "  ";

    for (WeightHistory::const_iterator position = pageBegin;
      position != pageEnd; ++position)
    {
      output << ' ' << std::left << std::setw(static_cast<int>(columnWidth - 1));
      output << position->first.getText();
    }

    output << "\n\n";
    pageBegin = pageEnd;
  }
  output.flags(originalFlags);
  output.precision(originalPrecision);

  output << "текущий вес: " << getCurrentWeight(profile, history) << " кг\n";
  output << "желаемый вес: " << target << " кг\n";
}
