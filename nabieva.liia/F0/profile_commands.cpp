#include "profile_commands.hpp"
#include <stdexcept>
#include "input.hpp"
#include "profile.hpp"

void nabieva::setProfileFromCommand(Profile& profile, const std::string& line, std::size_t position)
{
  const std::size_t requiredCount = 7;
  const std::size_t maximumCount = 11;

  std::string arguments[maximumCount] = {};
  std::size_t count = 0;

  while ((count < maximumCount)
    && nabieva::readToken(line, position, arguments[count]))
  {
    ++count;
  }

  std::string extra = "";

  bool valid = (count >= requiredCount)
    && !nabieva::readToken(line, position, extra);

  nabieva::profile_data_t data = {};

  if (valid)
  {
    data.name_ = arguments[0];
    data.sex_ = arguments[3];

    valid = nabieva::readNumber(arguments[1], data.currentWeight_)
      && nabieva::readNumber(arguments[2], data.targetWeight_)
      && nabieva::readPositiveInteger(arguments[4], data.age_)
      && nabieva::readNumber(arguments[5], data.height_)
      && nabieva::readNumber(arguments[6], data.activity_);
  }

  double goals[4] = {};

  for (std::size_t index = 0; valid && (index < 4); ++index)
  {
    const std::size_t argument = requiredCount + index;

    if ((argument < count) && (arguments[argument] != "auto"))
    {
      valid = nabieva::readNumber(arguments[argument], goals[index])
        && (goals[index] > 0.0);
    }
  }

  if (valid)
  {
    data.calories_ = goals[0];
    data.protein_ = goals[1];
    data.fat_ = goals[2];
    data.carbs_ = goals[3];

    valid = profile.setProfile(data);
  }

  if (!valid)
  {
    throw std::invalid_argument("неверные параметры профиля.");
  }
}
