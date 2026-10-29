#ifndef FOOD_HPP
#define FOOD_HPP

#include <string>

namespace nabieva
{
  class Food
  {
  public:
    Food(const std::string& name, double calories, double protein,
      double fat, double carbs, const std::string& category);

    const std::string& getName() const;
    const std::string& getCategory() const;

    double getCalories() const;
    double getProtein() const;
    double getFat() const;
    double getCarbs() const;

  private:
    std::string name_;
    double calories_;
    double protein_;
    double fat_;
    double carbs_;
    std::string category_;

    void validate() const;
  };
}

#endif
