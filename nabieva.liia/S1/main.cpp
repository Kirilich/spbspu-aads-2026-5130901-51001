#include <iostream>
#include <string>
#include <sstream>
#include <limits>
#include "list.hpp"

struct NamedList
{
  std::string name;
  nabieva::List<unsigned long long> numbers;
};

int main()
{
  nabieva::List<NamedList> sequences;
  std::string line;
  while (std::getline(std::cin, line))
  {
    if (line.empty()) continue;

    std::istringstream iss(line);
    std::string name;
    iss >> name;

    if (name.empty()) continue;

    NamedList seq;
    seq.name = name;

    unsigned long long number;
    while (iss >> number)
    {
      seq.numbers.push_back(number);
    }
    sequences.push_back(seq);
  }

  if (sequences.empty())
  {
    std::cout << "0\n";
    return 0;
  }
  for (nabieva::LIter<NamedList> it = sequences.begin(); it != sequences.end(); ++it)
  {
    std::cout << (*it).name;
    nabieva::LIter<NamedList> next = it;
    ++next;
    if (next != sequences.end())
      std::cout << " ";
  }
  std::cout << "\n";
  nabieva::List<nabieva::LIter<unsigned long long>> currentIters;
  for (nabieva::LIter<NamedList> it = sequences.begin(); it != sequences.end(); ++it)
  {
    currentIters.push_back((*it).numbers.begin());
  }

  nabieva::List<unsigned long long> sums;
  bool hasElements = true;

  while (hasElements)
  {
    hasElements = false;
    unsigned long long sum = 0;
    nabieva::LIter<nabieva::LIter<unsigned long long>> iterIt = currentIters.begin();
    nabieva::LIter<NamedList> seqIt = sequences.begin();

    while (seqIt != sequences.end() && iterIt != currentIters.end())
    {
      nabieva::List<unsigned long long>& numbers = (*seqIt).numbers;
      nabieva::LIter<unsigned long long>& current = *iterIt;

      if (current != numbers.end())
      {
        unsigned long long value = *current;
        std::cout << value;
        if (sum > std::numeric_limits<unsigned long long>::max() - value)
        {
          std::cerr << "overflow\n";
          return 1;
        }
        sum += value;
        ++current;
        hasElements = true;
      }
      ++seqIt;
      ++iterIt;
      if (seqIt != sequences.end() && hasElements)
      {
        nabieva::LIter<unsigned long long>& nextCurrent = *iterIt;
        if (nextCurrent != (*seqIt).numbers.end())
        {
          std::cout << " ";
        }
      }
    }

    if (hasElements)
    {
      sums.push_back(sum);
      std::cout << "\n";
    }
  }
  if (sums.empty())
  {
    std::cout << "0\n";
  }
  else
  {
    for (nabieva::LIter<unsigned long long> it = sums.begin(); it != sums.end(); ++it)
    {
      std::cout << *it;
      nabieva::LIter<unsigned long long> next = it;
      ++next;
      if (next != sums.end())
        std::cout << " ";
    }
    std::cout << "\n";
  }
  return 0;
}
