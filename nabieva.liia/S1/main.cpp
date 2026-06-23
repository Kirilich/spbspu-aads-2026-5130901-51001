#include <iostream>
#include <string>
#include <sstream>
#include <limits>
#include "list.hpp"

struct NamedList
{
  std::string name;
  nabieva::List<size_t> numbers;
};


namespace nabieva {
  void readInput(List<NamedList> sequences) {
    while (std::getline(std::cin, line))
    {
      if (line.empty()) continue;

      std::istringstream iss(line);
      std::string name;
      iss >> name;

      if (name.empty()) continue;

      NamedList seq;
      seq.name = name;

      size_t number;
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
  }

  void printName(List<NamedList> sequences) {
    for (nabieva::LIter<NamedList> it = sequences.begin(); it != sequences.end(); ++it)
    {
      std::cout << (*it).name;
      nabieva::LIter<NamedList> next = it;
      ++next;
      if (next != sequences.end())
        std::cout << " ";
    }
    std::cout << "\n";
  }

  nabieva::List<nabieva::List<size_t>> countSum(List<NamedList> sequences) {
    nabieva::List<nabieva::LIter<size_t>> currentIters;
    for (nabieva::LIter<NamedList> it = sequences.begin(); it != sequences.end(); ++it)
    {
      currentIters.push_back((*it).numbers.begin());
    }

    nabieva::List<size_t> sums;
    bool hasElements = true;

    while (hasElements)
    {
      hasElements = false;
      size_t sum = 0;
      nabieva::LIter<nabieva::LIter<size_t>> iterIt = currentIters.begin();
      nabieva::LIter<NamedList> seqIt = sequences.begin();

      while (seqIt != sequences.end() && iterIt != currentIters.end())
      {
        nabieva::List<size_t>& numbers = (*seqIt).numbers;
        nabieva::LIter<size_t>& current = *iterIt;

        if (current != numbers.end())
        {
          size_t value = *current;
          if (sum > std::numeric_limits<size_t>::max() - value)
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
      }

      if (hasElements)
      {
        sums.push_back(sum);
      }
    }
  }
}

int main()
{
  nabieva::List<NamedList> sequences;
  std::string line;

  readInput(&sequences);

  printName(&sequences);

  nabieva::List<nabieva::LIter<size_t>> currentItersForOutput;
  for (nabieva::LIter<NamedList> it = sequences.begin(); it != sequences.end(); ++it)
  {
    currentItersForOutput.push_back((*it).numbers.begin());
  }

  bool hasElementsForOutput = true;
  while (hasElementsForOutput)
  {
    hasElementsForOutput = false;
    nabieva::LIter<nabieva::LIter<size_t>> iterIt = currentItersForOutput.begin();
    nabieva::LIter<NamedList> seqIt = sequences.begin();
    bool firstElement = true;

    while (seqIt != sequences.end() && iterIt != currentItersForOutput.end())
    {
      nabieva::List<size_t>& numbers = (*seqIt).numbers;
      nabieva::LIter<size_t>& current = *iterIt;

      if (current != numbers.end())
      {
        if (!firstElement)
        {
          std::cout << " ";
        }
        firstElement = false;

        std::cout << *current;
        ++current;
        hasElementsForOutput = true;
      }

      ++seqIt;
      ++iterIt;
    }

    if (hasElementsForOutput)
    {
      std::cout << "\n";
    }
  }

  if (sums.empty())
  {
    std::cout << "0\n";
  }
  else
  {
    for (nabieva::LIter<size_t> it = sums.begin(); it != sums.end(); ++it)
    {
      std::cout << *it;
      nabieva::LIter<size_t> next = it;
      ++next;
      if (next != sums.end())
        std::cout << " ";
    }
    std::cout << "\n";
  }
  return 0;
}
