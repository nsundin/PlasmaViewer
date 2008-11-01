#include "DynText.h"


/** 
Original Code written by:
  +--------------------------------------+
  + Bob Pendleton: writer and programmer +
  + email: Bob@xxxxxxxxxxxxx             +
  + blog:  www.Stonewolf.net             +
  + web:   www.GameProgrammer.com        +
  +--------------------------------------+
	http://www.devolution.com/pipermail/sdl/2004-December/066119.html
	http://www.oreillynet.com/pub/au/1205
**/


static int powerOfTwo(int input) {
	int value = 1;
	while (value < input) {
		value <<= 1;
	}
	return value;
}


GLuint loadTextureColorKey(SDL_Surface *surface,GLfloat *texcoord, int ckr,int ckg, int ckb) {
  GLuint texture;
  int w, h;
  SDL_Surface *image;
  SDL_Rect area;
  Uint32 colorkey;

  // Use the surface width and height expanded to powers of 2 

  w = powerOfTwo(surface->w);
  h = powerOfTwo(surface->h);
  texcoord[0] = 0.0f;                    // Min X 
  texcoord[1] = 0.0f;                    // Min Y 
  texcoord[2] = (GLfloat)surface->w / w; // Max X 
  texcoord[3] = (GLfloat)surface->h / h; // Max Y 

  image = SDL_CreateRGBSurface(
                               SDL_SWSURFACE,
                               w, h,
                               32,
#if SDL_BYTEORDER == SDL_LIL_ENDIAN // OpenGL RGBA masks 
                               0x000000FF, 
                               0x0000FF00, 
                               0x00FF0000, 
                               0xFF000000
#else
                               0xFF000000,
                               0x00FF0000, 
                               0x0000FF00, 
                               0x000000FF
#endif
                               );
  if (image == NULL)
  {
    return 0;
  }

  // Set up so that colorkey pixels become transparent 

  colorkey = SDL_MapRGBA(image->format, ckr, ckg, ckb, 0);
  SDL_FillRect(image, NULL, colorkey);

  colorkey = SDL_MapRGBA(surface->format, ckr, ckg, ckb, 0);
  SDL_SetColorKey(surface, SDL_SRCCOLORKEY, colorkey);

  // Copy the surface into the GL texture image 
  area.x = 0;
  area.y = 0;
  area.w = surface->w;
  area.h = surface->h;
  SDL_BlitSurface(surface, &area, image, &area);

  // Create an OpenGL texture for the image 

  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D,
               0,
               GL_RGBA,
               w, h,
               0,
               GL_RGBA,
               GL_UNSIGNED_BYTE,
               image->pixels);

  SDL_FreeSurface(image); // No longer needed 

  return texture;
}

void DynText::loadChar(char c)
  {
    GLfloat texcoord[4];
    char letter[2] = {0, 0};

    if ((minGlyph <= c) && 
        (c <= maxGlyph) &&
        (NULL == glyphs[((int)c)].pic))
    {
      SDL_Surface *g0 = NULL;
      SDL_Surface *g1 = NULL;

      letter[0] = c;

      TTF_GlyphMetrics(ttfFont, 
                       (Uint16)c, 
                       &glyphs[((int)c)].minx, 
                       &glyphs[((int)c)].maxx, 
                       &glyphs[((int)c)].miny, 
                       &glyphs[((int)c)].maxy, 
                       &glyphs[((int)c)].advance);

      g0 = TTF_RenderText_Shaded(ttfFont, 
                                 letter, 
                                 foreground, 
                                 background);

      if (NULL != g0) {
        g1 = SDL_DisplayFormat(g0);
        SDL_FreeSurface(g0);
      }
      if (NULL != g1) {
        glyphs[((int)c)].pic = g1;
        glyphs[((int)c)].tex = loadTextureColorKey(g1, texcoord, 0, 0, 0);
        glyphs[((int)c)].texMinX = texcoord[0];
        glyphs[((int)c)].texMinY = texcoord[1];
        glyphs[((int)c)].texMaxX = texcoord[2];
        glyphs[((int)c)].texMaxY = texcoord[3];
      }
    }
  }

