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
  void readInput(List<NamedList>& sequences) {
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

      size_t number;
      while (iss >> number)
      {
        seq.numbers.push_back(number);
      }
      sequences.push_back(seq);
    }
  }

  nabieva::List<nabieva::List<size_t>> transposeToColumns(nabieva::List<NamedList>& sequences)
  {
    nabieva::List<nabieva::List<size_t>> columns;
    size_t maxLength = 0;
    for (nabieva::LIter<NamedList> it = sequences.begin(); it != sequences.end(); ++it)
    {
      size_t length = 0;
      for (nabieva::LIter<size_t> numIt = (*it).numbers.begin(); numIt != (*it).numbers.end(); ++numIt)
      {
        ++length;
      }
      if (length > maxLength)
        maxLength = length;
    }
    for (size_t col = 0; col < maxLength; ++col)
    {
      nabieva::List<size_t> column;
      for (nabieva::LIter<NamedList> seqIt = sequences.begin(); seqIt != sequences.end(); ++seqIt)
      {
        nabieva::List<size_t>& numbers = (*seqIt).numbers;
        nabieva::LIter<size_t> numIt = numbers.begin();
        for (size_t i = 0; i < col && numIt != numbers.end(); ++i)
        {
          ++numIt;
        }

        if (numIt != numbers.end())
        {
          column.push_back(*numIt);
        }
      }
      columns.push_back(column);
    }
    return columns;
  }

  void printName(List<NamedList>& sequences) {
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

  nabieva::List<size_t> countSum(nabieva::List<nabieva::List<size_t>>& columns)
  {
    nabieva::List<size_t> sums;
    if (columns.empty())
    {
      sums.push_back(0);
      return sums;
    }
    for (nabieva::LIter<nabieva::List<size_t>> colIt = columns.begin(); colIt != columns.end(); ++colIt)
    {
      size_t sum = 0;
      nabieva::List<size_t>& column = *colIt;
      for (nabieva::LIter<size_t> numIt = column.begin(); numIt != column.end(); ++numIt)
      {
        size_t value = *numIt;
        if (sum > std::numeric_limits<size_t>::max() - value)
        {
          throw std::overflow_error("overflow");
        }
        sum += value;
      }
      sums.push_back(sum);
    }
    return sums;
  }

  void printColumn(nabieva::List<nabieva::List<size_t>>& columns) {
    for (nabieva::LIter<nabieva::List<size_t>> colIt = columns.begin(); colIt != columns.end(); ++colIt)
    {
      nabieva::List<size_t>& column = *colIt;
      bool first = true;
      for (nabieva::LIter<size_t> it = column.begin(); it != column.end(); ++it)
      {
        if (!first)
          std::cout << " ";
        first = false;
        std::cout << *it;
      }
      std::cout << "\n";
    }
  }

  void printSum(nabieva::List<size_t>& sums) {
    bool first = true;
    for (nabieva::LIter<size_t> it = sums.begin(); it != sums.end(); ++it)
    {
      if (!first)
        std::cout << " ";
      first = false;
      std::cout << *it;
    }
    std::cout << "\n";
  }
}

int main()
{
  nabieva::List<NamedList> sequences;
  try {
    readInput(sequences);
    if (sequences.empty())
    {
      std::cout << "0\n";
      return 0;
    }
    nabieva::List<nabieva::List<size_t>> columns = transposeToColumns(sequences);
    printName(sequences);
    printColumn(columns);
    nabieva::List<size_t> sums = countSum(columns);
    printSum(sums);
  }
  catch (const std::overflow_error& e)
  {
    std::cerr << e.what() << "\n";
    return 1;
  }
  return 0;
}
