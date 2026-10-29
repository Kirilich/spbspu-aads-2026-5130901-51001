#include "profile.hpp"
#include <cmath>
#include <utility>
#include <ostream>
#include <iomanip>

nabieva::Profile::Profile() :
  data_{},
  hasData_(false),
  automaticGoals_{ false, false, false, false }
{
}
bool nabieva::Profile::setProfile(const profile_data_t& data)
{
  if (data.name_.empty() || !std::isfinite(data.currentWeight_)
    || !std::isfinite(data.targetWeight_) || !std::isfinite(data.height_)
    || !std::isfinite(data.activity_) || (data.currentWeight_ <= 0.0)
    || (data.targetWeight_ <= 0.0) || (data.height_ <= 0.0)
    || (data.age_ < 19) || (data.activity_ < 1.4) || (data.activity_ > 2.4)
    || ((data.sex_ != "male") && (data.sex_ != "female")))
  {
    return false;
  }

  double goals[4] = {
    data.calories_,
    data.protein_,
    data.fat_,
    data.carbs_
  };

  for (std::size_t index = 0; index < 4; ++index)
  {
    if (!std::isfinite(goals[index]) || (goals[index] < 0.0))
    {
      return false;
    }
  }

  if (goals[0] == 0.0)
  {
    if (data.age_ > 78)
    {
      return false;
    }

    const double sexTerm = (data.sex_ == "male") ? 5.0 : -161.0;

    const double resting = 10.0 * data.currentWeight_
      + 6.25 * data.height_
      - 5.0 * static_cast<double>(data.age_)
      + sexTerm;

    goals[0] = resting * data.activity_;
  }

  if (!std::isfinite(goals[0]) || (goals[0] <= 0.0))
  {
    return false;
  }

  const double shares[3] = { 0.20, 0.30, 0.50 };
  const double energyPerGram[3] = { 4.0, 9.0, 4.0 };

  double fixedEnergy = 0.0;
  double missingShare = 0.0;

  for (std::size_t index = 0; index < 3; ++index)
  {
    if (goals[index + 1] == 0.0)
    {
      missingShare += shares[index];
    }
    else
    {
      fixedEnergy += goals[index + 1] * energyPerGram[index];
    }
  }

  if (!std::isfinite(fixedEnergy))
  {
    return false;
  }

  if (missingShare > 0.0)
  {
    const double remainingEnergy = goals[0] - fixedEnergy;

    if (remainingEnergy <= 0.0)
    {
      return false;
    }

    for (std::size_t index = 0; index < 3; ++index)
    {
      if (goals[index + 1] == 0.0)
      {
        goals[index + 1] = remainingEnergy
          * (shares[index] / missingShare)
          / energyPerGram[index];
      }
    }
  }

  for (std::size_t index = 0; index < 4; ++index)
  {
    if (!std::isfinite(goals[index]) || (goals[index] <= 0.0))
    {
      return false;
    }
  }

  profile_data_t prepared = data;

  prepared.calories_ = goals[0];
  prepared.protein_ = goals[1];
  prepared.fat_ = goals[2];
  prepared.carbs_ = goals[3];

  data_ = prepared;

  automaticGoals_[0] = (data.calories_ == 0.0);
  automaticGoals_[1] = (data.protein_ == 0.0);
  automaticGoals_[2] = (data.fat_ == 0.0);
  automaticGoals_[3] = (data.carbs_ == 0.0);

  hasData_ = true;
  return true;
}
void nabieva::Profile::printProfile(std::ostream& output) const
{
  printProfile(output, data_.currentWeight_);
}

void nabieva::Profile::printProfile(
  std::ostream& output, double currentWeight) const
{
  if (!hasData_)
  {
    output << "профиль ещё не создан!\n";
    return;
  }

  output << "имя: " << data_.name_ << '\n';
  output << "текущий вес: " << currentWeight << " кг\n";
  output << "желаемый вес: " << data_.targetWeight_ << " кг\n";
  output << "до цели: "
    << std::abs(currentWeight - data_.targetWeight_) << " кг\n";

  output << "пол: " << ((data_.sex_ == "male") ? "мужской" : "женский") << '\n';
  output << "возраст: " << data_.age_ << '\n';
  output << "рост: " << data_.height_ << " см\n";
  output << "коэффициент активности: " << data_.activity_ << '\n';

  const char* names[4] = {
    "калории в день: ",
    "белки в день: ",
    "жиры в день: ",
    "углеводы в день: "
  };

  const double values[4] = {
    data_.calories_,
    data_.protein_,
    data_.fat_,
    data_.carbs_
  };
  const std::ios::fmtflags originalFlags = output.flags();
  const std::streamsize originalPrecision = output.precision();

  for (std::size_t index = 0; index < 4; ++index)
  {
    if (index > 0)
    {
      output << std::fixed << std::setprecision(0);
    }

    output << names[index] << values[index];
    output << ((index == 0) ? " ккал" : " г");
    output << (automaticGoals_[index] ? " (авто)\n" : " (вручную)\n");
  }

  output.flags(originalFlags);
  output.precision(originalPrecision);

  const double macroEnergy = data_.protein_ * 4.0
    + data_.fat_ * 9.0
    + data_.carbs_ * 4.0;

  if (std::abs(macroEnergy / data_.calories_ - 1.0) > 0.05)
  {
    output << "обратите внимание: калорийность заданных бжу отличается "
      << "от цели по калориям более чем на 5%.\n";
  }
}
bool nabieva::Profile::hasData() const
{
  return hasData_;
}

bool nabieva::Profile::hasMacroGoals() const
{
  return hasData_;
}

double nabieva::Profile::getCurrentWeight() const
{
  return data_.currentWeight_;
}

double nabieva::Profile::getTargetWeight() const
{
  return data_.targetWeight_;
}

double nabieva::Profile::getDailyCaloriesGoal() const
{
  return data_.calories_;
}

double nabieva::Profile::getDailyProteinGoal() const
{
  return data_.protein_;
}

double nabieva::Profile::getDailyFatGoal() const
{
  return data_.fat_;
}

double nabieva::Profile::getDailyCarbsGoal() const
{
  return data_.carbs_;
}
nabieva::profile_data_t nabieva::Profile::getInputData() const
{
  profile_data_t input = data_;

  if (automaticGoals_[0])
  {
    input.calories_ = 0.0;
  }
  if (automaticGoals_[1])
  {
    input.protein_ = 0.0;
  }
  if (automaticGoals_[2])
  {
    input.fat_ = 0.0;
  }
  if (automaticGoals_[3])
  {
    input.carbs_ = 0.0;
  }

  return input;
}

void nabieva::Profile::swap(Profile& other)
{
  std::swap(data_, other.data_);
  std::swap(hasData_, other.hasData_);
  std::swap(automaticGoals_, other.automaticGoals_);
}
