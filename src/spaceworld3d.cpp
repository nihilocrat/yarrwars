#include "spaceworld3d.h"

spaceCamera3D :: spaceCamera3D()
{


	// these will probably never need to be changed
	fovangle = 60;
	aspratio = 1;

	float speed = 0;
}


Point3D spaceCamera3D :: get()
{
	return eye;
}

void spaceCamera3D :: set(Point3D new_eye, Point3D new_focus)
{
	eye = new_eye;
	focus = new_focus;
}

void spaceCamera3D :: setbounds(SPfloat nearplane, SPfloat farplane)
{
	min_viewdistance = nearplane;
	max_viewdistance = farplane;
}

void spaceCamera3D :: track(spaceObj *target)
{
	Point3D newfoc;

	newfoc.x = target->pos.x;
	newfoc.y = G_ship_elev; // FIXME : for now, the camera can only look at stuff on the ship plane
	newfoc.z = target->pos.y;
}

void spaceCamera3D :: update(yarrCraft *ship) {

	glMatrixMode(GL_MODELVIEW); // Using modelview as a convenient matrix stack for our math

	glPushMatrix();
	glLoadIdentity(); 	// replace the current matrix with the indentity matrix to get started

	// figure out which way is up (in case we roll all the way over)
	int up=1;

	glPopMatrix();
	// done calculating



	focus.x = ship->pos.x;
	focus.y = 0;
	focus.z = ship->pos.y;

/*
	// scale the zoom amount for certain views
	float halfzoom = zoomamt / 2;
	float quarterzoom = zoomamt / 4;


	// math for chase cam
	switch(cam_orientation)
	{
		case LOCKED_CHASE:
			eye.x = ship->pos.x;
			eye.z = ship->pos.y;
			eye.x += sin(radians(ship->heading + 180)) * halfzoom;	// FIXME : not so hacky, please
			eye.y = quarterzoom * .75;
			eye.z += cos(radians(ship->heading + 180)) * halfzoom;
			break;


		case FREE_CHASE:			
			eye.x = ship->pos.x - halfzoom;
			eye.y = halfzoom;
			eye.z = ship->pos.y - halfzoom;
			break;

		case TURRET_CHASE:
			eye.x = ship->pos.x;
			eye.z = ship->pos.y;
			eye.x += sin(radians(ship->heading + 180)) * halfzoom;	// FIXME : not so hacky, please
			eye.y = quarterzoom;
			eye.z += cos(radians(ship->heading + 180)) * halfzoom;
			break;

		case OVERHEAD:
			eye.x = ship->pos.x - .1; // need to do this
			eye.y = zoomamt * 2;
			eye.z = ship->pos.y;
			break;
	}

	// FIXME : put in math here to figure out distance from
	// camera to target (or just use zoomamt) to adjust
	// the scaling factor for glPointSize
	// so particles show up right

	// now set the camera the right way
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Now, set a 3-D perspective view, with:
	// 60 degree FOV in y direction,
	// 1:1 aspect ratio for X,
	// starting at Z = 5,
	// ending at Z=3000
	gluPerspective(fovangle,aspratio,5,3500);

	gluLookAt(eye.x, eye.y,eye.z, focus.x,focus.y,focus.z, 0.0, up, 0.0);


	//Read the projection, modelview and viewport matrices
	//using the glGet functions.
	glGetDoublev( GL_PROJECTION_MATRIX, projection );
	glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
	glGetIntegerv( GL_VIEWPORT, viewport );
*/

	glMatrixMode(GL_MODELVIEW); // set to display stuff
	glLoadIdentity(); // resets matrix again

}