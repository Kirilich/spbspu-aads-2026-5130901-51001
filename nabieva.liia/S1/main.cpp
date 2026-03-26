#include <iostream>
#include <string>
#include <sstream>
#include <limits>
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
    int number;
    while (std::cin.peek() != '\n' && std::cin >> number)
    {
      seq.numbers.push_back(number);
    }
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
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

  nabieva::List<nabieva::LIter<int>> iters;
  for (nabieva::LIter<NamedList> it = sequences.begin(); it != sequences.end(); ++it)
  {
    iters.push_back((*it).numbers.begin());
  }
  bool finished = false;
  nabieva::List<long long> sums;
  while (!finished)
  {
    finished = true;
    long long sum = 0;
    nabieva::LIter<NamedList> seqIt = sequences.begin();
    nabieva::LIter<nabieva::LIter<int>> iterIt = iters.begin();
    while (seqIt != sequences.end())
    {
      nabieva::List<int>& list = (*seqIt).numbers;
      nabieva::LIter<int>& it = *iterIt;
      if (it != list.end())
      {
        int value = *it;
        std::cout << value << " ";
        if (sum > std::numeric_limits<long long>::max() - value)
        {
          std::cerr << "overflow\n";
          return 1;
        }
        sum += value;
        ++it;
        finished = false;
      }
      ++seqIt;
      ++iterIt;
    }
    if (!finished)
    {
      sums.push_back(sum);
      std::cout << "\n";
    }
  }
  for (nabieva::LIter<long long> it = sums.begin(); it != sums.end(); ++it)
  {
    std::cout << *it << " ";
  }
  std::cout << "\n";
  return 0;
}
