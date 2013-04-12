
#ifndef GRAPHIC_HPP_
# define GRAPHIC_HPP_

# include <SDL/SDL.h>
# include <string>
# include <math.h>

typedef union
{
  unsigned	color;
  unsigned char	tab[4];
}		u_color;

class Graphic
{
protected:
  unsigned	_x;
  unsigned	_y;
  SDL_Surface	*_screen;

public:

  Graphic(unsigned x, unsigned y, std::string const &title = ""):
    _x(x),
    _y(y),
    _screen(NULL)
  {
    SDL_Init(SDL_INIT_VIDEO);
    _screen = SDL_SetVideoMode(_x, _y, 32, SDL_HWSURFACE);
    SDL_WM_SetCaption(title.c_str(), NULL);
    SDL_FillRect(_screen, NULL, SDL_MapRGB(_screen->format, 0, 0, 0));
  }
  
  ~Graphic()
  {
    SDL_Quit();
  }

  void update(void)
  {
    SDL_Flip(_screen);
  }

  void PutPixel(int x, int y, Uint32 px)
  {
    int		bpp = _screen->format->BytesPerPixel;
    Uint8	*p = (Uint8*)_screen->pixels + y * _screen->pitch + x * bpp;

    switch (bpp)
      {
      case 1: *p = px; break;
      case 2: *(Uint16 *)p = px; break;
      case 4: *(Uint32 *)p = px; break;
      case 3:
	if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
	  {
	    p[0] = (px >> 16) & 0xff;
	    p[1] = (px >> 8) & 0xff;
	    p[2] = px & 0xff;
	  }
	else
	  {
	    p[0] = px & 0xff;
	    p[1] = (px >> 8) & 0xff;
	    p[2] = (px >> 16) & 0xff;
	  }
      }
  }

  void SafePutPixel(int x, int y, Uint32 px)
  {
    if (_screen && x >= 0 && x < _screen->w && y >= 0 && y < _screen->h)
      this->PutPixel(x, y, px);
  }

  void	drawSquare(int x, int y, unsigned color, unsigned size)
  {
    SDL_Rect rect = {x, y, size, size};
    SDL_FillRect(_screen, &rect, color);
  }

  void drawLine(int x1,int y1, int x2,int y2, unsigned color)
  {
    unsigned long couleur = color;
    int x,y;
    int Dx,Dy;
    int xincr,yincr;
    int erreur;
    int i;
 
    Dx = abs(x2-x1);
    Dy = abs(y2-y1);
    if(x1<x2)
      xincr = 1;
    else
      xincr = -1;
    if(y1<y2)
      yincr = 1;
    else
      yincr = -1;
 
    x = x1;
    y = y1;
    if(Dx>Dy)
      {
	erreur = Dx/2;
	for(i=0;i<Dx;i++)
	  {
	    x += xincr;
	    erreur += Dy;
	    if(erreur>Dx)
	      {
		erreur -= Dx;
		y += yincr;
	      }
	    this->PutPixel(x, y,couleur);
	  }
      }
    else
      {
	erreur = Dy/2;
	for(i=0;i<Dy;i++)
	  {
	    y += yincr;
	    erreur += Dx;
 
	    if(erreur>Dy)
	      {
		erreur -= Dy;
		x += xincr;
	      }
	    this->PutPixel(x, y, couleur);
	  }
      }
  }

  void drawCircle(int n_cx, int n_cy, int radius, Uint32 pixel)
  {
    double error = (double)-radius;
    double x = (double)radius -0.5;
    double y = (double)0.5;
    double cx = n_cx - 0.5;
    double cy = n_cy - 0.5;
 
    while (x >= y)
      {
	PutPixel((int)(cx + x), (int)(cy + y), pixel);
	PutPixel((int)(cx + y), (int)(cy + x), pixel);
	if (x != 0)
	  {
	    PutPixel((int)(cx - x), (int)(cy + y), pixel);
	    PutPixel((int)(cx + y), (int)(cy - x), pixel);
	  }
	if (y != 0)
	  {
	    PutPixel((int)(cx + x), (int)(cy - y), pixel);
	    PutPixel((int)(cx - y), (int)(cy + x), pixel);
	  }
	if (x != 0 && y != 0)
	  {
	    PutPixel((int)(cx - x), (int)(cy - y), pixel);
	    PutPixel((int)(cx - y), (int)(cy - x), pixel);
	  }
	error += y;
	++y;
	error += y;
	if (error >= 0)
	  {
	    --x;
	    error -= x;
	    error -= x;
	  }
      }
  }

  void fillCircle(int cx, int cy, int radius, Uint32 pixel)
  {
    static const int BPP = 4;
 
    double r = (double)radius;
    for (double dy = 1; dy <= r; dy += 1.0)
      {
        double dx = floor(sqrt((2.0 * r * dy) - (dy * dy)));
        int x = cx - dx;
        Uint8 *target_pixel_a = (Uint8 *)_screen->pixels + ((int)(cy + r - dy)) * _screen->pitch + x * BPP;
        Uint8 *target_pixel_b = (Uint8 *)_screen->pixels + ((int)(cy - r + dy)) * _screen->pitch + x * BPP;
        for (; x <= cx + dx; x++)
	  {
            *(Uint32 *)target_pixel_a = pixel;
            *(Uint32 *)target_pixel_b = pixel;
            target_pixel_a += BPP;
            target_pixel_b += BPP;
	  }
      }
  }

};

// extern Graphic g;

#endif
