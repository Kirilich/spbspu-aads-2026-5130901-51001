#ifndef RANDOM_CHOICE_HPP
#define RANDOM_CHOICE_HPP

#include <cstddef>
#include <random>

namespace nabieva
{
  std::mt19937& getRandomGenerator();
  std::size_t getRandomIndex(std::size_t count);
}

#endif
