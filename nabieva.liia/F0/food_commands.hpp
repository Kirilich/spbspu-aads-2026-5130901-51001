#ifndef FOOD_COMMANDS_HPP
#define FOOD_COMMANDS_HPP

#include <cstddef>
#include <iosfwd>
#include <string>
#include "food_catalog.hpp"

namespace nabieva
{
  bool addFood(FoodCatalog& catalog, const std::string& line,
    std::size_t position, std::ostream& output);
  bool removeFood(FoodCatalog& catalog, const std::string& line,
    std::size_t position, std::ostream& output);
  void listFood(const FoodCatalog& catalog, std::ostream& output);
  void saveFood(const FoodCatalog& catalog, const std::string& line,
    std::size_t position, std::ostream& output);
  bool loadFood(FoodCatalog& catalog, const std::string& line,
    std::size_t position, std::ostream& output);
}

#endif
