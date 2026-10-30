#include "profile.hpp"
#include <cmath>
#include <utility>
#include <ostream>
#include <iomanip>

namespace nabieva
{
  namespace detail
  {
    constexpr std::size_t minimumAge = 19;
    constexpr std::size_t maximumFormulaAge = 78;
    constexpr double minimumActivity = 1.4;
    constexpr double maximumActivity = 2.4;
    constexpr double weightCoefficient = 10.0;
    constexpr double heightCoefficient = 6.25;
    constexpr double ageCoefficient = 5.0;
    constexpr double proteinEnergyPerGram = 4.0;
    constexpr double fatEnergyPerGram = 9.0;
    constexpr double carbsEnergyPerGram = 4.0;
    constexpr double targetRatio = 1.0;
    constexpr double energyTolerance = 0.05;
  }
}

nabieva::Profile::Profile():
  data_{},
  hasData_(false),
  automaticGoals_{false, false, false, false}
{
}
bool nabieva::Profile::setProfile(const profile_data_t& data)
{
  if (data.name_.empty() || !std::isfinite(data.currentWeight_)
      || !std::isfinite(data.targetWeight_) || !std::isfinite(data.height_)
      || !std::isfinite(data.activity_) || (data.currentWeight_ <= 0.0)
      || (data.targetWeight_ <= 0.0) || (data.height_ <= 0.0) || (data.age_ < detail::minimumAge)
      || (data.activity_ < detail::minimumActivity) || (data.activity_ > detail::maximumActivity)
      || ((data.sex_ != "male") && (data.sex_ != "female")))
  {
    return false;
  }

  double goals[detail::nutritionValueCount] = {data.calories_, data.protein_, data.fat_,
      data.carbs_};

  for (std::size_t index = 0; index < detail::nutritionValueCount; ++index)
  {
    if (!std::isfinite(goals[index]) || (goals[index] < 0.0))
    {
      return false;
    }
  }

  if (goals[0] == 0.0)
  {
    if (data.age_ > detail::maximumFormulaAge)
    {
      return false;
    }

    const double sexTerm = (data.sex_ == "male") ? 5.0 : -161.0;

    const double resting = detail::weightCoefficient * data.currentWeight_
        + detail::heightCoefficient * data.height_
        - detail::ageCoefficient * static_cast< double >(data.age_) + sexTerm;

    goals[0] = resting * data.activity_;
  }

  if (!std::isfinite(goals[0]) || (goals[0] <= 0.0))
  {
    return false;
  }

  const double shares[detail::macroCount] = {0.20, 0.30, 0.50};
  const double energyPerGram[detail::macroCount] = {4.0, 9.0, 4.0};

  double fixedEnergy = 0.0;
  double missingShare = 0.0;

  for (std::size_t index = 0; index < detail::macroCount; ++index)
  {
    if (goals[index + detail::proteinIndex] == 0.0)
    {
      missingShare += shares[index];
    }
    else
    {
      fixedEnergy += goals[index + detail::proteinIndex] * energyPerGram[index];
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

    for (std::size_t index = 0; index < detail::macroCount; ++index)
    {
      if (goals[index + detail::proteinIndex] == 0.0)
      {
        goals[index + detail::proteinIndex] =
            remainingEnergy * (shares[index] / missingShare) / energyPerGram[index];
      }
    }
  }

  for (std::size_t index = 0; index < detail::nutritionValueCount; ++index)
  {
    if (!std::isfinite(goals[index]) || (goals[index] <= 0.0))
    {
      return false;
    }
  }

  profile_data_t prepared = data;

  prepared.calories_ = goals[0];
  prepared.protein_ = goals[detail::proteinIndex];
  prepared.fat_ = goals[detail::fatIndex];
  prepared.carbs_ = goals[detail::carbsIndex];

  data_ = prepared;

  automaticGoals_[0] = (data.calories_ == 0.0);
  automaticGoals_[detail::proteinIndex] = (data.protein_ == 0.0);
  automaticGoals_[detail::fatIndex] = (data.fat_ == 0.0);
  automaticGoals_[detail::carbsIndex] = (data.carbs_ == 0.0);

  hasData_ = true;
  return true;
}
void nabieva::Profile::printProfile(std::ostream& output) const
{
  printProfile(output, data_.currentWeight_);
}

void nabieva::Profile::printProfile(std::ostream& output, double currentWeight) const
{
  if (!hasData_)
  {
    output << "профиль ещё не создан!\n";
    return;
  }

  output << "имя: " << data_.name_ << '\n';
  output << "текущий вес: " << currentWeight << " кг\n";
  output << "желаемый вес: " << data_.targetWeight_ << " кг\n";
  output << "до цели: " << std::abs(currentWeight - data_.targetWeight_) << " кг\n";

  output << "пол: " << ((data_.sex_ == "male") ? "мужской" : "женский") << '\n';
  output << "возраст: " << data_.age_ << '\n';
  output << "рост: " << data_.height_ << " см\n";
  output << "коэффициент активности: " << data_.activity_ << '\n';

  const char* names[detail::nutritionValueCount] = {"калории в день: ", "белки в день: ",
      "жиры в день: ", "углеводы в день: "};

  const double values[detail::nutritionValueCount] = {data_.calories_, data_.protein_, data_.fat_,
      data_.carbs_};
  const std::ios::fmtflags originalFlags = output.flags();
  const std::streamsize originalPrecision = output.precision();

  for (std::size_t index = 0; index < detail::nutritionValueCount; ++index)
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

  const double macroEnergy = data_.protein_ * detail::proteinEnergyPerGram
      + data_.fat_ * detail::fatEnergyPerGram + data_.carbs_ * detail::carbsEnergyPerGram;

  if (std::abs(macroEnergy / data_.calories_ - detail::targetRatio) > detail::energyTolerance)
  {
    output << "обратите внимание: калорийность заданных бжу отличается "
           << "от цели по калориям более чем на 5%.\n";
  }
}
bool nabieva::Profile::hasData() const
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
  if (automaticGoals_[detail::proteinIndex])
  {
    input.protein_ = 0.0;
  }
  if (automaticGoals_[detail::fatIndex])
  {
    input.fat_ = 0.0;
  }
  if (automaticGoals_[detail::carbsIndex])
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
