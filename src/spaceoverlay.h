#if !defined(__SPACE_OVERLAY_INCLUDED)
#define __SPACE_OVERLAY_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "space.h"

void font_init();
void overlaySetTextColor(spaceColor coleur);

void writeoverlay(char* str);
void overlayBox(char* title, int x, int y, int width, int textlines, spaceColor color, spaceColor scolor);
void drawbar(int bar_x, int bar_y, int bar_width, int bar_height, SPfloat var, SPfloat varmax, spaceColor barcolor);

#endif