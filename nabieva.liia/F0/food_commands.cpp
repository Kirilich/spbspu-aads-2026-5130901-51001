#include "food_commands.hpp"
#include <cctype>
#include <fstream>
#include <iomanip>
#include <istream>
#include <limits>
#include <locale>
#include <ostream>
#include <stdexcept>
#include "application_commands.hpp"
#include "console.hpp"
#include "nutrition_constants.hpp"

nabieva::CommandResult nabieva::addFood(std::istream& input, command_context_t& context,
    std::ostream& output)
{
  FoodCatalog& catalog = context.catalog_;
  std::string name = "";
  std::string category = "";
  std::string extra = "";
  double values[detail::nutritionValueCount] = {};

  if (!(input >> name))
  {
    throw std::invalid_argument("не указано название блюда.");
  }

  for (std::size_t index = 0; index < detail::nutritionValueCount; ++index)
  {
    if (!(input >> values[index]))
    {
      throw std::invalid_argument("неверное значение кбжу.");
    }

    if (!std::isspace(input.peek()))
    {
      throw std::invalid_argument("после числа нужен пробел.");
    }
  }

  if (!(input >> category) || (input >> extra))
  {
    throw std::invalid_argument("неверные параметры блюда.");
  }

  const Food food(name, values[0], values[detail::proteinIndex], values[detail::fatIndex],
      values[detail::carbsIndex], category);

  if (catalog.add(food.getName(), food))
  {
    output << "блюдо добавлено.\n";
    return CommandResult::changed;
  }

  output << "блюдо с таким названием уже существует.\n";
  return CommandResult::unchanged;
}

nabieva::CommandResult nabieva::removeFood(std::istream& input, command_context_t& context,
    std::ostream& output)
{
  FoodCatalog& catalog = context.catalog_;
  std::string name = "";

  if (!(input >> name))
  {
    throw std::invalid_argument("не указано название блюда.");
  }

  std::string extra = "";
  if (input >> extra)
  {
    throw std::invalid_argument("лишние параметры.");
  }

  if (catalog.remove(name))
  {
    output << "блюдо удалено.\n";
    return CommandResult::changed;
  }

  output << "блюдо для удаления не найдено.\n";
  return CommandResult::unchanged;
}

nabieva::CommandResult nabieva::listFood(std::istream& input, command_context_t& context,
    std::ostream& output)
{
  const FoodCatalog& catalog = context.catalog_;
  std::string extra = "";
  if (input >> extra)
  {
    throw std::invalid_argument("лишние параметры команды.");
  }

  if (catalog.isEmpty())
  {
    output << "каталог блюд пуст.\n";
    return CommandResult::unchanged;
  }

  const List< std::string > names = getSortedFoodNames(catalog);

  const std::size_t minimumNameWidth = getTextWidth("название");
  const std::size_t columnGap = 2;
  std::size_t nameWidth = minimumNameWidth;

  for (List< std::string >::const_iterator current = names.cbegin(); current != names.cend();
      ++current)
  {
    if (getTextWidth(*current) > nameWidth)
    {
      nameWidth = getTextWidth(*current);
    }
  }

  nameWidth += columnGap;

  const int categoryWidth = 16;
  const int numericWidth = 14;

  const std::ios::fmtflags originalFlags = output.flags();

  output << "каталог блюд:\n";

  printText(output, "название", nameWidth);
  printText(output, "категория", categoryWidth);
  printText(output, "ккал", numericWidth, false);
  printText(output, "белки, г", numericWidth, false);
  printText(output, "жиры, г", numericWidth, false);
  printText(output, "углеводы, г", numericWidth, false);
  output << '\n';

  for (List< std::string >::const_iterator current = names.cbegin(); current != names.cend();
      ++current)
  {
    const Food* const food = &catalog.find(*current)->second;

    printText(output, food->getName(), nameWidth);
    printText(output, getCategoryText(food->getCategory()), categoryWidth);

    output << std::right << std::setw(numericWidth) << food->getCalories();

    output << std::setw(numericWidth) << food->getProtein();
    output << std::setw(numericWidth) << food->getFat();
    output << std::setw(numericWidth) << food->getCarbs() << '\n';
  }

  output.flags(originalFlags);
  return CommandResult::unchanged;
}

