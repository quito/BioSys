
#ifndef BOOLPARSER_HPP_
# define BOOLPARSER_HPP_

# include <string>
# include <list>
# include <utility>

enum symbols
  {
    opOR,
    opAND,
    opNOT,
    L_PAR,
    R_PAR,
    B_TRUE,
    B_FALSE,
    CHAR,
    END
  };

class BoolNode
{
public:
  virtual ~BoolNode(){}

  enum symbols	type;
  std::string	str;
};

class Unop : public BoolNode
{
public:
  BoolNode	*op1;
};

class Binop : public BoolNode
{
public:
  BoolNode	*op1;
  BoolNode	*op2;
};

class BoolParser
{
private:
  std::list<std::pair<enum symbols, char> > _tokenList;
  std::list<std::pair<enum symbols, char> > _outList;
  unsigned	_counter;

public:

  BoolParser();
  ~BoolParser();

  static enum symbols	getSymbol(char c);
  void			LexStr(const std::string &str, std::list<std::pair<enum symbols, char> > &tokenList);

  BoolNode		*parseORExpr(void);
  BoolNode		*parseAndExpr(void);
  BoolNode		*parseParExpr(void);
  BoolNode		*parseNot(void);
  BoolNode		*parseOperand(void);
  BoolNode		*parseChar(void);
  BoolNode		*parseBool(void);

  void			popToken(void);
  void			restoreToken(void);
  void			restoreToken(unsigned id);
  void			resetParser(void);

  BoolNode	*parseStr(const std::string &str);

  static void	readTree(BoolNode *node);
};

#endif
