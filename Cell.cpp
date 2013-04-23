
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include "Cell.hpp"
#include "tinyxml2.h"
#include "tools.hpp"
#include "boolParser.hpp"

Cell::Cell(bool isPlot) :
  _live(false),
  _time(0.f),
  _plot(NULL),
  _isPlot(isPlot)
{
  srand(time(NULL));
}

Cell::~Cell()
{
  this->deleteVectors();
}

void		Cell::deleteVectors(void)
{
  std::vector<t_protein*>::iterator	it = _proteins.begin();
  std::vector<t_protein*>::iterator	end = _proteins.end();
  std::vector<t_promoter*>::iterator	pit = _promoters.begin();
  std::vector<t_promoter*>::iterator	pend = _promoters.end();
  std::vector<t_reaction*>::iterator	rit = _reactions.begin();
  std::vector<t_reaction*>::iterator	rend = _reactions.end();

  for (; it != end; ++it)
    {
      delete (*it)->curve;
      delete (*it);
    }
  for (; pit != pend; ++pit)
    delete (*pit);
  for (; rit != rend; ++rit)
    delete (*rit);
}

void		Cell::loadFormulas(tinyxml2::XMLDocument &xml,
				   tinyxml2::XMLNode *prom,
				   t_promoter *promoter)
{
  tinyxml2::XMLNode *formulaStr;
  BoolNode		*ast;

  formulaStr = prom->FirstChildElement("formula");
  while (formulaStr)
  {
    if (formulaStr->ToElement()->GetText()
	&& (ast = _parser.parseStr(formulaStr->ToElement()->GetText())))
      promoter->formulas.push_back(ast);
    xml.DeleteNode(formulaStr);
    formulaStr = xml.FirstChildElement("formula");
  }
}

bool		Cell::loadPromoters(tinyxml2::XMLDocument &xml)
{
  tinyxml2::XMLNode *prom;
  tinyxml2::XMLNode *link;
  tinyxml2::XMLNode *name;
  t_promoter		*promoter;

  prom = xml.FirstChildElement("promoter");
  while (prom)
    {
      promoter = new t_promoter;
      promoter->name = prom->FirstChildElement("name")->GetText();
      promoter->productionSpeed = atof(prom->FirstChildElement("productionSpeed")->GetText());
      this->loadFormulas(xml, prom, promoter);
      std:: cout << "[\033[1;32m+\033[0m] " << promoter->name << " formulas parsed" << std::endl;
      link = prom->FirstChildElement("links");
      if (link)
	{
	  name = link->FirstChildElement("name");
	  while (name)
	    {
	      promoter->linksName.push_back(name->ToElement()->GetText());
	      xml.DeleteNode(name);
	      name = link->FirstChildElement("name");
	    }
	}
      _promoters.push_back(promoter);
      xml.DeleteNode(prom);
      prom = xml.FirstChildElement("promoter");
    }
  return true;
}

void		Cell::pushProtein(t_protein *protein, tinyxml2::XMLNode *name,
				  tinyxml2::XMLNode *linkType)
{
  if (name && linkType)
    {
      if (!strcmp(linkType->ToElement()->GetText(), "Activator"))
	protein->linksName.push_back(std::make_pair(name->ToElement()->GetText(), Link::ACTIVATOR));
      else if (!strcmp(linkType->ToElement()->GetText(), "Repressor"))
	protein->linksName.push_back(std::make_pair(name->ToElement()->GetText(), Link::REPRESSOR));
    }
}

bool		Cell::loadProteins(tinyxml2::XMLDocument &xml)
{
  tinyxml2::XMLNode *prot;
  tinyxml2::XMLNode *link;
  tinyxml2::XMLNode *name;
  tinyxml2::XMLNode *linkType;
  t_protein		*protein;

  prot = xml.FirstChildElement("protein");
  while (prot)
    {
      protein = new t_protein;
      protein->name = prot->FirstChildElement("name")->GetText();
      protein->concentration = atof(prot->FirstChildElement("concentration")->GetText());
      protein->degradationRate = atof(prot->FirstChildElement("degradationRate")->GetText());
      protein->curveColor = Tools::ahtoui(prot->FirstChildElement("curveColor")->GetText());
      protein->curve = NULL;
      protein->tmpConcentration = protein->concentration;
      if (!strcmp(prot->FirstChildElement("enableCurve")->GetText(), "True"))
	protein->curve = new Curve(protein->curveColor);
      link = prot->FirstChildElement("link");
      while (link)
	{
	  name = link->FirstChildElement("name");
	  linkType = link->FirstChildElement("type");
	  if (name && linkType)
	    this->pushProtein(protein, name, linkType);
	  xml.DeleteNode(link);
	  link = prot->FirstChildElement("link");
	}
      _proteins.push_back(protein);
      xml.DeleteNode(prot);
      prot = xml.FirstChildElement("protein");
    }
  return true;  
}

