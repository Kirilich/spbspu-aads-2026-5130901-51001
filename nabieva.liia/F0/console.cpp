#include "console.hpp"
#include <ostream>

std::size_t nabieva::getTextWidth(const std::string& text)
{
  std::size_t width = 0;
  const unsigned char continuationMask = 0xC0;
  const unsigned char continuationPrefix = 0x80;
  for (std::size_t index = 0; index < text.size(); ++index)
  {
    const unsigned char symbol = static_cast<unsigned char>(text[index]);
    if ((symbol & continuationMask) != continuationPrefix)
    {
      ++width;
    }
  }
  return width;
}

void nabieva::printText(std::ostream& output, const std::string& text,
  std::size_t width, bool leftAligned)
{
  const std::size_t length = getTextWidth(text);
  const std::size_t padding = (width > length) ? width - length : 0;
  if (!leftAligned)
  {
    output << std::string(padding, ' ');
  }
  output << text;
  if (leftAligned)
  {
    output << std::string(padding, ' ');
  }
}

std::string nabieva::getCategoryText(const std::string& category)
{
  if (category == "soup")
  {
    return "первое блюдо";
  }
  if (category == "main")
  {
    return "второе блюдо";
  }
  if (category == "side")
  {
    return "гарнир";
  }
  if (category == "dessert")
  {
    return "десерт/сладкое";
  }
  if (category == "drink")
  {
    return "напиток";
  }
  return category;
}

std::string nabieva::getPlanModeText(const std::string& mode)
{
  if (mode == "mild")
  {
    return "мягкий (mild)";
  }
  if (mode == "normal")
  {
    return "обычный (normal)";
  }
  if (mode == "aggressive")
  {
    return "интенсивный (aggressive)";
  }
  return mode;
}
