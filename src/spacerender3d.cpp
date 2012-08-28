#if !defined SPACE_RENDER_3D_INCLUDED
#define SPACE_RENDER_3D_INCLUDED

#include <iostream>


#include <stdlib.h>
#include "spaceobj.h"
#include "spacesector.h"

#include "spacerender3d.h"

Point3D::Point3D(SPfloat nx, SPfloat ny, SPfloat nz)
{
	x = nx;
	y = ny;
	z = nz;
}

Point3D::~Point3D()
{

}
/*
Point3D::dist(Point3D other)
{
    //Returns distance from obj1 to obj2
    SPfloat my_x, my_y, my_z;

	// FIXME : faster to do with C operators?
	my_x = (x - other.x);
    my_y = (y - other.y);
    my_z = (z - other.z);
    return hypot(my_x,my_y);
}*/






spaceTrail::spaceTrail(unsigned int nmax_life, unsigned int nmax_recharge, unsigned int nscatter, spaceColor ncolor)
{
	//spaceEmitter::spaceEmitter(nmax_life, nmax_recharge, ncolor);
	//cout << "maxparts : " << max_parts << "\t";
	
	recharge = 0;

	
	num_parts = 0;
	recharge = 0;

	max_life = nmax_life;
	max_recharge = nmax_recharge;
	max_parts = G_default_parts;//(unsigned int) (max_life / max_recharge) + 1;
	color = ncolor;	
	

	// set up the particle arrays
	part.reserve(max_parts);
	

	scatter = nscatter;

	// this sets the size of full-life particles
	size = 2.5;

}

void spaceTrail :: emit(spaceObj *bit)
{
	// CRAP : this is probably where the particle problem is
	// FIXME: give emitter real bind functions
	//		and then move this to there
	pos = parent->pos;

	bit->pos = pos;

	// now apply the offset
	bit->pos.x += offset.y * sin(radians(parent->heading));
	bit->pos.y += offset.y * cos(radians(parent->heading));

	bit->pos.x += offset.x * sin(radians(parent->heading));
	bit->pos.y += offset.x * cos(radians(parent->heading));

	if(scatter != 0)
	{
		bit->pos.x += (rand()%(int)(scatter*2) - (int)scatter);
		bit->pos.y += (rand()%(int)(scatter*2) - (int)scatter);
	}
}

void spaceTrail::particle_draw(spaceObj *p_part)
{
	glPushMatrix();	// FIXME : put the push and pop at a higher level
					// and do positioning relative to that

	float scale = p_part->life / 100.0;

	float actual_size = scale * size; 

	glColor3f(color.r * scale, color.g * scale, color.b * scale);
	/*
	// 'low-detail' bitmap points
	glBegin(GL_POINTS);
		glPointSize(actual_size);
		//glVertex3f(part[i].x, (SPfloat)G_ship_elev, part[i].y);
	glEnd();
	*/
	// 'high-detail' spherical points
	// FIXME : more efficient way of doing this?
	//		we might want to just draw a single polygon whose
	//		normal is pointing at the camera
	//		or if we're too lazy for that, making three which
	//		are aligned to each axis would probably have the same
	//		effect with less polys rendered
	glTranslatef(p_part->pos.x, (SPfloat)G_ship_elev, p_part->pos.y);
	
	bool simple_particles = true;
	// FIXME : move this to gamevars

	if(simple_particles)
		glutSolidCube(actual_size);
	else
		glutSolidSphere(actual_size, 4, 4);
	
	

	glPopMatrix();
}



void spaceTrail :: bind(spaceShip *nparent, spacePoint noffset)
{
	if(nparent == NULL)
	{
		cout << "unable to bind to null ship pointer\n";
		return;
	}

    parent = nparent;
	pos = parent->pos;

	offset = noffset;
}

// FIXME : hack
void spaceTrail :: bind(spaceBullet *nparent, spacePoint noffset)
{
	if(nparent == NULL)
	{
		cout << "unable to bind to null ship pointer\n";
		return;
	}

    parent = nparent;
	pos = parent->pos;

	offset = noffset;
}



void spaceBoom :: emit(spaceObj *bit)
{
	pos = parent->pos;

	bit->pos = pos;

	// FIXME : also emit the little bits around the big one
}



void spaceBoom::particle_draw(spaceObj *p_part)
{
	glPushMatrix();	// FIXME : put the push and pop at a higher level
					// and do positioning relative to that

	float scale = p_part->life / 100.0;

	float actual_size = scale * size; 

	glColor3f(pcolor.r * scale, pcolor.g * scale, pcolor.b * scale);
	/*
	// 'low-detail' bitmap points
	glBegin(GL_POINTS);
		glPointSize(actual_size);
		//glVertex3f(part[i].x, (SPfloat)G_ship_elev, part[i].y);
	glEnd();
	*/
	// 'high-detail' spherical points
	// FIXME : more efficient way of doing this?
	//		we might want to just draw a single polygon whose
	//		normal is pointing at the camera
	//		or if we're too lazy for that, making three which
	//		are aligned to each axis would probably have the same
	//		effect with less polys rendered
	glTranslatef(p_part->pos.x, (SPfloat)G_ship_elev, p_part->pos.y);
	

	glutSolidCube(actual_size);
	
	

	glPopMatrix();
}

