#include "food_catalog.hpp"

nabieva::List< std::string > nabieva::getSortedFoodNames(const FoodCatalog& catalog)
{
  List< std::string > names = catalog.getKeys();
  for (List< std::string >::iterator first = names.begin(); first != names.end(); ++first)
  {
    List< std::string >::iterator smallest = first;
    List< std::string >::iterator current = first;
    ++current;
    while (current != names.end())
    {
      if (*current < *smallest)
      {
        smallest = current;
      }
      ++current;
    }
    if (smallest != first)
    {
      const std::string temporary = *first;
      *first = *smallest;
      *smallest = temporary;
    }
  }
  return names;
}
