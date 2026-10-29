#ifndef PROFILE_HPP
#define PROFILE_HPP

#include <cstddef>
#include <iosfwd>
#include <string>

namespace nabieva
{
  struct profile_data_t
  {
    std::string name_;
    double currentWeight_;
    double targetWeight_;
    std::string sex_;
    std::size_t age_;
    double height_;
    double activity_;
    double calories_;
    double protein_;
    double fat_;
    double carbs_;
  };

  class Profile
  {
  public:
    Profile();

    profile_data_t getInputData() const;
    void swap(Profile& other);
    bool setProfile(const profile_data_t& data);
    void printProfile(std::ostream& output) const;
    void printProfile(std::ostream& output, double currentWeight) const;

    bool hasData() const;
    bool hasMacroGoals() const;

    double getCurrentWeight() const;
    double getTargetWeight() const;
    double getDailyCaloriesGoal() const;
    double getDailyProteinGoal() const;
    double getDailyFatGoal() const;
    double getDailyCarbsGoal() const;

  private:
    profile_data_t data_;
    bool hasData_;
    bool automaticGoals_[4];
  };
}

#endif
