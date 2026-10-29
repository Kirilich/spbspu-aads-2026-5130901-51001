#ifndef DATA_STORAGE_HPP
#define DATA_STORAGE_HPP

#include <string>
#include "meal_commands.hpp"
#include "weight_commands.hpp"

namespace nabieva
{
  std::string getDataFilePath(const std::string& programPath);
  bool loadData(const std::string& filePath, Profile& profile,
    FoodCatalog& catalog, WeightHistory& history, FoodDiary& diary);
  void saveData(const std::string& filePath, const Profile& profile,
    const FoodCatalog& catalog, const WeightHistory& history, const FoodDiary& diary);
}

#endif
