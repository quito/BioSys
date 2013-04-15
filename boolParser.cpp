
#include <fstream>
#include <iostream>
#include "boolParser.hpp"

enum symbols	BoolParser::getSymbol(char c)
{
  switch (c)
    {
    case '&':
      return opAND;
      break;
    case '|':
      return opOR;
      break;
    case '(':
      return L_PAR;
      break;
    case ')':
      return R_PAR;
      break;
    case '1':
      return B_TRUE;
      break;
    case '0':
      return B_FALSE;
      break;
    case '!':
      return opNOT;
      break;
    default:
      return CHAR;
      break;
    };
}

void		BoolParser::LexStr(const std::string &str,
				  std::list<std::pair<enum symbols, char> > &tokenList)
{
  unsigned	i = 0;

  for (; str[i]; ++i)
    tokenList.push_back(std::make_pair(this->getSymbol(str[i]), str[i]));
  tokenList.push_back(std::make_pair(END, str[i]));
}


BoolParser::BoolParser() :
  _counter(0)
{
}

BoolParser::~BoolParser()
{
}

BoolNode	*BoolParser::parseORExpr(void)
{
  BoolNode	*left;
  BoolNode	*right;
  Binop		*binop;
  std::string	value;

  if ((left = this->parseAndExpr()))
    {
      if (_tokenList.front().first == opOR)
	{
	  value = _tokenList.front().second;
	  this->popToken();
	  if ((right = this->parseORExpr()))
	    {
	      binop = new Binop;
	      binop->type = opOR;
	      binop->str = value;
	      binop->op1 = left;
	      binop->op2 = right;
	      return binop;
	    }
	  this->restoreToken();
	  return NULL;
	}
      return left;
    }
  else
    return NULL;
}

BoolNode	*BoolParser::parseAndExpr(void)
{
  BoolNode	*left;
  BoolNode	*right;
  Binop		*binop;
  std::string	value;

  if ((left = this->parseParExpr()))
    {
      if (_tokenList.front().first == opAND)
	{
	  value = _tokenList.front().second;
	  this->popToken();
	  if ((right = this->parseAndExpr()))
	    {
	      binop = new Binop;
	      binop->type = opOR;
	      binop->str = value;
	      binop->op1 = left;
	      binop->op2 = right;
	      return binop;
	    }
	  this->restoreToken();
	  return NULL;
	}
      return left;
    }
  else
    return NULL;
}

BoolNode	*BoolParser::parseParExpr(void)
{
  BoolNode	*op;
  unsigned	restorePoint;
  
  if ((op = this->parseNot()))
    return op;
  if (_tokenList.front().first == L_PAR)
    {
      restorePoint = _counter;
      this->popToken();
      if ((op = this->parseORExpr()))
	{
	  if (_tokenList.front().first == R_PAR)
	    {
	      this->popToken();
	      return op;
	    }
	  this->restoreToken(restorePoint);
	  delete op;
	}
    }
  return NULL;
}

BoolNode	*BoolParser::parseNot(void)
{
  BoolNode	*op;
  Unop		*unop;
  std::string	value;
  enum symbols	type;

  if (_tokenList.front().first == opNOT)
    {
      value = _tokenList.front().second;
      type = _tokenList.front().first;
      this->popToken();
      if ((op = this->parseORExpr()))
	{
	  unop = new Unop;
	  unop->str = value;
	  unop->type = type;
	  unop->op1 = op;
	  return unop;
	}
      return NULL;
    }
  return this->parseOperand();
}

BoolNode	*BoolParser::parseOperand(void)
{
  BoolNode	*nodeChar;
  BoolNode	*nodeName = NULL;

  if ((nodeChar = this->parseBool()))
    return nodeChar;
  while ((nodeChar = this->parseChar())
	 || (nodeName && (nodeChar = this->parseBool())))
    {
      if (!nodeName)
	{
	  nodeName = new BoolNode;
	  nodeName->type = CHAR;
	}
      nodeName->str += nodeChar->str;
      delete nodeChar;
    }
  return nodeName;
}

BoolNode	*BoolParser::parseChar(void)
{
  BoolNode	*node;

  if (_tokenList.front().first == CHAR)
    {
      node = new BoolNode;
      node->type = _tokenList.front().first;
      node->str = _tokenList.front().second;
      this->popToken();
      return node;
    }
  return NULL;
}

BoolNode	*BoolParser::parseBool(void)
{
  BoolNode	*node;

  if (_tokenList.front().first == B_TRUE || _tokenList.front().first == B_FALSE)
    {
      node = new BoolNode;
      node->type = _tokenList.front().first;
      node->str = _tokenList.front().second;
      this->popToken();
      return node;
    }
  return NULL;
}

void		BoolParser::popToken(void)
{
  _outList.push_front(_tokenList.front());
  _tokenList.pop_front();
  ++_counter;
}

void		BoolParser::restoreToken(void)
{
  _tokenList.push_front(_outList.front());
  _outList.pop_front();
  --_counter;
}

void		BoolParser::restoreToken(unsigned id)
{
  while (_counter != id && _counter)
    this->restoreToken();
}

void		BoolParser::resetParser(void)
{
  _counter = 0;
  _tokenList.clear();
  _outList.clear();
}

BoolNode	*BoolParser::parseStr(const std::string &str)
{
  BoolNode	*AST;

  this->resetParser();
  this->LexStr(str, _tokenList);
  AST = this->parseORExpr();
  // std::cout << "digraph G {" << std::endl;
  // readTree(AST);
  // std::cout << "}" << std::endl;
  return AST;
}

void		BoolParser::readTree(BoolNode *node)
{
  Binop		*binop;
  Unop		*unop;
  static int i = 0;

  i++;
  if (node)
    {
      if ((binop = dynamic_cast<Binop *>(node)))
	{
	  std::cout << "\""<<  binop->str << "\"" << " -> ";
	  readTree(binop->op1);
	  std::cout << "\"" << binop->str << "\"" << " -> ";
	  readTree(binop->op2);
	}
      else if ((unop = dynamic_cast<Unop*>(node)))
	{
	  std::cout <<  "\""<< unop->str << "\"" << " -> ";
	  readTree(unop->op1);
	}
      else
	std::cout <<  "\""<< node->str << "\""<<  ";" << std::endl;
    }
}
