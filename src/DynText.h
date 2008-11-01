#ifndef __FONT_H__
#define __FONT_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_opengl.h>


GLuint loadTextureColorKey(SDL_Surface *surface,GLfloat *texcoord, int ckr,int ckg, int ckb);
static int powerOfTwo(int input);

class DynText
{
private:
  static const int minGlyph = ' ';
  static const int maxGlyph = 126;

  typedef struct
  {
    int minx, maxx;
    int miny, maxy;
    int advance;
    SDL_Surface *pic;
    GLuint tex;
    GLfloat texMinX, texMinY;
    GLfloat texMaxX, texMaxY;
  } glyph;

  int height;
  int ascent;
  int descent;
  int lineSkip;
  glyph glyphs[maxGlyph + 1];
  const char * address;
  int length;
  int pointSize;
  int style;
  float fgRed, fgGreen, fgBlue;
  float bgRed, bgGreen, bgBlue;
  TTF_Font *ttfFont;
  SDL_Color foreground;
  SDL_Color background;
  void loadChar(char c);

public:
	DynText(const char * address, int pointSize, int style, float fgRed, float fgGreen, float fgBlue, float bgRed, float bgGreen, float bgBlue);
	~DynText();
	void initFont();
	int getLineSkip();
	int getHeight();
	void textSize(char *text, SDL_Rect *r);
	void drawText(char *text, int x, int y);
};
#endif
