
#include <algorithm>
#include "plot.hpp"

static bool		comparePairX(std::pair<float, float> const &i,
			std::pair<float, float> const &j)
{return (i.first < j.first);}

static bool		comparePairY(std::pair<float, float> const &i,
			std::pair<float, float> const &j)
{return (i.second < j.second);}


Curve::Curve(unsigned color) :
  _color(color)
{
}

void		Curve::addPoint(float x, float y)
{
  _points.push_back(std::make_pair(x, y));
}

void		Curve::buildCurveFromFunction(float (*func)(float), float minX,
					      float maxX, float step)
{
  for (; minX <= maxX; minX += step)
    this->addPoint(minX, func(minX));
}

void		Curve::sortPointsByX(void)
{
  std::sort(_points.begin(), _points.end(), comparePairX);  
}

void		Curve::sortPointsByY(void)
{
  std::sort(_points.begin(), _points.end(), comparePairY);  
}

std::pair<float, float> const	&Curve::getPoint(unsigned id) const
{
  return _points[id];
}

unsigned	Curve::getNbPoints(void) const
{
  return _points.size();
}

unsigned	Curve::getColor(void) const
{
  return _color;
}

void		Curve::setColor(unsigned color)
{
  _color = color;
}


Plot::Plot(float minx, float maxx, float  miny, float maxy,
	   unsigned x, unsigned y, std::string const &title) :
  Graphic(x, y, title),
  _minX(minx),
  _maxX(maxx),
  _minY(miny),
  _maxY(maxy)
{
  _scaleX = (float)_x / (_maxX - _minX);
  _scaleY = (float)_y / (_maxY - _minY);
}

Plot::~Plot()
{
  
}

void		Plot::drawRules(void)
{
  float		x;
  float		y;

  y = ABS(_scaleY * (ABS(_maxY - _minY) - ABS(_minY)));
  for (x = 0 ; x < _x; ++x)
    this->PutPixel(x, y, 0xFF0000);
  x = ABS(_scaleX * _minX);
  for (y = 0; y < _y; ++y)
    this->PutPixel(x, y, 0x00FF00);
  this->update();
}

void		Plot::drawCurve(Curve &curve)
{
  unsigned i = 0;
  unsigned size = curve.getNbPoints();

  float startx = ABS(_minX);
  float starty = ABS(_minY);

  for (; i < size ; ++i)
    this->SafePutPixel(((curve.getPoint(i)).first + startx) * _scaleX,
		   (_y - (((curve.getPoint(i)).second + starty) * _scaleY)),
		       curve.getColor());
  this->update();
}

void		Plot::drawCurves(void)
{
  std::vector<Curve *>::const_iterator	it = _curves.begin();
  std::vector<Curve *>::const_iterator	end = _curves.end();

  for (; it != end; ++it)
    if (*it)
      this->drawCurve(*(*it));
}

void		Plot::addCurve(Curve *curve)
{
  _curves.push_back(curve);
}

void		Plot::clearCurves(void)
{
  _curves.clear();
}

void		Plot::deleteCurve(Curve *ptr)
{
  std::vector<Curve*>::iterator	it = _curves.begin();
  std::vector<Curve*>::iterator	end = _curves.end();

  for (; it != end; ++it)
    if ((*it) == ptr)
      {
	_curves.erase(it);
	return ;
      }
}
