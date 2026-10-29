#include "input.hpp"
#include <limits>
#include <cmath>
#include <stdexcept>

bool nabieva::readToken(const std::string& line, std::size_t& position, std::string& token)
{
  const std::string separators = " \t\r\n\v\f";
  const std::size_t start = line.find_first_not_of(separators, position);

  if (start == std::string::npos)
  {
    position = line.size();
    token.clear();
    return false;
  }

  const std::size_t end = line.find_first_of(separators, start);

  if (end == std::string::npos)
  {
    token = line.substr(start);
    position = line.size();
  }
  else
  {
    token = line.substr(start, end - start);
    position = end;
  }

  return true;
}

bool nabieva::readNumber(const std::string& token, double& value)
{
  try
  {
    std::size_t processed = 0;
    const double number = std::stod(token, &processed);

    if ((processed != token.size()) || !std::isfinite(number))
    {
      return false;
    }

    value = number;
    return true;
  }
  catch (const std::invalid_argument&)
  {
    return false;
  }
  catch (const std::out_of_range&)
  {
    return false;
  }
}
nabieva::Date nabieva::readOptionalDate(const std::string& line, std::size_t position)
{
  std::string text = "";

  if (!readToken(line, position, text))
  {
    return getCurrentDate();
  }

  std::string extra = "";
  if (readToken(line, position, extra))
  {
    throw std::invalid_argument("после даты указаны лишние аргументы.");
  }

  return parseDate(text);
}
bool nabieva::readPositiveInteger(const std::string& token, std::size_t& value)
{
  if (token.empty())
  {
    return false;
  }

  const std::size_t base = 10;
  constexpr std::size_t maximum = std::numeric_limits< std::size_t >::max();
  std::size_t number = 0;

  for (std::size_t index = 0; index < token.size(); ++index)
  {
    const char symbol = token[index];

    if ((symbol < '0') || (symbol > '9'))
    {
      return false;
    }

    const std::size_t digit = static_cast<std::size_t>(symbol - '0');

    if (number > (maximum - digit) / base)
    {
      return false;
    }

    number = number * base + digit;
  }

  if (number == 0)
  {
    return false;
  }

  value = number;
  return true;
}
