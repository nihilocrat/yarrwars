#if !defined SPACE_RENDER_INCLUDED
#define SPACE_RENDER_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "glut.h"

#include "spaceobj.h"
#include "spacesector.h"


// put a clever define here that says we're compiling for the client target
// if I knew more about stuff I would do this at the makefile level


/********************************************************************************************************/
/* a few globals (tsk tsk)                                                                      */
/********************************************************************************************************/

// might want to move this over to the 'scene manager' thing
// controls on what plane along the Y axis the world is drawn
#define G_ship_elev		15.
#define G_turret_elev	21
#define G_grid_elev		0.
#define G_label_elev	30.


/********************************************************************************************************/
/* simple 3d necessities (Point3D)                                                                            */
/********************************************************************************************************/


// if I want to be clean, I should make sure Point3D only gets used in render actions, and never
// in game logic
class Point3D
{
public:
	SPfloat x, y, z;
	
	Point3D(SPfloat nx=0, SPfloat ny=0, SPfloat nz=0);
	virtual ~Point3D();
};


/********************************************************************************************************/
/* more complicated 3d stuff                                                                           */
/********************************************************************************************************/


// FIXME : it might be better, in the future, to use callback functions rather than child classes
//		for overloading 'emit'
class spaceTrail : public spaceEmitter
{
public:
	spaceTrail(){};
	spaceTrail(unsigned int nmax_life, unsigned int nmax_recharge, unsigned int nscatter,
		spaceColor ncolor);

	void bind(spaceShip *nparent, spacePoint noffset);
	void bind(spaceBullet *nparent, spacePoint noffset); // FIXME : hack
	void emit(spaceObj *bit);

	void particle_draw(spaceObj *p_part);

	unsigned int scatter;
	spacePoint offset;
};


class spaceBoom : public spaceEmitter
{
public:
	spaceBoom(){};
	spaceBoom(unsigned int nmax_life, unsigned int nmax_recharge, unsigned int nscatter,
		spaceColor ncolor);

	void bind(spaceShip *nparent, spacePoint noffset);
	void bind(spaceBullet *nparent, spacePoint noffset); // FIXME : hack
	void emit(spaceObj *bit);

	void particle_draw(spaceObj *p_part);

	unsigned int scatter;
	spaceShip *parent;
	spacePoint offset;
};

#endif