/*
void spaceTrail :: emit(spacePoint *bit)
{
	// instead of updating the position every cycle to fit the offset,
	// why not just do it when we create new bits?
	SPfloat myoffsetx = parent->pos.x + ( offset.x * cos(radians(parent->heading)) );
	SPfloat myoffsety = parent->pos.y + ( offset.y * sin(radians(parent->heading)) );

	bit->x = pos.x + 1.0*(rand()%11-5);
	bit->y = pos.y + 1.0*(rand()%11-5);
}
*/

/********************************************************************************************************/
/* draw functions for spaceobj.h                                                                        */
/********************************************************************************************************/

void spaceObj::draw()
{
	glPushMatrix();
	
	glTranslatef(pos.x, G_ship_elev, pos.y);

	glBegin(GL_POINTS);
		glColor3f(color.r, color.g, color.b);
		glVertex3f(0, 0, 0);
	glEnd();

	glPopMatrix();
}

void spaceObj::drawlabel()
{
	spaceColor G_ifcolor(0.5, 1.0, 0.0);

	char* c_name = (char*)name.c_str();

	// color
	glColor3f(G_ifcolor.r, G_ifcolor.g, G_ifcolor.b); // sets the color to a nice torquoise

	// move to the lower left corner
	glRasterPos3f(pos.x, G_label_elev, pos.y);
	
	// loop through all characters and show them on screen
	int i = 0;
	while (c_name[i] != 0) {
		glutBitmapCharacter(GLUT_BITMAP_8_BY_13, c_name[i]);
		i++;
	}
}

void spacePlanet::draw()
{
	// FIXME : might be able to get a tiny speed increase if we merge this matrix with the
	//		one used to draw the grid and any other static content
	//		i.e. get rid of push and pop matrix, but only allow specific functions to call this one
	glPushMatrix();

	glTranslatef(pos.x, G_grid_elev - size, pos.y);
	glRotatef(35,  0,0,1);

	glColor3f(color.r, color.g, color.b);
	glutSolidSphere(size, 24, 24);

	glPopMatrix();
}

void spaceShip::draw()
{
	glPushMatrix(); // "pushes" a new matrix onto the stack - basically gives us a rotation matrix to work with

	glTranslatef(pos.x, G_ship_elev, pos.y);
	
	glRotatef(heading, 0.0, 1.0, 0); // rotates the square around the Z axis
	
	glScalef(1, 1, 1.0);
	
	glColor3f(color.r, color.g, color.b);	
	glutSolidCone(12, 30, 6, 6);

	glRotatef(180, 0.0, 1.0, 0); // rotates the square around the Z axis
	glutSolidCone(12, 10, 6, 6);

	glPopMatrix();	// "pops" our matrix back off the stack - applies the stuff we said to do
}

void spaceTurret::draw()
{
	glPushMatrix();
	// NOTE : notice that there are no push or pop matrices
	//	turrets have to be attached to something else to be drawn

	// translate it into the right place
	glTranslatef(pos.x, (G_turret_elev), pos.y);

	// apply offset and heading
	//glTranslatef(offset.x, 0, offset.y);	
	glRotatef(heading, 0,1,0);

	glScalef(1, 1, 1.0);
	
	glColor3f(color.r, color.g, color.b);	
	
	glBegin(GL_TRIANGLES);
		glNormal3f(0,1,-1);
		//back face
		glVertex3f( 0,  4, -2);
		glVertex3f( 4,  0,  0);
		glVertex3f(-4,  0,  0);

		// left face
		glNormal3f(-1,1,0);
		glVertex3f( 0,  4, -2);
		glVertex3f(-4,  0,  0);
		glVertex3f( 0,  2,  10);

		// right face
		glNormal3f(1,1,0);
		glVertex3f( 0,  4, -2);
		glVertex3f( 4,  0,  0);
		glVertex3f( 0,  2,  10);

		// bottom face
		glNormal3f(0,-1,1);
		glVertex3f(-4,  0,  0);
		glVertex3f( 4,  0,  0);
		glVertex3f( 0,  2,  10);

	glEnd();
	
	glPopMatrix();
}

void spaceBullet::draw()
{
	// wow, this is easy!
	glPushMatrix();

	glTranslatef(pos.x, G_turret_elev, pos.y);	
	glRotatef(heading, 0,1,0);
	
	glColor3f(color.r, color.g, color.b);
	glutSolidSphere(size, 4, 4);

	glPopMatrix();
}


/********************************************************************************************************/
/* draw functions for spacesector.h                                                                     */
/********************************************************************************************************/


