
#include <iostream>
#include <fstream>
#include <string>
#include "Cell.hpp"
#include "tinyxml2.h"

Cell::Cell()
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

bool		Cell::loadProteins(tinyxml2::XMLDocument &xml)
{
  
}

bool		Cell::LoadFromFile(const std::string &path)
{
  tinyxml2::XMLDocument	doc;

  doc.LoadFile(path.c_str());
  loadPromoters(doc);

  return true;
}