// don't forget : make sum of reaction rate of 
bool			Cell::loadReactions(tinyxml2::XMLDocument &xml)
{
  tinyxml2::XMLNode	*nodeReaction;
  tinyxml2::XMLNode	*nodeReactionType;
  tinyxml2::XMLNode	*nodePromoter;
  tinyxml2::XMLNode	*nodeProtein;
  tinyxml2::XMLNode	*nodeValue;
  t_reaction		*reaction;

  nodeReaction = xml.FirstChildElement("Reaction");
  while (nodeReaction)
    {
      nodeReactionType = nodeReaction->FirstChildElement("type");
      nodePromoter = nodeReaction->FirstChildElement("promoter");
      nodeProtein = nodeReaction->FirstChildElement("protein");
      nodeValue = nodeReaction->FirstChildElement("value");
      if (nodeReactionType && nodeValue)
	{
	  reaction = new t_reaction;
	  reaction->promoter = NULL;
	  reaction->protein = NULL;

	  if (!strcmp(nodeReactionType->ToElement()->GetText(), "degradation"))
	    reaction->type = Reaction::DEGRADATION;
	  else if (!strcmp(nodeReactionType->ToElement()->GetText(), "production"))
	    reaction->type = Reaction::PRODUCTION;
	  else
	    {
	      std::cerr << "Bad type of reaction in xml configuration file";
	      delete reaction;
	      return false;
	    }

	  if (reaction->type == Reaction::PRODUCTION)
	    {
	      reaction->promoter = this->getPromoterFromName(nodePromoter->ToElement()->GetText());
	      if (!(reaction->promoter))
		{
		  std::cerr << "Bad promoter name in XML file" << std::endl;
		  delete reaction;
		  return false;
		}
	    }
	  else if (reaction->type == Reaction::DEGRADATION)
	    {
	    reaction->protein =  this->getProteinFromName(nodeProtein->ToElement()->GetText());
	    if (!(reaction->protein))
	      {
		std::cerr << "Bad protein name in XML file";
		delete reaction;
		return false;	      
	      }
	    }

	  reaction->rate = atof(nodeValue->ToElement()->GetText());
	  _reactions.push_back(reaction);
	}
      else
	{
	  std::cerr << "Bad reaction format in xml configuration file" << std::endl;
	  return false;
	}
      xml.DeleteNode(nodeReaction);
      nodeReaction = xml.FirstChildElement("Reaction");
    }  
  return true;
}

bool		Cell::LoadFromFile(const std::string &path)
{
  tinyxml2::XMLDocument	doc;

  this->deleteVectors();
  _proteins.clear();
  _promoters.clear();
  _reactions.clear();
  doc.LoadFile(path.c_str());

  loadPromoters(doc);
  std:: cout << "[\033[1;32m+\033[0m] Promoters Loaded" << std::endl;
  loadProteins(doc);
  std:: cout << "[\033[1;32m+\033[0m] Proteins Loaded" << std::endl;
  loadReactions(doc);
  std:: cout << "[\033[1;32m+\033[0m] Interraction Loaded" << std::endl;
  makeNetwork();
  std:: cout << "[\033[1;32m+\033[0m] Network Linked" << std::endl;
  return true;
}

t_protein	*Cell::getProteinFromName(const std::string &name) const
{
  std::vector<t_protein*>::const_iterator	it = _proteins.begin();
  std::vector<t_protein*>::const_iterator	end = _proteins.end();

  for (; it != end; ++it)
    {
      if ((*it) && !name.compare((*it)->name))
	return (*it);
    }
  return (NULL);
}

t_promoter	*Cell::getPromoterFromName(const std::string &name) const
{
  std::vector<t_promoter*>::const_iterator	it = _promoters.begin();
  std::vector<t_promoter*>::const_iterator	end = _promoters.end();

  for (; it != end; ++it)
    {
      if ((*it) && !name.compare((*it)->name))
	return (*it);
    }
  return (NULL);
}

