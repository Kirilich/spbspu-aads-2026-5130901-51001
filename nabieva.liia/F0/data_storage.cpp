#include "data_storage.hpp"
#include <cerrno>
#include <cstdio>
#include <fstream>
#include <iomanip>
#include <limits>
#include <locale>
#include <stdexcept>
#include "input.hpp"
#include "nutrition.hpp"

namespace nabieva
{
  static bool fileExists(const std::string& path)
  {
    errno = 0;
    std::FILE* const file = std::fopen(path.c_str(), "rb");

    if (file == nullptr)
    {
      if (errno == ENOENT)
      {
        return false;
      }
      throw std::runtime_error("нет доступа к файлу данных.");
    }

    std::fclose(file);
    return true;
  }

  static std::string readWord(std::istream& input)
  {
    std::string word = "";
    if (!(input >> word))
    {
      throw std::invalid_argument("файл данных неполный.");
    }
    return word;
  }

  static void expectWord(std::istream& input, const char* expected)
  {
    if (readWord(input) != expected)
    {
      throw std::invalid_argument("неверный раздел файла данных.");
    }
  }

  static std::size_t readCount(std::istream& input)
  {
    const std::string word = readWord(input);
    std::size_t count = 0;
    if ((word != "0") && !readPositiveInteger(word, count))
    {
      throw std::invalid_argument("неверное количество записей.");
    }
    return count;
  }

  static double readValue(std::istream& input)
  {
    double value = 0.0;
    if (!readNumber(readWord(input), value))
    {
      throw std::invalid_argument("неверное число в файле данных.");
    }
    return value;
  }

  static Food readFood(std::istream& input)
  {
    const std::string name = readWord(input);
    const double calories = readValue(input);
    const double protein = readValue(input);
    const double fat = readValue(input);
    const double carbs = readValue(input);
    const std::string category = readWord(input);
    return Food(name, calories, protein, fat, carbs, category);
  }

  static void writeFood(std::ostream& output, const Food& food)
  {
    output << food.getName() << ' ' << food.getCalories() << ' ';
    output << food.getProtein() << ' ' << food.getFat() << ' ';
    output << food.getCarbs() << ' ' << food.getCategory();
  }

  static void readProfile(std::istream& input, Profile& profile)
  {
    expectWord(input, "PROFILE");
    const std::size_t present = readCount(input);
    if (present == 0)
    {
      return;
    }
    if (present != 1)
    {
      throw std::invalid_argument("неверный признак наличия профиля.");
    }

    profile_data_t data = {};
    data.name_ = readWord(input);
    data.currentWeight_ = readValue(input);
    data.targetWeight_ = readValue(input);
    data.sex_ = readWord(input);
    data.age_ = readCount(input);
    data.height_ = readValue(input);
    data.activity_ = readValue(input);
    data.calories_ = readValue(input);
    data.protein_ = readValue(input);
    data.fat_ = readValue(input);
    data.carbs_ = readValue(input);

    if (!profile.setProfile(data))
    {
      throw std::invalid_argument("сохранённый профиль содержит ошибки.");
    }
  }

  static void readCatalog(std::istream& input, FoodCatalog& catalog)
  {
    expectWord(input, "FOODS");
    const std::size_t count = readCount(input);
    for (std::size_t index = 0; index < count; ++index)
    {
      const Food food = readFood(input);
      if (!catalog.add(food.getName(), food))
      {
        throw std::invalid_argument("названия блюд повторяются.");
      }
    }
  }

  static void readWeights(std::istream& input, WeightHistory& history)
  {
    expectWord(input, "WEIGHTS");
    const std::size_t count = readCount(input);
    for (std::size_t index = 0; index < count; ++index)
    {
      const Date date = parseDate(readWord(input));
      const double weight = readValue(input);
      if ((weight <= 0.0) || !history.add(date, weight))
      {
        throw std::invalid_argument("запись веса неверна или повторяется.");
      }
    }
  }

  static void readDiary(std::istream& input, FoodDiary& diary)
  {
    expectWord(input, "DIARY");
    const std::size_t days = readCount(input);
    for (std::size_t day = 0; day < days; ++day)
    {
      const Date date = parseDate(readWord(input));
      const std::size_t count = readCount(input);
      if ((count == 0) || (diary.find(date) != nullptr))
      {
        throw std::invalid_argument("день дневника пуст или повторяется.");
      }

      MealList meals;
      MealList::iterator last = meals.end();
      for (std::size_t index = 0; index < count; ++index)
      {
        const Food food = readFood(input);
        const double grams = readValue(input);
        const Meal meal(food, grams);
        if (meals.isEmpty())
        {
          meals.pushFront(meal);
          last = meals.begin();
        }
        else
        {
          last = meals.insertAfter(last, meal);
        }
      }

      calculateNutrition(meals);
      diary.add(date, meals);
    }
  }

