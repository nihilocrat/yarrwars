#include <math.h>
#include <string.h>
#include <iostream.h>
#include <fstream.h>

#include "glut.h"
#include "spacelib.h"

GLfloat rad(GLfloat deg)
{
	return deg * 0.017453;
}

simpleShip::simpleShip()
{
	x = 0; y = 0; xvel = 0; yvel = 0; heading = 0, impulse = 0; size = 1.0;
}

simpleShip::simpleShip(GLfloat nx, GLfloat ny, GLfloat nhead, GLfloat nimp, GLfloat ntr, spaceColor ncolor)
{
	x = nx; y = ny; heading = nhead; impulse = nimp; turnrate = ntr;
		color = ncolor;
}


void simpleShip::draw()
{
	glPushMatrix(); // "pushes" a new matrix onto the stack - basically gives us a rotation matrix to work with

	glTranslatef(x, y, 0.0);
	
	glRotatef(heading, 0.0, 0.0, 1.0); // rotates the square around the Z axis

	glScalef(1, -1, 1.0);
	glBegin(GL_TRIANGLES);
		

		glColor3f(color.r - 0.4, color.g - 0.4, color.b - 0.4);
		glVertex3f(-10, -8, 0.0);
		glVertex3f(-10,  -2, 0.0);
		glColor3f(color.r, color.g, color.b); // sets the color with floating point numbers
		glVertex3f(10,   -2,  0.0);

		glColor3f(0.6, 1.0, 0.2);
		glVertex3f(-10, -8, 0.0);
		glVertex3f(-5,  -12, 0.0);
		glColor3f(0.3, 0.7, 0.0);
		glVertex3f(-6,   -6,  0.0);
	glEnd();

	glScalef(1, -1, 1.0);

	glBegin(GL_TRIANGLES);
		glColor3f(color.r - 0.4, color.g - 0.4, color.b - 0.4);
		glVertex3f(-10, -8, 0.0);
		glVertex3f(-10,  -2, 0.0);
		glColor3f(color.r, color.g, color.b); // sets the color with floating point numbers
		glVertex3f(10,   -2,  0.0);

		glColor3f(0.6, 1.0, 0.2);
		glVertex3f(-10, -8, 0.0);
		glVertex3f(-5,  -12, 0.0);
		glColor3f(0.3, 0.7, 0.0);
		glVertex3f(-6,   -6,  0.0);
	glEnd();
	// cockpit
	
	glScalef(1, 1, 1.0);
	glBegin(GL_TRIANGLES);
		glColor3f(0.6, 1.0, 0.2); // make it green
		glVertex3f(-3, -2, 0.0);
		glVertex3f(-3,  2, 0.0);
		glColor3f(0.3, 0.7, 0.0); // make it green
		glVertex3f(3, 0.0,  0.0);
	glEnd();

	glPopMatrix();	// "pops" our matrix back off the stack - applies the stuff we said to do
}

void simpleShip::turn(int direction)
{
	if(direction == 0) return;

	switch(direction)
	{
		case LEFT:
			heading += turnrate;
			break;
		case RIGHT:
			heading -= turnrate;
			break;

		// correctly wrap things
		if(heading > 360)
			heading -= 360;
		if(heading < 0)
			heading += 360;
	}
}

void simpleShip::move()
{
	x += xvel;
	y += yvel;
}

void simpleShip::thrust()
{
	xvel += cos(rad(heading)) * impulse;
	yvel += sin(rad(heading)) * impulse;
}

/*
void spaceDrawSEO(string filename)
{
	ifstream seo_file(filename);

	if(seo_file == NULL)
	{
		cout << "Could not open object data from " << filename << "\n";
		exit(0); // FIXME : don't just kill the program, jeez
	}

	while(!seo_file.eof())
	{
		string commandtype;
		GLfloat vertices;

		cin >> commandtype;

		switch(commandtype)
		{
			case "poly": // draw a poly
				glBegin(); // ... so start a drawing sequence
				cin >> vertices;
				for(i=0; i<vertices; i++)
				{
					GLfloat v1, v2;
					cin >> v1 >> v2;
					glVertex2f(v1, v2);
				}
				glEnd(); // and end it
			break;
		}
	}
}
*/