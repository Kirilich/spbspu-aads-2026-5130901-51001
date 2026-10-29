#include "date.hpp"
#include <ctime>
#include <stdexcept>

namespace nabieva
{
  static int getMonthDays(int month, int year)
  {
    const int firstMonth = 1;
    const int monthCount = 12;
    const int daysInMonths[monthCount] = {
      31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
    };

    int daysInMonth = daysInMonths[month - firstMonth];
    const int leapYearPeriod = 4;
    const int centuryPeriod = 100;
    const int leapCenturyPeriod = 400;
    const bool leapYear = ((year % leapYearPeriod == 0)
      && (year % centuryPeriod != 0)) || (year % leapCenturyPeriod == 0);
    const int february = 2;

    if ((month == february) && leapYear)
    {
      ++daysInMonth;
    }

    return daysInMonth;
  }
}

nabieva::Date::Date(int day, int month, int year) :
  day_(day),
  month_(month),
  year_(year)
{
  const int firstYear = 1;
  const int lastYear = 9999;
  const int firstMonth = 1;
  const int monthCount = 12;
  const int firstDay = 1;

  if ((year_ < firstYear) || (year_ > lastYear)
    || (month_ < firstMonth) || (month_ > monthCount))
  {
    throw std::invalid_argument("несуществующая дата.");
  }

  const int daysInMonth = getMonthDays(month_, year_);

  if ((day_ < firstDay) || (day_ > daysInMonth))
  {
    throw std::invalid_argument("несуществующая дата.");
  }
}

bool nabieva::Date::operator==(const Date& other) const
{
  return (day_ == other.day_) && (month_ == other.month_) && (year_ == other.year_);
}

bool nabieva::Date::operator!=(const Date& other) const
{
  return !(*this == other);
}

bool nabieva::Date::operator<(const Date& other) const
{
  if (year_ != other.year_)
  {
    return year_ < other.year_;
  }

  if (month_ != other.month_)
  {
    return month_ < other.month_;
  }

  return day_ < other.day_;
}

bool nabieva::Date::operator>(const Date& other) const
{
  return other < *this;
}

bool nabieva::Date::operator<=(const Date& other) const
{
  return (*this < other) || (*this == other);
}

bool nabieva::Date::operator>=(const Date& other) const
{
  return (*this > other) || (*this == other);
}

int nabieva::Date::getDay() const
{
  return day_;
}

int nabieva::Date::getMonth() const
{
  return month_;
}

int nabieva::Date::getYear() const
{
  return year_;
}

nabieva::Date nabieva::Date::getPreviousDay() const
{
  const int firstDay = 1;
  const int firstMonth = 1;
  const int lastMonth = 12;
  const int firstYear = 1;

  if (day_ > firstDay)
  {
    return Date(day_ - 1, month_, year_);
  }

  if (month_ > firstMonth)
  {
    const int previousMonth = month_ - 1;
    return Date(getMonthDays(previousMonth, year_), previousMonth, year_);
  }

  if (year_ == firstYear)
  {
    throw std::out_of_range("нельзя перейти к дате раньше 01.01.0001.");
  }

  const int previousYear = year_ - 1;
  return Date(getMonthDays(lastMonth, previousYear), lastMonth, previousYear);
}

nabieva::Date nabieva::Date::getNextDay() const
{
  const int firstDay = 1;
  const int firstMonth = 1;
  const int lastMonth = 12;
  const int lastYear = 9999;

  if (day_ < getMonthDays(month_, year_))
  {
    return Date(day_ + 1, month_, year_);
  }

  if (month_ < lastMonth)
  {
    return Date(firstDay, month_ + 1, year_);
  }

  if (year_ == lastYear)
  {
    throw std::out_of_range("нельзя перейти к дате позже 31.12.9999.");
  }

  return Date(firstDay, firstMonth, year_ + 1);
}

std::string nabieva::Date::getText() const
{
  const std::size_t dayWidth = 2;
  const std::size_t monthWidth = 2;
  const std::size_t yearWidth = 4;
  std::string dayText = std::to_string(day_);
  std::string monthText = std::to_string(month_);
  std::string yearText = std::to_string(year_);

  while (dayText.size() < dayWidth)
  {
    dayText = "0" + dayText;
  }

  while (monthText.size() < monthWidth)
  {
    monthText = "0" + monthText;
  }

  while (yearText.size() < yearWidth)
  {
    yearText = "0" + yearText;
  }

  return dayText + "." + monthText + "." + yearText;
}

nabieva::Date nabieva::parseDate(const std::string& text)
{
  const std::size_t dateLength = 10;
  const std::size_t firstPoint = 2;
  const std::size_t secondPoint = 5;

  if (text.size() != dateLength)
  {
    throw std::invalid_argument("неверный формат даты. используйте дд.мм.гггг.");
  }

  if ((text[firstPoint] != '.') || (text[secondPoint] != '.'))
  {
    throw std::invalid_argument("неверный формат даты. используйте дд.мм.гггг.");
  }

  for (std::size_t index = 0; index < text.size(); ++index)
  {
    if ((index == firstPoint) || (index == secondPoint))
    {
      continue;
    }

    if ((text[index] < '0') || (text[index] > '9'))
    {
      throw std::invalid_argument("неверный формат даты. используйте дд.мм.гггг.");
    }
  }

  const std::size_t monthStart = firstPoint + 1;
  const std::size_t monthLength = secondPoint - monthStart;
  const std::size_t yearStart = secondPoint + 1;
  const int day = std::stoi(text.substr(0, firstPoint));
  const int month = std::stoi(text.substr(monthStart, monthLength));
  const int year = std::stoi(text.substr(yearStart));

  return Date(day, month, year);
}

nabieva::Date nabieva::getCurrentDate()
{
  const std::time_t now = std::time(nullptr);
  const std::time_t timeError = static_cast<std::time_t>(-1);

  if (now == timeError)
  {
    throw std::runtime_error("не удалось получить текущее время.");
  }

  const std::tm* const localTime = std::localtime(&now);

  if (localTime == nullptr)
  {
    throw std::runtime_error("не удалось определить текущую дату.");
  }

  const int firstMonth = 1;
  const int yearOffset = 1900;
  const int day = localTime->tm_mday;
  const int month = localTime->tm_mon + firstMonth;
  const int year = localTime->tm_year + yearOffset;

  return Date(day, month, year);
}
