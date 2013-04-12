
#ifndef CELL_HPP_
# define CELL_HPP_

# include <vector>
# include "tinyxml2.h"

namespace Link
{
  enum Type
    {
      ACTIVATOR,
      INIBITOR
    };
}

struct s_promoter;

typedef struct	s_protein
{
  std::vector<std::pair<struct s_promoter *, Link::Type> > promoters;
}		t_protein;

typedef struct s_cistron
{
}		t_cistron;

typedef struct	s_promoter
{
  std::string				name;
  std::vector<std::string>		linksName;
  std::vector<struct s_protein *>	proteins;
}		t_promoter;

class Cell
{
private:
  
  std::vector<t_protein*>	_proteins;
  std::vector<t_promoter*>	_promoters;
  
public:
  
  Cell();
  ~Cell(){}
  
  bool		LoadFromFile(const std::string &path);
  bool		loadPromoters(tinyxml2::XMLDocument &xml);
  bool		loadProteins(tinyxml2::XMLDocument &xml);
};

#endif
