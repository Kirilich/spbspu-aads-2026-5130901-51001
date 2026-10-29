#ifndef WEIGHT_COMMANDS_HPP
#define WEIGHT_COMMANDS_HPP

#include <cstddef>
#include <iosfwd>
#include <string>
#include "bstree.hpp"
#include "date.hpp"
#include "profile.hpp"

namespace nabieva
{
  using WeightHistory = BSTree< Date, double >;

  double getCurrentWeight(const Profile& profile, const WeightHistory& history);
  bool logWeight(const Profile& profile, WeightHistory& history,
    const std::string& line, std::size_t position, std::ostream& output);
  bool removeWeight(WeightHistory& history, const std::string& line,
    std::size_t position, std::ostream& output);
  void listWeights(const WeightHistory& history, std::ostream& output);
  void drawWeightGraph(const Profile& profile, const WeightHistory& history,
    std::ostream& output);
  double getWeightForDate(const Profile& profile, const WeightHistory& history, const Date& date);
}

#endif
