#ifndef __YARR_CRAFT_H__
#define __YARR_CRAFT_H__


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "spaceobj.h"
#include "spacerender3d.h"

#include <vector>
using namespace std;


#define G_max_turrets		3


class yarrPlanet : public spacePlanet
{
public:
	yarrPlanet(string nname = "Strongbadia", spacePoint npos = spacePoint(), SPfloat nsize=200,
		spaceColor ncolor=spaceColor());

	void setsector(spaceSector *newsector);
};

class yarrCraft : public spaceShip
{
	/// a class for shiplike objects with turrets and such
public:
	yarrCraft();
	yarrCraft(string newname, spacePoint npos, SPfloat nheading, 
					   spaceColor ncolor, spaceColor nscolor);
	
	void init();
	void setsector(spaceSector *newsector);
	
	void setteam(unsigned int nid, spaceColor ncolor);
	void leaveteam();
	void setgroup(spaceGroup *newgroup);
	void leavegroup();

	// controls
	void thrust();	// overload spaceShip::thrust()
	void retro();
	void fire();
	void track();

	void draw();
	
	void handle();
	void handle_health();	// overload spaceObj::handle_health()

	spaceTrail contrail;
	spaceTurret turrets[G_max_turrets];	// Laziness Council says we just use an array, okay?

	// 
	spacePoint target;
	spaceSector* sector;
	//extern spaceTeam* team;
	rawpointer controller_rawpointer;	// HACK : BLASPHEMY

	// Yarr Wars game variables
	//unsigned int grog, maxgrog, fame;
	unsigned int oldid;
	spaceColor oldcolor;

	spaceGroup *oldgroup;
};

/*
class yarrPlanet : public spacePlanet
{
	/// Big, immobile things!
public:
	yarrPlanet(char* nname = "Strongbadia", spacePoint npos = spacePoint(), SPfloat nsize=200,
		spaceColor ncolor=spaceColor());

	void draw();

	SPfloat grog, booty;
};
*/



class yarrBullet : public spaceBullet
{
public:
	yarrBullet(spaceSector *mysector);

	void move();

	spaceTrail contrail;
};


class yarrCannon : public spaceWeapon
{
public:
	yarrCannon(spaceColor bcolor = spaceColor(0,0,0));

	vector<yarrBullet> part;
};

class yarrTurret : public spaceTurret
{
public:
	yarrTurret(spaceWeapon myweapon);
	void draw(){ spaceTurret::draw(); };

	yarrCannon gun;
};


/*
class yarrSector : public spaceSector
{
public:
	//vector< yarrCraft* > ships;

	vector< yarrAI* > ai;
};
*/

#endif

