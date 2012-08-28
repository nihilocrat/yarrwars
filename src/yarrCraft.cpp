#include <iostream.h>

#include "spaceobj.h"
#include "spacerender3d.h"

#include "yarrCraft.h"
#include "yarrAI.h"

extern map<string, SPfloat> spaceGamevars;

map<string, SPfloat> yarrShipvars;

yarrPlanet :: yarrPlanet(string nname, spacePoint npos, SPfloat nsize, spaceColor ncolor)
{
	name = nname;

	pos = npos;
	color = ncolor;
	size = nsize;
	alive = true;
}

void yarrPlanet :: setsector(spaceSector *newsector)
{
	// nothing special here...
	newsector->add_planet(this);
}


yarrCraft :: yarrCraft()
{
	spaceShip::spaceShip();
}

yarrCraft :: yarrCraft(string newname, spacePoint npos, SPfloat nheading,
					   spaceColor ncolor, spaceColor nscolor)
{
	yarrCraft();

	
	name = newname;

	pos = npos;
    heading = nheading;
	color = ncolor;
	scolor = nscolor;

		// FIXME : constants... in the far future I should move these to a loadable config
	turnspeed = 0.6;
	impulse = 0.05; // originally .02
	maxvel = 2.;
	size = 20;
	maxlife = 21;
	life = maxlife;

	// game constants
	maxgrog = 100;
	grog = 20;
}

void yarrCraft :: init()
{
	// first things first, tell the game that it's alive
	alive = true;

	// ship stats
	turnspeed = yarrShipvars["turnspeed"];;
	impulse = yarrShipvars["impulse"]; // without friction : 0.02  with : 0.08
	maxvel = yarrShipvars["max_vel"];
	size = 20;
	maxlife = yarrShipvars["max_life"];
	life = maxlife;
	fame = 0;

	// stating grog
	// add some fuzziness for great inequality fun!
	maxgrog = spaceGamevars["max_grog"];
	grog = spaceGamevars["starting_grog"] + 
		rand() %(int)spaceGamevars["starting_grog_fuzzy"]*2
		- (int)spaceGamevars["starting_grog_fuzzy"];

	
	contrail = spaceTrail(200, 3, 3, scolor);
	contrail.bind(this, spacePoint(0, -34));

	spaceWeapon yarrgun( yarrShipvars["bullet_life"],
						yarrShipvars["reload_time"],
						yarrShipvars["bullet_speed"],
						yarrShipvars["bullet_scatter"],
						scolor);

	// mount the turrets
	int i;
	for(i = 0 ; i < G_max_turrets ; i++)
	{
		turrets[i] = spaceTurret(yarrShipvars["turret_turnrate"], 1,  yarrgun);
		//turrets[i].color = color;
		//turrets[i] = yarrTurret(yarrCannon(scolor));
		turrets[i].bind(this, spacePoint(0,((i*15) - 20)));
	}
}

void yarrCraft :: setsector(spaceSector *newsector)
{

	// bind myself and my contrail to the sector
	sector = newsector;
	newsector->add_ship(this);
	newsector->add_fx(&contrail);
	int i;
	for(i = 0 ; i < G_max_turrets ; i++)
	{
		newsector->add_weapon( &(turrets[i].gun) );
	}

	/*
	newteam = sector->teams.back();

	team = newteam;
	team_id = newteam->id;
	//neuteam.color = spaceColor(sr, sg, sb);
	newteam->add_ship(this);
	*/
}

void yarrCraft :: setteam(unsigned int nid, spaceColor ncolor)
{
	// first save old values
	oldcolor = scolor;
	oldid = team_id;

	//team = newteam;
	team_id = nid;
	scolor = ncolor;
	//newteam->add_ship(this);

	// also mess with gun and contrail colors
	int i;
	for(i = 0 ; i < G_max_turrets ; i++)
	{
		turrets[i].color = ncolor;
		turrets[i].gun.pcolor = ncolor;
	}

	contrail.color = ncolor;
}


