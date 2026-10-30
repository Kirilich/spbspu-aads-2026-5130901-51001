#ifndef WEIGHT_COMMANDS_HPP
#define WEIGHT_COMMANDS_HPP

#include <iosfwd>
#include "bstree.hpp"
#include "date.hpp"

namespace nabieva
{
  class Profile;
  enum class CommandResult;
  struct command_context_t;

  using WeightHistory = BSTree< Date, double >;

  CommandResult logWeight(std::istream& input, command_context_t& context, std::ostream& output);
  CommandResult removeWeight(std::istream& input, command_context_t& context, std::ostream& output);
  CommandResult listWeights(std::istream& input, command_context_t& context, std::ostream& output);
  CommandResult drawWeightGraph(std::istream& input, command_context_t& context,
      std::ostream& output);

  double getCurrentWeight(const Profile& profile, const WeightHistory& history);
  double getWeightForDate(const Profile& profile, const WeightHistory& history, const Date& date);
}

#endif
