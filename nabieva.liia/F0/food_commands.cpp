#include "food_commands.hpp"
#include "console.hpp"
#include <iomanip>
#include <ostream>
#include <stdexcept>
#include "input.hpp"
#include <fstream>
#include <limits>
#include <locale>

bool nabieva::addFood(
  FoodCatalog& catalog,
  const std::string& line,
  std::size_t position,
  std::ostream& output)
{
  const std::size_t argumentCount = 6;
  std::string arguments[argumentCount] = {};

  for (std::size_t index = 0; index < argumentCount; ++index)
  {
    if (!readToken(line, position, arguments[index]))
    {
      throw std::invalid_argument("не хватает параметров блюда.");
    }
  }

  std::string extra = "";

  if (readToken(line, position, extra))
  {
    throw std::invalid_argument("неверные параметры блюда.");
  }

  const std::size_t valueCount = 4;
  double values[valueCount] = {};

  for (std::size_t index = 0; index < valueCount; ++index)
  {
    if (!readNumber(arguments[index + 1], values[index]))
    {
      throw std::invalid_argument("неверное число в характеристиках блюда.");
    }
  }

  const Food food(
    arguments[0],
    values[0],
    values[1],
    values[2],
    values[3],
    arguments[5]);

  if (catalog.add(food.getName(), food))
  {
    output << "блюдо добавлено.\n";
    return true;
  }
  else
  {
    output << "блюдо с таким названием уже существует.\n";
    return false;
  }
}

bool nabieva::removeFood(
  FoodCatalog& catalog,
  const std::string& line,
  std::size_t position,
  std::ostream& output)
{
  std::string name = "";
  std::string extra = "";

  if (!readToken(line, position, name)
    || readToken(line, position, extra))
  {
    throw std::invalid_argument("неверные параметры блюда.");
  }

  if (catalog.remove(name))
  {
    output << "блюдо удалено.\n";
    return true;
  }
  else
  {
    output << "блюдо для удаления не найдено.\n";
    return false;
  }
}

void nabieva::listFood(
  const FoodCatalog& catalog, std::ostream& output)
{
  if (catalog.isEmpty())
  {
    output << "каталог блюд пуст.\n";
    return;
  }

  const List< std::string > names = getSortedFoodNames(catalog);

  const std::size_t minimumNameWidth = getTextWidth("название");
  const std::size_t columnGap = 2;
  std::size_t nameWidth = minimumNameWidth;

  for (List< std::string >::const_iterator current = names.cbegin();
    current != names.cend(); ++current)
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

  for (List< std::string >::const_iterator current = names.cbegin();
    current != names.cend(); ++current)
  {
    const Food* const food = catalog.find(*current);

    printText(output, food->getName(), nameWidth);
    printText(output, getCategoryText(food->getCategory()), categoryWidth);

    output << std::right
      << std::setw(numericWidth)
      << food->getCalories();

    output << std::setw(numericWidth) << food->getProtein();
    output << std::setw(numericWidth) << food->getFat();
    output << std::setw(numericWidth) << food->getCarbs() << '\n';
  }

  output.flags(originalFlags);
}

void nabieva::saveFood(
  const FoodCatalog& catalog,
  const std::string& line,
  std::size_t position,
  std::ostream& output)
{
  std::string fileName = "";
  std::string extra = "";

  if (!readToken(line, position, fileName)
    || readToken(line, position, extra))
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

  file << std::setprecision(
    std::numeric_limits< double >::max_digits10);

  file << "FOOD_CATALOG 1\n";
  file << catalog.getSize() << '\n';

  for (List< std::string >::const_iterator current = names.cbegin();
    current != names.cend(); ++current)
  {
    const Food* const food = catalog.find(*current);

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
}
bool nabieva::loadFood(
  FoodCatalog& catalog,
  const std::string& line,
  std::size_t position,
  std::ostream& output)
{
  std::string fileName = "";
  std::string extra = "";

  if (!readToken(line, position, fileName)
    || readToken(line, position, extra))
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
  std::string countText = "";
  std::size_t count = 0;

  if (!(file >> marker >> version >> countText)
    || (marker != "FOOD_CATALOG")
    || (version != "1")
    || ((countText != "0") && !readPositiveInteger(countText, count)))
  {
    throw std::invalid_argument("неверный заголовок файла каталога блюд.");
  }

  const std::size_t bucketCount = 16;
  FoodCatalog loadedCatalog(bucketCount);

  for (std::size_t index = 0; index < count; ++index)
  {
    std::string name = "";
    std::string category = "";
    std::string numbers[4] = {};

    if (!(file >> name >> numbers[0] >> numbers[1]
      >> numbers[2] >> numbers[3] >> category))
    {
      throw std::invalid_argument("запись о блюде неполная.");
    }

    double values[4] = {};

    for (std::size_t number = 0; number < 4; ++number)
    {
      if (!readNumber(numbers[number], values[number]))
      {
        throw std::invalid_argument("неверное число в характеристиках блюда.");
      }
    }

    const Food food(name, values[0], values[1],
      values[2], values[3], category);

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
  return true;
}
