
#ifndef PLOT_HPP_
# define PLOT_HPP_


# include <utility>
# include <vector>

# include "graphic.hpp"

# define ABS(x) (x<0?(-x):(x))

/*
** Curve is a class that represent a vector of 2D points
*/
class Curve
{

private:

  std::vector<std::pair<float, float> >	_points;
  unsigned				_color;

public:

  Curve(unsigned color = 0xFFFFFF);
  ~Curve(){}

  void	addPoint(float x, float y);
  void	buildCurveFromFunction(float (*func)(float), float minX,
			       float maxX, float step = 0.01);
  void	sortPointsByX(void);
  void	sortPointsByY(void);
  std::pair<float, float> const	&getPoint(unsigned id) const;
  unsigned getNbPoints(void) const;
  unsigned getColor(void) const;
  void	setColor(unsigned color);
};

/*
** Plot is A class that permit to draw graphics.
** Plot class take Curve object for drawing it.
*/
class Plot : public Graphic
{

private:

  // std::vector<std::pair<float, float> >	_points;
  std::vector<Curve *>			_curves;
  float					_minX;
  float					_maxX;
  float					_minY;
  float					_maxY;
  float					_scaleX;
  float					_scaleY;

public:

  Plot(float minx, float maxx, float  miny, float maxy,
       unsigned x = 600, unsigned y = 600, std::string const &title = "");
  ~Plot();

  void	drawRules(void);
  void	drawCurve(Curve &curve);
  void	drawCurves(void);
  void	addCurve(Curve *curve);
  void	clearCurves(void);
  void	deleteCurve(Curve *ptr);
};

#endif
