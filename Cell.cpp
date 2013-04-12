
#include <iostream>
#include <fstream>
#include <string>
#include "Cell.hpp"
#include "tinyxml2.h"

Cell::Cell()
{
}

Cell::~Cell()
{
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
      std::cout << prom->FirstChildElement("name")->GetText() << std::endl;
      promoter = new t_promoter;
      promoter->name = prom->FirstChildElement("name")->GetText();
      link = prom->FirstChildElement("links");
      if (link)
	{
	  name = link->FirstChildElement("name");
	  while (name)
	    {
	      promoter->linksName.push_back(name->ToElement()->GetText());
	      std::cout << "-> " << name->ToElement()->GetText() << std::endl;
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
	{
	  std::cout << "-> " << name->ToElement()->GetText() << " : Activator" << std::endl;
	  protein->linksName.push_back(std::make_pair(name->ToElement()->GetText(), Link::ACTIVATOR));
	}
      else if (!strcmp(linkType->ToElement()->GetText(), "Repressor"))
	{
	  std::cout << "-> " << name->ToElement()->GetText() << " : Repressor" << std::endl;
	  protein->linksName.push_back(std::make_pair(name->ToElement()->GetText(), Link::REPRESSOR));
	}
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
      std::cout << protein->name << std::endl;
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
  loadProteins(doc);
  makeNetwork();
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
