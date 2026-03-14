#include <iostream>
#include <string>
#include <sstream>
#include "list.hpp"

struct NamedList
{
  std::string name;
  nabieva::List<int> numbers;
};

int main()
{
  nabieva::List<NamedList> sequences;
  std::string name;
  while (std::cin >> name)
  {
    NamedList seq;
    seq.name = name;

    std::string line;
    std::getline(std::cin, line);
    std::stringstream ss(line);
    int number;
    while (ss >> number)
    {
      seq.numbers.push_back(number);
    }
    sequences.push_back(seq);
  }
  if (sequences.empty())
  {
    std::cout << 0 << "\n";
    return 0;
  }
  for (nabieva::LIter<NamedList> it = sequences.begin(); it != sequences.end(); ++it)
  {
    std::cout << (*it).name << " ";
  }
  std::cout << "\n";
}
