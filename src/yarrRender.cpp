#include "spacerender3d.h"
#include "yarrCraft.h"

void yarrCraft::draw()
{
	glPushMatrix(); // "pushes" a new matrix onto the stack - basically gives us a rotation matrix to work with

	glTranslatef(pos.x, G_ship_elev, pos.y);
	
	glRotatef(heading, 0.0, 1, 0.0); // rotates the square around the Z axis
	
	glScalef(1, 1, 1.0);
	glColor3f(color.r, color.g, color.b);
	
	// bow (?)
	//glTranslatef(0, 0, 30);	
	//glutSolidCone(5, 20, 6, 6);

	// bridge
	glDisable(GL_LIGHTING);

	glColor3f(scolor.r, scolor.g, scolor.b);
	glTranslatef(0, 3, 22);
	
	glScalef(4, 4, 4);
	glutSolidDodecahedron();
	glScalef(.25, .25, .25);

	glTranslatef(0, -3, 0);
	
	glEnable(GL_LIGHTING);
	

	// hull
	glColor3f(color.r, color.g, color.b);
	glTranslatef(0, 0, -22);
	glScalef(2, 1, 4);
	
	glRotatef(180, 1, 0, 0);
	glutSolidSphere(8, 5, 6);
	glRotatef(180, 1, 0, 0);
	
	glScalef(.5, 1, .25);

	// wings

	glTranslatef(0, 0, -14);

	glColor3f(scolor.r, scolor.g, scolor.b);
	glRotatef(90, 0, 1, 0);
	glScalef(1, .5, 2);
	glutSolidCone(12, 12, 5, 1);
	glRotatef(180, 0, 1, 0);
	glutSolidCone(12, 12, 5, 1);

	//glScalef(1, 4, .5);

	// fix the matrix for the sake of drawing the turret
	glPopMatrix();
	//glRotatef(90, 0, 1, 0);
	//glTranslatef(0, 0, 14);

	// now draw the turrets
	int i;
	for(i = 0; i < G_max_turrets ; i++)
	{
		turrets[i].draw();
	}

	//glPopMatrix();
}