  static void writeSnapshot(std::ostream& output, const Profile& profile,
    const FoodCatalog& catalog, const WeightHistory& history, const FoodDiary& diary)
  {
    output.imbue(std::locale::classic());
    output << std::setprecision(std::numeric_limits< double >::max_digits10);
    output << "FITNESS_DATA 1\nPROFILE " << (profile.hasData() ? 1 : 0) << '\n';

    if (profile.hasData())
    {
      const profile_data_t data = profile.getInputData();
      output << data.name_ << ' ' << data.currentWeight_ << ' ' << data.targetWeight_ << ' ';
      output << data.sex_ << ' ' << data.age_ << ' ' << data.height_ << ' ' << data.activity_ << ' ';
      output << data.calories_ << ' ' << data.protein_ << ' ' << data.fat_ << ' ' << data.carbs_ << '\n';
    }

    output << "FOODS " << catalog.getSize() << '\n';
    const List< std::string > names = catalog.getKeys();
    for (List< std::string >::const_iterator current = names.cbegin();
      current != names.cend(); ++current)
    {
      writeFood(output, *catalog.find(*current));
      output << '\n';
    }

    output << "WEIGHTS " << history.getSize() << '\n';
    for (WeightHistory::const_iterator current = history.cbegin();
      current != history.cend(); ++current)
    {
      output << current->first.getText() << ' ' << current->second << '\n';
    }

    output << "DIARY " << diary.getSize() << '\n';
    for (FoodDiary::const_iterator day = diary.cbegin(); day != diary.cend(); ++day)
    {
      std::size_t count = 0;
      for (MealList::const_iterator meal = day->second.cbegin(); meal != day->second.cend(); ++meal)
      {
        ++count;
      }
      output << day->first.getText() << ' ' << count << '\n';

      for (MealList::const_iterator meal = day->second.cbegin(); meal != day->second.cend(); ++meal)
      {
        writeFood(output, meal->getFood());
        output << ' ' << meal->getGrams() << '\n';
      }
    }
    output << "END\n";
  }
}

std::string nabieva::getDataFilePath(const std::string& programPath)
{
  const std::size_t separator = programPath.find_last_of("/\\");
  if (separator == std::string::npos)
  {
    return "data/tracker.txt";
  }
  return programPath.substr(0, separator + 1) + "data/tracker.txt";
}

bool nabieva::loadData(const std::string& filePath, Profile& profile,
  FoodCatalog& catalog, WeightHistory& history, FoodDiary& diary)
{
  std::string sourcePath = filePath;
  if (!fileExists(sourcePath))
  {
    sourcePath += ".bak";
    if (!fileExists(sourcePath))
    {
      return false;
    }
  }

  std::ifstream file(sourcePath);
  if (!file)
  {
    throw std::runtime_error("не удалось открыть файл данных.");
  }
  file.imbue(std::locale::classic());

  Profile loadedProfile;
  const std::size_t bucketCount = 16;
  FoodCatalog loadedCatalog(bucketCount);
  WeightHistory loadedHistory;
  FoodDiary loadedDiary;

  expectWord(file, "FITNESS_DATA");
  expectWord(file, "1");
  readProfile(file, loadedProfile);
  readCatalog(file, loadedCatalog);
  readWeights(file, loadedHistory);
  readDiary(file, loadedDiary);
  if (!loadedProfile.hasData() && !loadedHistory.isEmpty())
  {
    throw std::invalid_argument("для истории веса нужен профиль.");
  }
  expectWord(file, "END");

  std::string extra = "";
  if (file >> extra)
  {
    throw std::invalid_argument("после конца данных обнаружены лишние записи.");
  }
  if (!file.eof() || file.bad())
  {
    throw std::runtime_error("не удалось завершить чтение файла данных.");
  }

  profile.swap(loadedProfile);
  catalog.swap(loadedCatalog);
  history.swap(loadedHistory);
  diary.swap(loadedDiary);
  return true;
}

void nabieva::saveData(const std::string& filePath, const Profile& profile,
  const FoodCatalog& catalog, const WeightHistory& history, const FoodDiary& diary)
{
  const std::string temporaryPath = filePath + ".tmp";
  const std::string backupPath = filePath + ".bak";
  std::ofstream file(temporaryPath);
  if (!file)
  {
    throw std::runtime_error("не удалось создать временный файл. проверьте папку data.");
  }

  writeSnapshot(file, profile, catalog, history, diary);
  file.close();
  if (!file)
  {
    throw std::runtime_error("не удалось записать файл данных.");
  }

  const bool hadPreviousFile = fileExists(filePath);
  if (hadPreviousFile)
  {
    if (fileExists(backupPath) && (std::remove(backupPath.c_str()) != 0))
    {
      throw std::runtime_error("не удалось заменить резервную копию данных.");
    }
    if (std::rename(filePath.c_str(), backupPath.c_str()) != 0)
    {
      throw std::runtime_error("не удалось сохранить предыдущий файл в резервную копию.");
    }
  }

  if (std::rename(temporaryPath.c_str(), filePath.c_str()) != 0)
  {
    if (hadPreviousFile && (std::rename(backupPath.c_str(), filePath.c_str()) != 0))
    {
      throw std::runtime_error("не удалось сохранить данные. предыдущая версия находится в tracker.txt.bak.");
    }
    throw std::runtime_error("не удалось заменить файл данных.");
  }
}