void spaceSector :: drawgrid(spacePoint target)
{
	glPushMatrix();

		// grid
	glBegin(GL_LINES);
	
		glColor3f(0.1,0.2,0.4); // faded navy
		
		// first draw ones along x, then z
		float x, z;

		float viewdist = 3000;

		float xmin = -viewdist;
		float xmax = viewdist;
		float zmin = -viewdist;
		float zmax = viewdist;


		// move the 'center' of the grid
		if(target.x  <= center.x - gridspacing)
			center.x -= gridspacing;
		if(target.x  >= center.x + gridspacing)
			center.x += gridspacing;
		
		if(target.y  <= center.y - gridspacing)
			center.y -= gridspacing;
		if(target.y >= center.y + gridspacing)
			center.y += gridspacing;


		for(x = xmin ; x <= xmax ; x += gridspacing)
		{


			for(z = zmin ; z <= zmax ; z += gridspacing )
			{

				glVertex3f(center.x + xmin, G_grid_elev, center.y + z);
				glVertex3f(center.x + xmax, G_grid_elev, center.y + z);
			}

			glVertex3f(center.x + x, G_grid_elev, center.y + zmin);
			glVertex3f(center.x + x, G_grid_elev, center.y + zmax);
		}

	glEnd();

	glPopMatrix();
}; // HACK blarglgarlgl

/********************************************************************************************************/
/* draw functions for overview mode                                                                     */
/********************************************************************************************************/

void spaceSector :: overview_drawgrid(spacePoint target, SPfloat zoomamt)
{
	glPushMatrix();

		// grid
	glBegin(GL_LINES);
	
		glColor3f(0.1,0.2,0.4); // faded navy
		
		// first draw ones along x, then z
		float x, z, gridspacing = 500;

		float viewdist = 3000;

		// add some 'paddding' to the grid
		float xmin = -zoomamt - gridspacing;
		float xmax = zoomamt + gridspacing;
		float zmin = -zoomamt - gridspacing;
		float zmax = zoomamt + gridspacing;


		// move the 'center' of the grid
		if(target.x  <= center.x - gridspacing)
			center.x -= gridspacing;
		if(target.x  >= center.x + gridspacing)
			center.x += gridspacing;
		
		if(target.y  <= center.y - gridspacing)
			center.y -= gridspacing;
		if(target.y >= center.y + gridspacing)
			center.y += gridspacing;


		for(x = xmin ; x <= xmax ; x += gridspacing)
		{


			for(z = zmin ; z <= zmax ; z += gridspacing )
			{

				glVertex2f(center.x + xmin, center.y + z);
				glVertex2f(center.x + xmax, center.y + z);
			}

			glVertex2f(center.x + x, center.y + zmin);
			glVertex2f(center.x + x, center.y + zmax);
		}

	glEnd();

	glPopMatrix();
}; // HACK blarglgarlgl


void spaceShip::overview_draw()
{
	glPushMatrix();
	
	glTranslatef(pos.y, pos.x, 0);
	
	glRotatef(heading, 0.0, 0.0, 1.0); // rotates the square around the Z axis
	
	glScalef(1, 1, 1.0);
	
	// ships are drawn in their team color in overview
	glColor3f(scolor.r, scolor.g, scolor.b);	

	glBegin(GL_TRIANGLES);
		glVertex2i(-20, -30);
		glVertex2i(60, 0);
		glVertex2i(-20, 30);
	glEnd();

	glPopMatrix();
}

void spacePlanet::overview_draw()
{
	
	glPushMatrix();
	
	glTranslatef(pos.y, pos.x, 0);
		
	glScalef(1, 1, 1.0);
	
	// ships are drawn in their team color in overview
	glColor3f(color.r, color.g, color.b);	

	glBegin(GL_POLYGON);
		// how complex do we want our sphere?
		int vertices = 16;
		// 6.28318 is 2 pi, just you so know
		float radians_per_vertex = 6.28318 / vertices;

		unsigned int v;
		for(v = 0; v < vertices ; v++)
		{
			glVertex2i(	sin(v * radians_per_vertex) * size, cos(v * radians_per_vertex) * size);
		}
	glEnd();

	// HACK : only planets get labels here, so there's no need to make a general function
	//	 for writing them

	spaceColor G_ifcolor(0.5, 1.0, 0.0);

	char* c_name = (char*)name.c_str();

	// color
	glColor3f(G_ifcolor.r, G_ifcolor.g, G_ifcolor.b); // sets the color to a nice torquoise

	// move to the lower left corner
	glRasterPos3f(pos.y/4, pos.x/4, 0);
	//glTranslatef(pos.y, pos.x, 0);

	// loop through all characters and show them on screen
	int i = 0;
	while (c_name[i] != 0) {
		glutBitmapCharacter(GLUT_BITMAP_8_BY_13, c_name[i]);
		i++;
	}


	glPopMatrix();
}

void spaceBullet::overview_draw()
{
	// just a dot!
	glPushMatrix();

	glPointSize(3);

	glColor3f(color.r, color.g, color.b);	

	glBegin(GL_POINTS);
		glVertex2i(pos.y, pos.x);
	glEnd();

	glPopMatrix();
}

#endif
