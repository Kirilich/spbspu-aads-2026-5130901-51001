#include <exception>
#include <iostream>
#include <sstream>
#include <string>
#include "application_commands.hpp"
#include "data_storage.hpp"

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
  const int defaultArgumentCount = 1;
  const int customPathArgumentCount = 3;
  const int optionIndex = 1;
  const int pathIndex = 2;
  const int inputErrorCode = 1;
  const int storageErrorCode = 2;

  nabieva::Profile profile;
  nabieva::WeightHistory weightHistory;
  nabieva::FoodDiary diary;
  const std::size_t bucketCount = 16;
  nabieva::FoodCatalog catalog(bucketCount);
  std::string dataFile = nabieva::getDataFilePath((argc > 0) ? argv[0] : "");
  if ((argc == customPathArgumentCount) && (std::string(argv[optionIndex]) == "--data-dir")
      && !std::string(argv[pathIndex]).empty())
  {
    dataFile = std::string(argv[pathIndex]) + "/tracker.txt";
  }
  else if (argc != defaultArgumentCount)
  {
    std::cerr << "запуск: fitness [--data-dir <существующая_папка>]\n";
    return inputErrorCode;
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

  nabieva::command_context_t context = {profile, catalog, weightHistory, diary, dataFile};

  nabieva::CommandTree commands;
  nabieva::registerCommands(commands);

  std::string line = "";
  bool firstCommand = true;

  while (std::getline(std::cin, line))
  {
    std::istringstream input(line);
    std::string command = "";

    if (!(input >> command))
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
      const nabieva::CommandResult result =
          nabieva::executeCommand(commands, command, input, context, std::cout);
      if (result == nabieva::CommandResult::exit)
      {
        break;
      }
      if ((result == nabieva::CommandResult::changed)
          || (result == nabieva::CommandResult::profileChanged))
      {
        const bool saved = nabieva::saveApplicationData(dataFile, profile, catalog, weightHistory,
            diary, savingEnabled);
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
    std::cerr << "ошибка чтения ввода.\n";
    return inputErrorCode;
  }

  if (!nabieva::saveApplicationData(dataFile, profile, catalog, weightHistory, diary,
          savingEnabled))
  {
    return storageErrorCode;
  }

  return 0;
}
