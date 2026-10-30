#ifndef NUTRITION_CONSTANTS_HPP
#define NUTRITION_CONSTANTS_HPP

#include <cstddef>

namespace nabieva
{
  namespace detail
  {
    constexpr std::size_t nutritionValueCount = 4;
    constexpr std::size_t macroCount = 3;
    constexpr std::size_t proteinIndex = 1;
    constexpr std::size_t fatIndex = 2;
    constexpr std::size_t carbsIndex = 3;
    constexpr std::size_t mealsPerDay = 3;
    constexpr std::size_t lunchIndex = 1;
    constexpr double referenceGrams = 100.0;
  }
}

#endif
