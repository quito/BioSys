
#ifndef CELL_HPP_
# define CELL_HPP_

# include <vector>
# include "tinyxml2.h"
# include "plot.hpp"
# include "boolParser.hpp"

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
  float				tmpConcentration;
  float				degradationRate;
  unsigned			curveColor;
  Curve				*curve;
  std::vector<std::pair<std::string, Link::Type> >	linksName;
  std::vector<std::pair<struct s_promoter *, Link::Type> > promoters;
}		t_protein;

typedef struct	s_promoter
{
  std::string				name;
  float					productionSpeed;
  std::vector<std::string>		linksName;
  std::vector<BoolNode*>		formulas;
  std::vector<struct s_protein *>	proteins;
}		t_promoter;
namespace Reaction
{
  enum Type
    {
      DEGRADATION,
      PRODUCTION
    };
}
typedef struct	s_reaction
{
  Reaction::Type	type;
  t_protein		*protein;
  t_promoter		*promoter;
  float			rate;
}		t_reaction;



class Cell
{
private:
  
  std::vector<t_protein*>	_proteins;
  std::vector<t_promoter*>	_promoters;
  std::vector<t_reaction*>	_reactions;
  bool				_live;
  float				_time;

  Plot				*_plot;
  bool				_isPlot;
  BoolParser			_parser;

public:
  
  Cell(bool isPlot = false);
  ~Cell();

  void		deleteVectors(void);
  bool		LoadFromFile(const std::string &path);
  bool		loadReactions(tinyxml2::XMLDocument &xml);
  void		loadFormulas(tinyxml2::XMLDocument &xml, tinyxml2::XMLNode *prom,
				   t_promoter *promoter);
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

  void		applyDegradation(void);
  void		applyCalculus(void);
  float		executeTreeRec(BoolNode *node);
  void		executeTree(BoolNode *tree, t_promoter *prom);
  void		applyPromoterProduction(t_promoter *prom);
  void		applyProduction(void);
  void		live(void);

  void		applyDegradation(t_protein *protein);
  void		applyProduction(t_promoter *promoter);
  void		applyReaction(unsigned id);
  float		*setGillespiRate(unsigned &sizeRatesTab);
  int		binarySearch(float sortedArray[], int  first, int  last, float key);
  unsigned	applyGilespi(float &dt, float *rates, unsigned sizeRatesTab);
  void		liveGillespis(void);
};

#endif
