
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include "Cell.hpp"
#include "tinyxml2.h"
#include "tools.hpp"

Cell::Cell(bool isPlot) :
  _live(false),
  _time(0.f),
  _plot(NULL),
  _isPlot(isPlot)
{
}

Cell::~Cell()
{
  std::vector<t_protein*>::iterator	it = _proteins.begin();
  std::vector<t_protein*>::iterator	end = _proteins.end();
  std::vector<t_promoter*>::iterator	pit = _promoters.begin();
  std::vector<t_promoter*>::iterator	pend = _promoters.end();

  delete _plot;
  for (; it != end; ++it)
    {
      delete (*it)->curve;
      delete (*it);
    }
  for (; pit != pend; ++pit)
    delete (*pit);
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

bool		Cell::LoadFromFile(const std::string &path)
{
  tinyxml2::XMLDocument	doc;

  doc.LoadFile(path.c_str());
  loadPromoters(doc);
  std:: cout << "[\033[1;32m+\033[0m] Promoters Loaded" << std::endl;
  loadProteins(doc);
  std:: cout << "[\033[1;32m+\033[0m] Proteins Loaded" << std::endl;
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
  _plot = new Plot(-0.1, 5, -0.1, 1, 600, 600, "Systems Biology simulation");
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

void		Cell::live(void)
{
  _live = true;
  while (_live)
    {
      if (_isPlot && !_plot)
	this->enablePlot();


      this->applyDegradation();
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
