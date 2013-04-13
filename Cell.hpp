
#ifndef CELL_HPP_
# define CELL_HPP_

# include <vector>
# include "tinyxml2.h"
# include "plot.hpp"

namespace Link
{
  enum Type
    {
      ACTIVATOR,
      REPRESSOR
    };
}

struct s_promoter;

typedef struct	s_protein
{
  std::string			name;
  float				concentration;
  float				degradationRate;
  unsigned			curveColor;
  Curve				*curve;
  std::vector<std::pair<std::string, Link::Type> >	linksName;
  std::vector<std::pair<struct s_promoter *, Link::Type> > promoters;
}		t_protein;

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
  bool				_live;
  float				_time;

  Plot				*_plot;
  bool				_isPlot;

public:
  
  Cell(bool isPlot = false);
  ~Cell();
  
  bool		LoadFromFile(const std::string &path);
  bool		loadPromoters(tinyxml2::XMLDocument &xml);
  void		pushProtein(t_protein *protein, tinyxml2::XMLNode *name,
			    tinyxml2::XMLNode *linkType);
  bool		loadProteins(tinyxml2::XMLDocument &xml);
  t_protein	*getProteinFromName(const std::string &name) const;
  t_promoter	*getPromoterFromName(const std::string &name) const;
  void		linkProteins(void);
  void		linkPromoters(void);
  void		makeNetwork(void);

  void		drawCurves(void);
  void		updateCurves(void);
  void		enablePlot(void);
  void		disablePlot(void);

  void		live(void);
};

#endif
