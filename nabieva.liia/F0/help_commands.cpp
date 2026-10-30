#include "help_commands.hpp"
#include <istream>
#include <ostream>
#include <stdexcept>
#include "application_commands.hpp"

nabieva::CommandResult nabieva::printHelp(std::istream& input, command_context_t& context,
    std::ostream& output)
{
  const std::string& dataFile = context.dataFile_;
  std::string extra = "";
  if (input >> extra)
  {
    throw std::invalid_argument("лишние параметры команды.");
  }

  output << "доступные команды:\n";
  output << "help — показать список команд\n";
  output << "profile <имя> <вес> <желаемый_вес> <male/female> "
         << "<возраст> <рост_см> <активность>\n";
  output << "        [<ккал/auto> [<белки/auto> "
         << "[<жиры/auto> [<углеводы/auto>]]]]\n";
  output << "коэффициент активности PAL: 1.4–2.4. "
         << "автоматический расчёт калорий: возраст 19–78 лет.\n";
  output << "пол: male — мужской, female — женский. auto — рассчитать автоматически.\n";
  output << "даты: дд.мм.гггг; дробные числа — с точкой.\n";
  output << "параметры в квадратных скобках необязательны; дата по умолчанию — сегодня.\n";
  output << "\nshow-profile — показать профиль\n";
  output << "food-save <имя_файла> — сохранить каталог блюд\n";
  output << "food-load <имя_файла> — загрузить каталог блюд\n";
  output << "food-add <название> <ккал> <белки> <жиры> "
         << "<углеводы> <категория>\n";

  output << "категории: soup — первое блюдо, main — второе блюдо,\n           side "
            "— гарнир, dessert — десерт/сладкое, drink — напиток.\n";
  output << "food-remove <название> — удалить блюдо из каталога\n";
  output << "food-list — показать каталог блюд\n";
  output << "\nweight-log <вес> [<дата>] — записать вес\n";
  output << "weight-remove <дата> — удалить запись веса\n";
  output << "weight-list — показать историю веса\n";
  output << "weight-graph — показать график веса\n";
  output << "\neat <название_блюда> <граммы> [<дата>] — записать съеденное\n";
  output << "eat-list [<дата>] — показать записи о питании\n";
  output << "eat-remove <номер> [<дата>] — удалить запись о питании\n";
  output << "\nday-stats [<дата>] — показать статистику за день\n";
  output << "week-stats [<конечная_дата>] — показать статистику за 7 дней\n";
  output << "export <начальная_дата> <конечная_дата> — сохранить отчёт\n";
  output << "\nanalyze-day [<дата>] — проанализировать питание\n";
  output << "recommend-meal — порекомендовать блюдо на сегодня\n";
  output << "generate-plan <первая_дата> <дни:1-31> "
            "<mild/normal/aggressive>\nрежимы: mild — мягкий (10%), normal — "
            "обычный (20%), aggressive — интенсивный (30%).\n";
  output << "\nфайл автоматического хранения: " << dataFile << '\n';
  output << "данные сохраняются после изменений и загружаются при запуске.\n";
  output << "exit — закрыть программу\n";
  return CommandResult::unchanged;
}

nabieva::CommandResult nabieva::exitProgram(std::istream& input, command_context_t&, std::ostream&)
{
  std::string extra = "";
  if (input >> extra)
  {
    throw std::invalid_argument("лишние параметры команды.");
  }

  return CommandResult::exit;
}
