#ifndef DATE_HPP
#define DATE_HPP
#include <string>

namespace nabieva
{
  class Date
  {
  public:
    Date(int day, int month, int year);

    bool operator==(const Date& other) const;
    bool operator!=(const Date& other) const;
    bool operator<(const Date& other) const;
    bool operator>(const Date& other) const;
    bool operator<=(const Date& other) const;
    bool operator>=(const Date& other) const;

    int getDay() const;
    int getMonth() const;
    int getYear() const;
    std::string getText() const;
    Date getPreviousDay() const;
    Date getNextDay() const;

  private:
    int day_;
    int month_;
    int year_;
  };

  Date parseDate(const std::string& text);
  Date getCurrentDate();
}

#endif
