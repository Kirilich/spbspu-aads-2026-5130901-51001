#include "random_choice.hpp"
#include <chrono>
#include <stdexcept>

std::mt19937& nabieva::getRandomGenerator()
{
  const std::chrono::high_resolution_clock::duration time =
    std::chrono::high_resolution_clock::now().time_since_epoch();
  static std::mt19937 generator(static_cast< std::mt19937::result_type >(time.count()));
  return generator;
}

std::size_t nabieva::getRandomIndex(std::size_t count)
{
  if (count == 0)
  {
    throw std::invalid_argument("нет вариантов для случайного выбора.");
  }
  std::uniform_int_distribution< std::size_t > distribution(0, count - 1);
  return distribution(getRandomGenerator());
}
