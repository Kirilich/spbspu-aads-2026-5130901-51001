#include "profile_commands.hpp"
#include <cctype>
#include <cmath>
#include <istream>
#include <string>
#include <stdexcept>
#include "application_commands.hpp"
#include "profile.hpp"
#include "nutrition_constants.hpp"

nabieva::CommandResult nabieva::setProfileFromCommand(std::istream& input,
    command_context_t& context, std::ostream&)
{
  profile_data_t data = {};

  if (!(input >> data.name_ >> data.currentWeight_) || !std::isspace(input.peek())
      || !(input >> data.targetWeight_) || !std::isspace(input.peek()) || !(input >> data.sex_))
  {
    throw std::invalid_argument("неверное имя, вес или пол.");
  }

  input >> std::ws;
  if (!std::isdigit(input.peek()) || !(input >> data.age_) || !std::isspace(input.peek())
      || !(input >> data.height_) || !std::isspace(input.peek()) || !(input >> data.activity_)
      || (!input.eof() && !std::isspace(input.peek())))
  {
    throw std::invalid_argument("неверный возраст, рост или активность.");
  }

  double goals[detail::nutritionValueCount] = {};
  for (std::size_t index = 0; index < detail::nutritionValueCount; ++index)
  {
    std::string text = "";
    if (!(input >> text))
    {
      break;
    }
    if (text == "auto")
    {
      continue;
    }

    std::size_t processed = 0;
    goals[index] = std::stod(text, &processed);
    if ((processed != text.size()) || !std::isfinite(goals[index]) || (goals[index] <= 0.0))
    {
      throw std::invalid_argument("неверная цель питания.");
    }
  }

  std::string extra = "";
  if (input >> extra)
  {
    throw std::invalid_argument("лишние параметры профиля.");
  }

  data.calories_ = goals[0];
  data.protein_ = goals[detail::proteinIndex];
  data.fat_ = goals[detail::fatIndex];
  data.carbs_ = goals[detail::carbsIndex];
  if (!context.profile_.setProfile(data))
  {
    throw std::invalid_argument("неверные параметры профиля.");
  }
  return CommandResult::profileChanged;
}

nabieva::CommandResult nabieva::showProfile(std::istream& input, command_context_t& context,
    std::ostream& output)
{
  std::string extra = "";
  if (input >> extra)
  {
    throw std::invalid_argument("лишние параметры команды.");
  }

  context.profile_.printProfile(output, getCurrentWeight(context.profile_, context.history_));
  return CommandResult::unchanged;
}