DynText::DynText(const char * address, int pointSize, int style, float fgRed, float fgGreen, float fgBlue, float bgRed, float bgGreen, float bgBlue):
    address(address), length(length),
    pointSize(pointSize),
    style(style),
    fgRed(fgRed), fgGreen(fgGreen), fgBlue(fgBlue),
    bgRed(bgRed), bgGreen(bgGreen), bgBlue(bgBlue),
    ttfFont(NULL)
  {
	  if (TTF_Init() < 0) {
        printf("Can't init SDL_ttf");
      }
	  initFont();
  }


DynText::~DynText() {
	TTF_Quit();
}

void DynText::initFont() {
    int i;

    ttfFont = TTF_OpenFont(address, pointSize);
    if (NULL == ttfFont) {
		printf("Can't open font file at: %s",address);
    }

    TTF_SetFontStyle(ttfFont, style);

    foreground.r = (Uint8)(255 * fgRed);
    foreground.g  = (Uint8)(255 * fgGreen);
    foreground.b = (Uint8)(255 * fgBlue);

    background.r = (Uint8)(255 * bgRed);
    background.g = (Uint8)(255 * bgGreen);
    background.b = (Uint8)(255 * bgBlue);

    height = TTF_FontHeight(ttfFont);
    ascent = TTF_FontAscent(ttfFont);
    descent = TTF_FontDescent(ttfFont);
    lineSkip = TTF_FontLineSkip(ttfFont);

    for (i = minGlyph; i <= maxGlyph; i++)
    {
      glyphs[i].pic = NULL;
      glyphs[i].tex = 0;
    }
  }

int DynText::getLineSkip() {
	return lineSkip;
}

int DynText::getHeight() {
	return height;
}

void DynText::textSize(char *text, SDL_Rect *r) {
    int maxx = 0;
    int advance = 0;

    r->x = 0;
    r->y = 0;
    r->w = 0;
    r->h = height;

    while (0 != *text)
    {
      if ((minGlyph <= *text) && (*text <= maxGlyph))
      {
        loadChar(*text);

        maxx = glyphs[((int)*text)].maxx;
        advance = glyphs[((int)*text)].advance;
        r->w += advance;
      }

      text++;
    }

    r->w = r->w - advance + maxx;
}

void DynText::drawText(char *text, int x, int y) {
    GLfloat left, right;
    GLfloat top, bottom;
    GLfloat texMinX, texMinY;
    GLfloat texMaxX, texMaxY;

    glPushAttrib(GL_ALL_ATTRIB_BITS);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    while (0 != *text)
    {
      if ((minGlyph <= *text) && (*text <= maxGlyph))
      {
        loadChar(*text);

        texMinX = glyphs[((int)*text)].texMinX;
        texMinY = glyphs[((int)*text)].texMinY;
        texMaxX = glyphs[((int)*text)].texMaxX;
        texMaxY = glyphs[((int)*text)].texMaxY;

        left   = x;
        right  = x + glyphs[((int)*text)].pic->w;
        top    = y;
        bottom = y + glyphs[((int)*text)].pic->h;

        glBindTexture(GL_TEXTURE_2D, glyphs[((int)*text)].tex);

        glBegin(GL_TRIANGLE_STRIP);

        glTexCoord2f(texMinX, texMinY); glVertex2f( left,    top);
        glTexCoord2f(texMaxX, texMinY); glVertex2f(right,    top);
        glTexCoord2f(texMinX, texMaxY); glVertex2f( left, bottom);
        glTexCoord2f(texMaxX, texMaxY); glVertex2f(right, bottom);

        glEnd();

        x += glyphs[((int)*text)].advance;
      }

      text++;
    }

    glPopAttrib();
}

