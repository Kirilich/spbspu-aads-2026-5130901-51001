#include "food.hpp"
#include <cmath>
#include <stdexcept>

nabieva::Food::Food(
  const std::string& name,
  double calories,
  double protein,
  double fat,
  double carbs,
  const std::string& category) :
  name_(name),
  calories_(calories),
  protein_(protein),
  fat_(fat),
  carbs_(carbs),
  category_(category)
{
  validate();
}

const std::string& nabieva::Food::getName() const
{
  return name_;
}

const std::string& nabieva::Food::getCategory() const
{
  return category_;
}

double nabieva::Food::getCalories() const
{
  return calories_;
}

double nabieva::Food::getProtein() const
{
  return protein_;
}

double nabieva::Food::getFat() const
{
  return fat_;
}

double nabieva::Food::getCarbs() const
{
  return carbs_;
}

void nabieva::Food::validate() const
{
  const std::string separators = " \t\r\n\v\f";

  if (name_.empty()
    || (name_.find_first_of(separators) != std::string::npos))
  {
    throw std::invalid_argument("недопустимое название блюда.");
  }

  if ((category_ != "soup")
    && (category_ != "main")
    && (category_ != "side")
    && (category_ != "dessert")
    && (category_ != "drink"))
  {
    throw std::invalid_argument("неизвестная категория блюда.");
  }

  if (!std::isfinite(calories_)
    || !std::isfinite(protein_)
    || !std::isfinite(fat_)
    || !std::isfinite(carbs_))
  {
    throw std::invalid_argument("кбжу должны быть конечными числами.");
  }

  if ((calories_ < 0.0)
    || (protein_ < 0.0)
    || (fat_ < 0.0)
    || (carbs_ < 0.0))
  {
    throw std::invalid_argument("кбжу не могут быть отрицательными.");
  }
}