nabieva::CommandResult nabieva::saveFood(std::istream& input, command_context_t& context,
    std::ostream& output)
{
  const FoodCatalog& catalog = context.catalog_;
  std::string fileName = "";
  std::string extra = "";

  if (!(input >> fileName) || (input >> extra))
  {
    throw std::invalid_argument("нужно указать одно имя файла.");
  }

  const List< std::string > names = catalog.getKeys();

  std::ofstream file(fileName);

  if (!file)
  {
    throw std::runtime_error("не удалось создать файл каталога блюд.");
  }

  file.imbue(std::locale::classic());

  file << std::setprecision(std::numeric_limits< double >::max_digits10);

  file << "FOOD_CATALOG 1\n";
  file << catalog.getSize() << '\n';

  for (List< std::string >::const_iterator current = names.cbegin(); current != names.cend();
      ++current)
  {
    const Food* const food = &catalog.find(*current)->second;

    file << food->getName() << ' ';
    file << food->getCalories() << ' ';
    file << food->getProtein() << ' ';
    file << food->getFat() << ' ';
    file << food->getCarbs() << ' ';
    file << food->getCategory() << '\n';
  }

  file.close();

  if (!file)
  {
    throw std::runtime_error("не удалось записать каталог блюд.");
  }

  output << "каталог блюд сохранён: " << fileName << '\n';
  return CommandResult::unchanged;
}
nabieva::CommandResult nabieva::loadFood(std::istream& input, command_context_t& context,
    std::ostream& output)
{
  FoodCatalog& catalog = context.catalog_;
  std::string fileName = "";
  std::string extra = "";

  if (!(input >> fileName) || (input >> extra))
  {
    throw std::invalid_argument("нужно указать одно имя файла.");
  }

  std::ifstream file(fileName);

  if (!file)
  {
    throw std::runtime_error("не удалось открыть файл каталога блюд.");
  }

  file.imbue(std::locale::classic());

  std::string marker = "";
  std::string version = "";
  std::size_t count = 0;

  if (!(file >> marker >> version) || (marker != "FOOD_CATALOG") || (version != "1"))
  {
    throw std::invalid_argument("неверный заголовок файла каталога блюд.");
  }

  file >> std::ws;
  if (!std::isdigit(file.peek()) || !(file >> count) || (!file.eof() && !std::isspace(file.peek())))
  {
    throw std::invalid_argument("неверное количество блюд.");
  }

  const std::size_t bucketCount = 16;
  FoodCatalog loadedCatalog(bucketCount);

  for (std::size_t index = 0; index < count; ++index)
  {
    std::string name = "";
    std::string category = "";
    double values[detail::nutritionValueCount] = {};

    if (!(file >> name))
    {
      throw std::invalid_argument("запись о блюде неполная.");
    }

    for (std::size_t number = 0; number < detail::nutritionValueCount; ++number)
    {
      if (!(file >> values[number]) || !std::isspace(file.peek()))
      {
        throw std::invalid_argument("неверное число в характеристиках блюда.");
      }
    }

    if (!(file >> category))
    {
      throw std::invalid_argument("не указана категория блюда.");
    }

    const Food food(name, values[0], values[detail::proteinIndex], values[detail::fatIndex],
        values[detail::carbsIndex], category);

    if (!loadedCatalog.add(name, food))
    {
      throw std::invalid_argument("названия блюд повторяются.");
    }
  }

  if (file >> extra)
  {
    throw std::invalid_argument("после каталога обнаружены лишние данные.");
  }

  if (!file.eof() || file.bad())
  {
    throw std::runtime_error("не удалось прочитать каталог блюд.");
  }

  catalog.swap(loadedCatalog);

  output << "каталог блюд загружен: " << fileName << '\n';
  return CommandResult::changed;
}
