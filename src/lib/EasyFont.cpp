// EasyFont.cpp: implementation of the EasyFont class.
//
//////////////////////////////////////////////////////////////////////

#include "..\\glut.h"
#include "EasyFont.h"
#include "BitmapTexture.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

EasyFont::EasyFont()
{
	// set default options

	// white color
	red = 1;
	green = 1;
	blue = 1;
	
	// standard size
	size = 1;

	// no kerning
	topkern = 0;
	xkern = 0;
	bottomkern = 0;

}

EasyFont::~EasyFont()
{

}

void EasyFont::LoadFont(int id) {

	fontID = id;
	SetTextureParameters();
	LoadBitmapTexture(fontID);

}

void EasyFont::SetColor(float r,float g,float b) {

	// set current color

	red = r;
	green = g;
	blue = b;

}

void EasyFont::SetSize(float s) {

	// set font size (in world units)

	size = s;

}

void EasyFont::SetKern(float xk,float tk,float bk) {

	// set kerning factors

	xkern = xk;
	topkern = tk;
	bottomkern = bk;
}
	

// Draw text at drawing point
void EasyFont::PlotText(char text[]) {
	// draw text along current X axis scaled to one font unit per one world unit

	// set general text material
	SetMaterial();

	// check if color material, blending, and 2D textures are enabled, and enable if not
	bool CMEnabled = glIsEnabled(GL_COLOR_MATERIAL);
	if (!CMEnabled) glEnable(GL_COLOR_MATERIAL);

	// blending allows transparent text, allowing background color in black spots
	bool BlendEnabled = glIsEnabled(GL_BLEND);
	if (!BlendEnabled) glEnable (GL_BLEND); 
	glBlendFunc (GL_ONE, GL_ONE);

	bool Tex2dEnabled = glIsEnabled(GL_TEXTURE_2D);
	if (!Tex2dEnabled) glEnable (GL_TEXTURE_2D); 

	glColor3f(red,green,blue);
	SetCurrentTexture(fontID);

	glPushMatrix();
	glScalef(size,size,size);

	glNormal3f(0,0,1);
	
	// step through text
	int i;
	for (i=0;i<strlen(text);i++) {
		
		char c = text[i];
		
		if (c>=32 && c<127) { // check to see if it's a character we have mapped
			
			// find texture coordinates - assumes a 10x10 grid of character pictures starting at char 32
			float tx,ty;
			tx = (int)((c-32)%10)*0.1;
			ty = 1.0-((int)((c-32)/10)*0.1);

			// draw texture mapped quad
			glBegin(GL_QUADS);
			glTexCoord2f(tx+0.1*xkern,ty-0.1+0.1*bottomkern);
			glVertex2f(0,bottomkern);
			glTexCoord2f(tx+0.1-0.1*xkern,ty-0.1+0.1*bottomkern);
			glVertex2f(1-2*xkern,bottomkern);
			glTexCoord2f(tx+0.1-0.1*xkern,ty-0.1*topkern);
			glVertex2f(1-2*xkern,1-topkern);
			glTexCoord2f(tx+0.1*xkern,ty-0.1*topkern);
			glVertex2f(0,1-topkern);
			glEnd();

			// shift over one spot
			glTranslatef(1-2*xkern,0,0);
		}
	}

	glPopMatrix();

	// if color material was off, turn it back off
	if (!CMEnabled) glDisable(GL_COLOR_MATERIAL);

	// if blending was off, turn it back off
	if (!BlendEnabled) glDisable(GL_BLEND); 

	// if 2D textures was off, turn it back off
	if (!Tex2dEnabled) glDisable (GL_TEXTURE_2D); 

}



// Center selected text at drawing point
void EasyFont::CenterText(char text[]) {

	glPushMatrix();

	float width = strlen(text)*size*(1-2*xkern);
	float height = 1-topkern-bottomkern;

	glTranslatef(-0.5*width,-0.5*height,0);

	PlotText(text);

	glPopMatrix();
}



// generic bright text material
void EasyFont::SetMaterial() {

	GLfloat mamb[] = {1.0,1.0,1.0,1.0};
	GLfloat mdiff[] = {1.0,1.0,1.0,1.0};
	GLfloat mspec[] = {0.9,0.9,0.9,1.0};
	GLfloat shininess = 75;

	glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,mamb);
	glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,mdiff);
	glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,mspec);
	glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shininess);
}