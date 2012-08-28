#if !defined(__SPACE_WORLD_3D_INCLUDED)
#define __SPACE_WORLD_3D_INCLUDED

#include "glut.h"
#include "spacerender3d.h"
#include "yarrCraft.h"

enum cameraMode
{
	LOCKED_CHASE,
	FREE_CHASE,
	TURRET_CHASE,
	OVERHEAD
};

class spaceCamera3D
{
public:
	spaceCamera3D();
	virtual ~spaceCamera3D(){};

	void update(yarrCraft *ship);
	void track(spaceObj *target);

	// wrapper functions
	void set(Point3D new_eye, Point3D new_focus);
	Point3D get();
	void setbounds(SPfloat nearplane, SPfloat farplane);

// holy crap, I'm actually modularizing for once!
protected:
	SPfloat fovangle, aspratio;
	SPfloat min_viewdistance, max_viewdistance;

	Point3D eye, focus;
};

#endif