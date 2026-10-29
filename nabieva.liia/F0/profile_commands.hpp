#ifndef PROFILE_COMMANDS_HPP
#define PROFILE_COMMANDS_HPP

#include <cstddef>
#include <string>

namespace nabieva
{
  class Profile;

  void setProfileFromCommand(Profile& profile, const std::string& line, std::size_t position);
}

#endif
