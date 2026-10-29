#ifndef FOOD_CATALOG_HPP
#define FOOD_CATALOG_HPP

#include <string>
#include "food.hpp"
#include "hash_table.hpp"

namespace nabieva
{
  using FoodCatalog = HashTable< std::string, Food >;

  List< std::string > getSortedFoodNames(const FoodCatalog& catalog);
}

#endif