void		Cell::linkProteins(void)
{
  std::vector<t_protein*>::iterator	it = _proteins.begin();
  std::vector<t_protein*>::iterator	end = _proteins.end();
  std::vector<std::pair<std::string, Link::Type> >::iterator	lit;
  std::vector<std::pair<std::string, Link::Type> >::iterator	lend;
  t_promoter		*prom;

  for (; it != end; ++it)
    {
      lit = (*it)->linksName.begin();
      lend = (*it)->linksName.end();
      for (; lit != lend; ++lit)
	{
	  prom = this->getPromoterFromName((*lit).first);
	  (*it)->promoters.push_back(std::make_pair(prom, (*lit).second));
	}
    }  
}

void		Cell::linkPromoters(void)
{
  std::vector<t_promoter*>::iterator	it = _promoters.begin();
  std::vector<t_promoter*>::iterator	end = _promoters.end();
  std::vector<std::string>::iterator	lit;
  std::vector<std::string>::iterator	lend;
  t_protein		*prot;

  for (; it != end; ++it)
    {
      lit = (*it)->linksName.begin();
      lend = (*it)->linksName.end();
      for (; lit != lend; ++lit)
	{
	  prot = this->getProteinFromName(*lit);
	  (*it)->proteins.push_back(prot);
	}
    }  
}

void		Cell::makeNetwork(void)
{
  this->linkProteins();
  this->linkPromoters();
}


void		Cell::enablePlot(void)
{
  std::vector<t_protein*>::iterator	it = _proteins.begin();
  std::vector<t_protein*>::iterator	end = _proteins.end();

  if (_plot)
    delete _plot;
  _plot = new Plot(-0.1, 1000, -0.1, 1, 600, 600, "Systems Biology simulation");
  _isPlot = true;
  _plot->drawRules();
  for (; it != end; ++it)
    _plot->addCurve((*it)->curve);
}

void		Cell::disablePlot(void)
{
  if (_plot)
    {
      delete _plot;
      _plot = NULL;
      _isPlot = false;
    }
}

void		Cell::drawCurves(void)
{
  if (_plot)
    _plot->drawCurves();
}

void		Cell::updateCurves(void)
{
  std::vector<t_protein*>::iterator	it = _proteins.begin();
  std::vector<t_protein*>::iterator	end = _proteins.end();

  for (; it != end; ++it)
    if (*it && (*it)->curve)
      (*it)->curve->addPoint(_time, (*it)->concentration);
}

void		Cell::applyDegradation(void)
{
  std::vector<t_protein*>::iterator	it = _proteins.begin();
  std::vector<t_protein*>::iterator	end = _proteins.end();

  for (; it != end; ++it)
    if (*it)
      (*it)->tmpConcentration =  (*it)->concentration * (1.f - (*it)->degradationRate);
}

void		Cell::applyCalculus(void)
{
  std::vector<t_protein*>::iterator	it = _proteins.begin();
  std::vector<t_protein*>::iterator	end = _proteins.end();

  for (; it != end; ++it)
    if (*it)
      (*it)->concentration = (*it)->tmpConcentration < 0 ? 0 : (*it)->tmpConcentration;
}

float		Cell::executeTreeRec(BoolNode *node)
{
  Binop		*binop;
  Unop		*unop;

  if (!node)
    return 0;
  if ((binop = dynamic_cast<Binop *>(node)))
    {
      if (binop->type == opOR)
	return this->executeTreeRec(binop->op1) + this->executeTreeRec(binop->op2);
      else if (binop->type == opAND)
	{
	  float		result1;
	  float		result2;
	  result1 = this->executeTreeRec(binop->op1);
	  result2 = this->executeTreeRec(binop->op2);
	  // return result1 * result2;
	  return result1 < result2 ? result1 : result2;
	}
      return 0.1;
    }
  else if ((unop = dynamic_cast<Unop*>(node)))
    {
      if (node->type == opNOT)
	return (- this->executeTreeRec(unop->op1));
      return 0;
    }
  else
    {
      if (node->type == CHAR)
	return this->getProteinFromName(node->str)->concentration;
      return atoi(node->str.c_str());
    }
}

void		Cell::executeTree(BoolNode *tree, t_promoter *prom)
{
  std::vector<t_protein *>::iterator	it = prom->proteins.begin();
  std::vector<t_protein *>::iterator	end = prom->proteins.end();
  float					production = this->executeTreeRec(tree);

  // std::cout << "production de " << production * prom->productionSpeed << std::endl;
  for (; it != end; ++it)
    (*it)->tmpConcentration += production * prom->productionSpeed;
}

