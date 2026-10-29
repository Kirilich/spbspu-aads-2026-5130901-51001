#include <iostream>
#include <stdexcept>
#include <string>
#include "application_commands.hpp"
#include "data_storage.hpp"
#include "input.hpp"

namespace nabieva
{
  static bool saveApplicationData(const std::string& filePath, const Profile& profile,
    const FoodCatalog& catalog, const WeightHistory& history, const FoodDiary& diary,
    bool savingEnabled)
  {
    if (!savingEnabled)
    {
      std::cerr << "изменения остались только в памяти: автосохранение отключено.\n";
      return false;
    }

    try
    {
      saveData(filePath, profile, catalog, history, diary);
      return true;
    }
    catch (const std::exception& error)
    {
      std::cerr << "не удалось сохранить данные: " << error.what() << '\n';
      return false;
    }
  }
}

int main(int argc, char* argv[])
{
  nabieva::Profile profile;
  nabieva::WeightHistory weightHistory;
  nabieva::FoodDiary diary;
  const std::size_t bucketCount = 16;
  nabieva::FoodCatalog catalog(bucketCount);
  std::string dataFile = nabieva::getDataFilePath((argc > 0) ? argv[0] : "");
  if ((argc == 3) && (std::string(argv[1]) == "--data-dir")
    && !std::string(argv[2]).empty())
  {
    dataFile = std::string(argv[2]) + "/tracker.txt";
  }
  else if (argc != 1)
  {
    std::cerr << "запуск: fitness [--data-dir <существующая_папка>]\n";
    return 1;
  }

  bool savingEnabled = true;
  try
  {
    if (nabieva::loadData(dataFile, profile, catalog, weightHistory, diary))
    {
      std::cout << "сохранённые данные загружены.\n\n";
    }
  }
  catch (const std::exception& error)
  {
    std::cerr << "не удалось загрузить данные: " << error.what() << '\n';
    std::cerr << "автосохранение отключено для защиты существующего файла: " << dataFile << '\n';
    savingEnabled = false;
  }

  nabieva::command_context_t context = { profile, catalog, weightHistory, diary, dataFile };

  std::string line = "";
  bool firstCommand = true;

  while (std::getline(std::cin, line))
  {
    std::size_t position = 0;
    std::string command = "";

    if (!nabieva::readToken(line, position, command))
    {
      continue;
    }
    if (command.empty())
    {
      continue;
    }
    if (!firstCommand)
    {
      std::cout << '\n';
    }
    firstCommand = false;

    try
    {
      const nabieva::CommandResult result = nabieva::executeCommand(
        command, line, position, context, std::cout);
      if (result == nabieva::CommandResult::exit)
      {
        break;
      }
      if ((result == nabieva::CommandResult::changed)
        || (result == nabieva::CommandResult::profileChanged))
      {
        const bool saved = nabieva::saveApplicationData(
          dataFile, profile, catalog, weightHistory, diary, savingEnabled);
        if (result == nabieva::CommandResult::profileChanged)
        {
          std::cout << (saved ? "профиль сохранён.\n"
            : "профиль изменён только в памяти; в файл данные не сохранены.\n");
        }
      }
    }
    catch (const std::exception&)
    {
      std::cout << "<INVALID COMMAND>\n";
    }
  }

  if (std::cin.bad())
  {
    const int inputErrorCode = 1;
    std::cerr << "ошибка чтения ввода.\n";
    return inputErrorCode;
  }

  if (!nabieva::saveApplicationData(
    dataFile, profile, catalog, weightHistory, diary, savingEnabled))
  {
    return 2;
  }

  return 0;
}
