#include "spaceoverlay.h"
#include "glut.h"

#include "lib\BitmapTexture.h"	// texture loading functions
#include "lib\resource.h"
#include "lib\EasyFont.h"

EasyFont theFont;

int text_x = 0;
int text_y = 0;
spaceColor tcolor(1,1,1);

void font_init()
{
	// now set up font texture
	// stolen viciously from EasyText demo

	// this tells OpenGL how to handle textures; 
	// GL_MODULATE indicates that the texture should just stack on top of 
	// the underlying material's color and shading
	// So, use GL_MODULATE instead of GL_REPLACE if lighting is being used
	// other possibilities : GL_DECAL, GL_BLEND
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//enable textures 
	//glEnable(GL_TEXTURE_2D);

	// make sure we're drawing both sides of polygons
	glPolygonMode(GL_FRONT_AND_BACK, GL_POLYGON);

	// LOAD TEXTURES
	//LoadAllTextures();
	SetTextureParameters();
	
	/* // Fun lo-fi font!
	theFont.LoadFont(ID_SMALLFONT);
	theFont.SetKern(0.22,0.1,0.1);
	theFont.SetSize(14);
	*/

	// more legible, boring courier font
	theFont.LoadFont(IDB_TEXT);
	theFont.SetSize(16);
	theFont.SetKern(0.26,0.18,0.05);

}


void overlaySetTextColor(spaceColor coleur)
{
	theFont.SetColor(coleur.r, coleur.g, coleur.b);
}

void drawbar(int bar_x, int bar_y, int bar_width, int bar_height, SPfloat var, SPfloat varmax, spaceColor barcolor)
{	
	// how much the bar is filled
	int bar_fill = (var / varmax) * bar_width;
	
	// target info display
	glBegin(GL_QUADS);
		// empty bar
		glColor3f(0.2, 0.2, 0.2);		// darker
		glVertex2i(bar_x, bar_y);
		glVertex2i(bar_x, bar_y - bar_height);
		glColor3f(0.4, 0.4, 0.4);
		glVertex2i(bar_x + bar_width, bar_y - bar_height);
		glVertex2i(bar_x + bar_width, bar_y);


		// filling
		glColor3f(barcolor.r / 2, barcolor.g / 2, barcolor.b / 2);	// darker
		glVertex2i(bar_x, bar_y);
		glVertex2i(bar_x, bar_y - bar_height);
		glColor3f(barcolor.r, barcolor.g, barcolor.b);		// lighter
		glVertex2i(bar_x + bar_fill, bar_y - bar_height);
		glVertex2i(bar_x + bar_fill, bar_y);
	glEnd();
}


void writeoverlay(char* str)
{
	/// simply draws the string given at the cursor's location

	//glRasterPos2i(text_x, text_y);
	glPushMatrix();
	glTranslatef(text_x, text_y, 0);
	// NOTE : apparently everything's upside-down!
	glRotatef(180, 1, 0, 0);

	// standard font
	theFont.PlotText(str);

	glPopMatrix();

	text_y += 14;
}

void overlayBox(char* title, int x, int y, int width, int textlines, spaceColor color, spaceColor scolor)
{
	int padding = 4;
	GLfloat trans = 0.7;

		// background colors
	spaceColor bg_bright( color.r - .3, color.g - .3, color.b - .3);
	spaceColor bg_medium( color.r - .5, color.g - .5, color.b - .5);
	spaceColor bg_dark( color.r - .6, color.g - .6, color.b - .6);


	// draw a box around the info
	glBegin(GL_QUADS);
		// upperleft is brighter
		glColor4f(bg_bright.r, bg_bright.g, bg_bright.b, trans);
		glVertex2i(x,y);
		glColor4f(bg_medium.r, bg_medium.g, bg_medium.b, trans);
		glVertex2i(x + width + padding, y);
		glColor4f(bg_dark.r, bg_dark.g, bg_dark.b, trans);
		glVertex2i(x + width + padding, textlines * 20 + y + padding);
		glColor4f(bg_medium.r, bg_medium.g, bg_medium.b, trans);
		glVertex2i(x, textlines * 20 + y + padding);
	glEnd();

	// draw the box title

	// foreground color
	spaceColor fg_text(scolor.r, scolor.g, scolor.b);
	
	// everything else is 'primary'
	glColor4f(fg_text.r, fg_text.g, fg_text.b, 1);

	text_x = x + 5;
	text_y = y + 10 + padding;
	writeoverlay(title);

	glBegin(GL_LINES);
		glVertex2i(x + 5, text_y - 9);
		glVertex2i(x + width - 5, text_y - 9);
	glEnd();

	text_y += padding; // a little extra
}