void yarrCraft :: leaveteam()
{
	scolor = oldcolor;
	team_id = oldid;
}


void yarrCraft :: setgroup(spaceGroup *newgroup)
{
	oldgroup = group;

	group = newgroup;

	team_id = group->id;
	scolor = group->color;

	//group->push_back(this);
	group->append(this);

	// see if I'm leader
	if(group->size() <= 1)
		group->leader = this;


	// also mess with gun and contrail colors
	int i;
	for(i = 0 ; i < G_max_turrets ; i++)
	{
		turrets[i].color = group->color;
		turrets[i].gun.pcolor = group->color;
	}

	contrail.color = group->color;
}

void yarrCraft :: leavegroup()
{
	group->remove(this);

	group = oldgroup;
	
	setgroup(group);
}


void yarrCraft :: thrust()
{
	// don't do physics or control stuff if I don't exist!
	if(!alive) return;

	// first do everything a normal ship does...
	spaceShip::thrust();

	// and be sure to fire the engines, too
	contrail.fire();
}

void yarrCraft :: retro()
{
	/// accelerate in the opposite direction at half impulse
	limitvel();
	push(heading - 180, impulse * 0.5);

	contrail.fire();
}

void yarrCraft :: fire()
{
	// don't do physics or control stuff if I don't exist!
	if(!alive) return;

	int i;
	for(i = 0 ; i < G_max_turrets ; i++)
	{
		turrets[i].fire();
	}
}

void yarrCraft :: handle()
{
	int i;
	for(i = 0 ; i < G_max_turrets ; i++)
	{
		// move the turrets along with the craft
		turrets[i].move();
	}

	// point them in the right direction
	track();
}

void yarrCraft :: handle_health()
{
	spaceShip::handle_health();

	// also handle grog
	if(grog <= 0)
	{
		life = 0;
		alive = false;
	}

	// and limit overflows
	if(grog > maxgrog)
	{
		grog = maxgrog;
	}
}

void yarrCraft :: track()
{	
	/// NOTE: tracking is not convergent : turrets will have identical angles
	///		at all times to give a "broadside" effect

	spaceDirection turndir = NEUTRAL;
	SPfloat desired = pos.angleto(target);
	SPfloat current = turrets[0].heading; // HACK : arbitrary turret


	// first see if we should turn at all
	if (abs(desired - current) < 2) return;

	// turn to face the target
	if (desired > current) {
		if (wrap(desired - current) > 180) turndir = RIGHT;
		else turndir = LEFT;
	}
	else {
		if (wrap(current - desired) > 180) turndir = LEFT;
		else turndir = RIGHT;
	}

	
	int i;
	for(i = 0 ; i < G_max_turrets ; i++)
	{
		// point them in the right direction
		turrets[i].turn(turndir);
	}
}

yarrCannon :: yarrCannon(spaceColor bcolor)
{
	// define it as a spaceWeapon with these parameters
	// FIXME FIXME
	spaceWeapon::spaceWeapon(150, 120, 1.5, 45, bcolor);

	/*
	max_life = 150;
	max_recharge = 50;
	bulletspeed = 1.5;
	damage_amt = 1.;
	scatter = 45;
	*/
}

yarrTurret :: yarrTurret(spaceWeapon myweapon)
{
	spaceTurret::spaceTurret(1.0, 1, myweapon);
}

yarrBullet :: yarrBullet(spaceSector *mysector)
{
	spaceBullet::spaceBullet();

	// a bullet with a contrail!
	contrail = spaceTrail(75, 5, 0, color);
	contrail.bind(this, spacePoint(0, -size));

	mysector->add_fx(&contrail);
}

void yarrBullet :: move()
{
	spaceBullet::move();

	// shoot out bits of contrail!
	contrail.fire();
}