void		Cell::applyPromoterProduction(t_promoter *prom)
{
  if (!prom)
    return;

  std::vector<BoolNode*>::const_iterator it = prom->formulas.begin();
  std::vector<BoolNode*>::const_iterator end = prom->formulas.end();

  for (; it != end; ++it)
    this->executeTree((*it), prom);
}

void		Cell::applyProduction(void)
{
  std::vector<t_promoter*>::iterator	it = _promoters.begin();
  std::vector<t_promoter*>::iterator	end = _promoters.end();

  for (; it != end; ++it)
    this->applyPromoterProduction((*it));
}

void		Cell::live(void)
{
  _live = true;
  while (_live)
    {
      if (_isPlot && !_plot)
	this->enablePlot();


      this->applyDegradation();
      this->applyProduction();
      this->applyCalculus();
      if (_isPlot && _plot)
	{
	  this->updateCurves();
	  this->drawCurves();
	}
      _time += 0.001;
      usleep(1);
    }
}


int		Cell::binarySearch(float sortedArray[], int  first, int  last, float key)
{
  int llast = last ;
  int ffirst = first;
  int mid;

  while (ffirst <= llast)
    {
      mid = (int) (ffirst + llast) / 2;
      if (key > sortedArray[mid])
        ffirst = mid + 1;
      else if (key < sortedArray[mid])
        llast = mid - 1;
      else
        return mid;
    }
  return llast;
}

unsigned	Cell::applyGilespi(float &dt, float *rates, unsigned sizeRatesTab)
{
  float		R;
  float		u;
  int		id;
  float		*rate_tot = new float[sizeRatesTab + 1];

  rate_tot[0] = 0.f;
  for (unsigned i = 0; i < sizeRatesTab; i++)
    rate_tot[i+1] = rate_tot[i] + rates[i];
  R = rate_tot[sizeRatesTab];
  dt = -log(GETFRAND(100))/R; // U
  // std::cout << "dt:" << dt << " other : " << GETFRAND(100) << std::endl;
  u = ((float)(rand() % (int)(rate_tot[sizeRatesTab] * 100))) / 100.f; // U'
  // std::cout << "u' = " << u << std::endl;
  id = binarySearch(rate_tot, 0, sizeRatesTab, u);
  delete [] rate_tot;
  // std::cout << "id=" << id << std::endl;
  return id;
}

float		*Cell::setGillespiRate(unsigned &sizeRatesTab)
{
  unsigned				i;
  float					*rates = NULL;
  std::vector<t_reaction*>::iterator	it = _reactions.begin();
  std::vector<t_reaction*>::iterator	end = _reactions.end();


  rates = new float[_reactions.size()];
  sizeRatesTab = _reactions.size();
  i = 0;
  for (; it != end; ++it)
    if ((*it))
      rates[i++] = (*it)->rate;
  return rates;
}

void		Cell::applyDegradation(t_protein *protein)
{
  if (protein)
    protein->concentration *= 1.f - protein->degradationRate;
}

void		Cell::applyProduction(t_promoter *promoter)
{
  
}

void		Cell::applyReaction(unsigned id)
{
  if (id < _reactions.size() && _reactions[id])
    {
      if (_reactions[id]->type == Reaction::PRODUCTION)
	this->applyProduction(_reactions[id]->promoter);
      else if (_reactions[id]->type == Reaction::DEGRADATION)
	this->applyDegradation(_reactions[id]->protein);
    }
  else
    std::cerr << "[-] Bad reaction ID"<< std::endl;
}

void		Cell::liveGillespis(void)
{
  float		dt;
  float		*rates;
  unsigned	sizeRatesTab;
  unsigned	idReaction;
  
  //tmp
  _isPlot = true;

  _live = true;
  rates = this->setGillespiRate(sizeRatesTab);
  while (_live)
    {
      if (_isPlot && !_plot)
	this->enablePlot();

      idReaction = this->applyGilespi(dt, rates, sizeRatesTab);
      this->applyReaction(idReaction);

      if (_isPlot && _plot)
	{
	  this->updateCurves();
	  this->drawCurves();
	}
      _time += dt/10;
      // std::cout << _time << std::endl;
      // usleep(1);
    }
}
