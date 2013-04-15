#include <iostream>
#include <cmath>
#include "graphic.hpp"
#include "plot.hpp"
#include "Cell.hpp"
#include "boolParser.hpp"

float func(float x)
{
  return (tanhf(x));
}

void plot()
{
  Plot p(-5, 5, -5, 5, 600, 600, "lotka volterra");
  Curve c1(0x00FF00);
  Curve	c2(0xFF0000);
  Curve	c3(0x0000FF);
  float x = -10;

  p.addCurve(&c1);
  p.addCurve(&c2);
  p.addCurve(&c3);
  c3.buildCurveFromFunction(logf, 0, 10, 0.001);
  while (x < 10)
    {
      c1.addPoint(x, func(x));
      c2.addPoint(x, expf(x));
      x += 0.001;
    }
  
  p.drawRules();
  p.drawCurves();
  while (1);  
}

void graphic()
{
  Graphic g(600, 600, "New Window");

  g.PutPixel(300, 300, 0x00FF00);
  g.drawSquare(0, 0, 0x00FF00, 50);
  g.drawCircle(400, 100, 50, 0xFF00FF);
  g.fillCircle(400, 200, 50, 0x00FFFF);
  g.update();
  while (1);
}

int main()
{
  // plot();
  // Cell c(true);
  // c.LoadFromFile("./data/network");
  // c.live();
  BoolParser bp;

  bp.parseStr("(prot1|prot2)&!(prot3)");
  return 0;
}
