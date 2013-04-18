
#ifndef TOOLS_HPP_
# define TOOLS_HPP_

# include <string>

# define GETFRAND(r) (1.f - ((float)(rand() % r) / r))

namespace Tools
{
  unsigned	ahtoui(const std::string &str);
}

#endif
