
#include <sstream>
#include <iostream>
#include "tools.hpp"

unsigned	Tools::ahtoui(const std::string &str)
{
  std::stringstream ss;
  unsigned x;

  ss << std::hex << str;
  ss >> x;
  return x;
}
