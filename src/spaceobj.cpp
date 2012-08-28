#include <iostream>
#include <string>
#include <map>
#include <algorithm>

using namespace std;

#include <math.h>
#include <stdlib.h>
#include "spaceobj.h"



// game globals
extern map<string,SPfloat> spaceGamevars;

/********************************************************************************************************/
/* some extra math functions                                                                            */
/********************************************************************************************************/


SPfloat radians(SPfloat degrees)
{
	return degrees * 0.017453;
}
SPfloat degrees(SPfloat radians)
{
	return (180.0/3.14159) * radians;
}

SPfloat wrap(SPfloat angle)
{
	if(angle >= 360)
		return angle - 360;
	if(angle < 0)
		return angle + 360;

	return angle;
}



/********************************************************************************************************/
/* Attribute Objects (Point, Vector, etc.)                                                              */
/********************************************************************************************************/

spacePoint :: spacePoint(SPfloat nx, SPfloat ny)
{
	x = nx;
	y = ny;
}

bool spacePoint :: operator== (spacePoint p2)
{
	if(x == p2.x)
		if(y == p2.y)
			return true;

	return false;
}

spacePoint spacePoint :: operator+ (spacePoint p2)
{
	spacePoint newpoint;

	newpoint.x = x + p2.x;
	newpoint.y = y + p2.y;

	return newpoint;
}

spacePoint spacePoint :: operator- (spacePoint p2)
{
	spacePoint newpoint;

	newpoint.x = x - p2.x;
	newpoint.y = y - p2.y;

	return newpoint;
}

SPfloat spacePoint :: angleto(spacePoint obj2)
{
	if( obj2 == NULL )
	{
		//cout << "spaceObj " << name.c_str() << " angleto null pointer\n";
		return 0;
	}

    //Returns the angle from obj1 to obj2
	SPfloat my_x = obj2.x - x;
    SPfloat my_y = obj2.y - y;

	//dir = -(180.0/3.14159) * atan2(ship.x-x,ship.y-y)

    return degrees(atan2(my_x,my_y));
}



SPfloat spacePoint :: dist(spacePoint obj2)
{
    //Returns distance from obj1 to obj2
    SPfloat my_x, my_y;

	my_x = (x - obj2.x);
    my_y = (y - obj2.y);
    
	// FIXME : faster to do with C operators and sqrt?
	//return sqrt( (my_x * my_x) + (my_y * my_y) );
	return hypot(my_x,my_y);
}


spaceColor :: spaceColor(SPfloat nr, SPfloat ng, SPfloat nb)
{
	r = nr;
	g = ng;
	b = nb;
}


/********************************************************************************************************/
/* Basic Objects (spaceObj, spaceShip)                                                                */
/********************************************************************************************************/

spaceObj :: spaceObj()
{
	// default name for objects
	name = "Foobar";

	color = spaceColor();
	size = 1.0;
	alive = true;
}


void spaceGroup :: append(spaceObj* newguy)
{
	if(newguy == NULL) return;

	push_back(newguy);

	// now, redistribute grog
	redistribute();
}

	
void spaceGroup :: remove(spaceObj* oldguy)
{
	// first, find the ship
	vector<spaceObj*>::iterator pirate_i;
	// FIXME : why is this broken!?!?
	
	for(pirate_i = begin(); pirate_i != end(); pirate_i++)
	{
		if((*pirate_i) == oldguy)
		{
			erase(pirate_i);
			break;
		}
	}


	// set up a second-in-command ifpossible
	if(oldguy == leader)
		if(size() > 0)
			leader = back(); // FIXME : I'm too tired to do something sensible here
	/*
	pirate_i = find(mates.begin(), mates.end(), oldpirate);
	
	if(pirate_i != mates.end()) // sort of like != NULL in STL world
	{
		mates.erase(pirate_i);
	}
	*/

	
	// 'kill' the team if it has no more members
	if(size() <= 0)
		alive = false;
}


void spaceGroup :: redistribute()
{
	// for now, we're doing it commie style!
	unsigned int totalgrog = 0;

	vector<spaceObj*>::iterator matey;
	for(matey = begin();
		matey != end();
		matey++)
	{
			spaceShip *me = (spaceShip*)(*matey);
			totalgrog += me->grog;
	}

	unsigned int grog_per_ship = totalgrog / size();

	for(matey = begin();
		matey != end();
		matey++)
	{
			spaceShip *me = (spaceShip*)(*matey);
			me->grog = grog_per_ship;

			// limit overflows
			if(me->grog > me->maxgrog)
				me->grog = me->maxgrog;
	}
}



void spaceGroup :: calc_avg_heading()
{
	// add up the headings of each mate and divide by the number
	// easy!
	int nmates = size();
	SPfloat total_heading = 0;

	vector<spaceObj*>::iterator m_i;
	for(m_i = begin(); m_i != end() ; m_i++)
	{
		total_heading += (*m_i)->heading; 
	}

	// we don't do divide by zero, son.
	if( nmates == 0 )
		nmates = 1;


	avg_heading = total_heading / nmates;
}

spacePlanet :: spacePlanet(string nname, spacePoint npos, SPfloat nsize, spaceColor ncolor)
{
	name = nname;

	pos = npos;
	color = ncolor;
	size = nsize;
	alive = true;
}


spaceShip :: spaceShip()
{
	// vector that desribes current heading (angle) and impulse (mag)
	// therefore, it should (normally) remain constant

	color = spaceColor();
	size = 1.0;
	maxvel = 10.0;
	heading = 0;
}

spaceShip :: spaceShip(string newname, spacePoint npos, SPfloat nheading, SPfloat nturnspeed,
					   SPfloat nimpulse, SPfloat nmaxvel, spaceColor ncolor, spaceColor nscolor)
{
	name = newname;

	pos = npos;
	turnspeed = nturnspeed;
    heading = nheading;
	impulse = nimpulse;
	maxvel = nmaxvel;
	color = ncolor;
	scolor = nscolor;
}

void spaceShip :: move()
{
	// don't do physics or control stuff if I don't exist!
	if(!alive) return;

    // Move an object along its current vector
    pos.x += vel.x;
	pos.y += vel.y;
};

void spaceShip :: friction(SPfloat amount)
{
	// don't do physics or control stuff if I don't exist!
	if(!alive) return;

	/// Apply friction to this object, but only if it's moving
	if (vel.x*vel.x + vel.y*vel.y > 0) {
		vel.x *= amount;
		vel.y *= amount;

		// make sure we didn't go over
		/*if(abs(vel.x) < 0)
			vel.x = 0;
		if(abs(vel.y) < 0)
			vel.y = 0;
		*/
	}
}

void spaceShip :: push(SPfloat theta, SPfloat magnitude)
{
	// don't do physics or control stuff if I don't exist!
	if(!alive) return;
	
	vel.x += sin( radians( theta ) ) * magnitude;
	vel.y += cos( radians( theta ) ) * magnitude;
}

void spaceShip :: turn(spaceDirection direction)
{
	// don't do physics or control stuff if I don't exist!
	if(!alive) return;
    
	if(direction == RIGHT)    heading -= turnspeed;
    if(direction == LEFT)     heading += turnspeed;

    heading = wrap(heading); // make sure the angle is 0-360
}

void spaceShip :: turn(SPfloat maxturn)
{
	if( maxturn > turnspeed )
	{
		maxturn = turnspeed;
	}
	else if( maxturn < -turnspeed)
	{
		maxturn = -turnspeed;
	}

	/// Positive values turn LEFT
	/// Negative values turn RIGHT
	heading += maxturn;

	heading = wrap(heading);
}


void spaceShip :: thrust()
{
	// don't do physics or control stuff if I don't exist!
	if(!alive) return;

	///''' Basic linear movement function, pushes object along its thrust vector'''
	limitvel();

	vel.x += sin(radians(heading)) * impulse;
	vel.y += cos(radians(heading)) * impulse;
}

void spaceShip :: limitvel()
{
	// don't do physics or control stuff if I don't exist!
	if(!alive) return;

	/// limit speed
	if (vel.x*vel.x + vel.y*vel.y > maxvel*maxvel) {
		vel.x *= 0.99;
		vel.y *= 0.99;
	}
}


void spaceShip :: handle_health()
{
	/// simply sees if this ship should be dead or not
	if(life <= 0 || grog <= 0)
	{
		life = 0;
		alive = false;

		//FIXME : now plop down a 'dying ship' object
		//	or just a bunch of explosions

		// remove this guy from his group
		group->remove(this);
	}
	
	// limit healing weapons / other stuff to the maximum amount of life
	if(life > maxlife)
	{
		life = maxlife;
	}
}


bool spaceShip :: collide(spaceObj *other)
{
	// don't test collision with the same object!
	if( other == this )
		return false;

	// debug
	//return false;


	// HACK  : bullets are assumed to be zero-width for speed and better 'effect'
	//size = 0;

	/*
	// new method (faster?)
	float collide_dist = size + other->size;

	if( fabs(other->pos.x - pos.x) <= collide_dist )
		if( fabs(other->pos.y - pos.y) <= collide_dist )
			return true;

	return false;
	*/
	
	// old method (slower?)

	float itsleft = other->pos.x - other->size;
	float itsright = other->pos.x + other->size;
	float itstop = other->pos.y - other->size;
	float itsbottom = other->pos.y + other->size;

	// now see if they overlap
	if (pos.x + size > itsleft)
		if (pos.x - size < itsright)
			if (pos.y + size > itstop)
				if (pos.y - size < itsbottom)
					return true;
	
	return false;
}


/********************************************************************************************************/
/* Projectile / Effect Objects (spaceEmitter, spaceBullet)                                              */
/********************************************************************************************************/

spaceEmitter::spaceEmitter(unsigned int nmax_life, unsigned int nmax_recharge,
						 spaceColor ncolor)
{
	num_parts = 0;
	recharge = 0;

	max_life = nmax_life;
	max_recharge = nmax_recharge;
	max_parts = (unsigned int) (max_life / max_recharge) + 1;
	pcolor = ncolor;

	// set up the particle array
	// reserving space makes the vector more efficient
	part.reserve(max_parts);
}

spaceEmitter::~spaceEmitter()
{
	//delete part;
	//delete part_life;
}


void spaceEmitter::fire()
{
	if(recharge <= 0)
	{
		// make a new particle
		spaceObj newpart;
		newpart.life = max_life; // give it LIFE!!!!

		// FIXME : I guess lump this in with tick for the time being rather than make a move func
		//pos = parent->pos;

		// first translate it to here...
		//newpart.pos = pos;
		newpart.alive = true;

		// emit the particle in the gameworld
		emit(&newpart);

		// push the particle onto the vector
		part.push_back(newpart);

		// start the recharge timer
		recharge = max_recharge;
	}
}


void spaceEmitter::tick(unsigned int numticks)
{
	// first tick the recharge timer
	if(recharge > 0)
		recharge--;

	// if there are no particles, I'm 'dead' right now
	/*
	if(part.size() <= 0)
	{
		alive = false;
		//return;
	}
	*/

	vector<spaceObj>::iterator part_i;
	for(part_i = part.begin() ; part_i != part.end() ; part_i++)
	{
		// kill off 'dead' particles
		if( part_i->alive && part_i->life <= 0 )
		{
			part_i->life = 0;
			part_i->alive = false;

			// erase the bullet from the vector
			part.erase(part_i);
			part_i--;	// NOTE : for some bizarre reason, doing this avoids nasty pointer issues
		}
		else
		{
			part_i->life -= numticks;
		
			// handle other per-tick stuff here
			//part_i->move();
		}
	}
}


void spaceEmitter::draw()
{
	vector<spaceObj>::iterator part_i;
	for(part_i = part.begin() ; part_i != part.end() ; part_i++)
	{
		// just in case...
		if(part_i != NULL)
			particle_draw(part_i);
	}

}


void spaceEmitter::move(spacePoint newpos)
{
	// FIXME : same problem as the turret updating its position
	pos = newpos;
}



/********************************************************************************************************/
/* Advanced Objects (spaceCraft, spaceTurret)                                                            */
/********************************************************************************************************/


spaceWeapon::spaceWeapon(unsigned int nmax_life, unsigned int nmax_recharge,
						 SPfloat nbulletspeed, SPfloat nscatter, spaceColor ncolor)
{
	num_parts = 0;
	recharge = 0;

	max_life = nmax_life;
	
	// prevent divide-by-zero below
	if(max_recharge < 0)
		max_recharge = 1;
	else
		max_recharge = nmax_recharge;
	
	bulletspeed = nbulletspeed;
	damage_amt = 1.; // FIXME : make this a parameter!
	scatter = nscatter;
	max_parts = (unsigned int) (max_life / max_recharge) + 1;

	// set up the particle array
	// reserving space makes the vector more efficient
	part.reserve(max_parts);

	pcolor = ncolor;
}

spaceWeapon::~spaceWeapon()
{
	//delete part;
	//delete part_life;
}


void spaceWeapon :: bind(spaceShip *nparent, spacePoint noffset)
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


void spaceWeapon::fire()
{
	if(recharge <= 0)
	{
		alive = true;	// we have particles now!

		// make a new particle
		spaceBullet newpart;
		newpart.life = max_life; // give it LIFE!!!!
		newpart.alive = true;
		newpart.size = 2.5;
		newpart.color = pcolor; // FIXME : make this reflect the IFF's color
		newpart.team_id = parent->team_id;
		newpart.owner = parent->parent;

		// now pass this particle to the emit function, where fun things happen!
		//emit(&newpart);
		// FIXME : I guess lump this in with tick for the time being rather than make a move func
		pos = parent->pos;
		vel = parent->vel;

		// first translate it to here...
		newpart.pos = pos;
		newpart.vel = vel;

		// then orient it in the right direction
		// and add 'inaccuracy'
		newpart.heading = parent->heading;
		newpart.heading = wrap(newpart.heading + (rand()%(int)(scatter*2) - (int)scatter));
		
		// then give it some velocity to send it on its way!
		newpart.impulse = bulletspeed;
		newpart.thrust();
		//newpart.push(parent->heading, bulletspeed);

		// push the particle onto the vector
		part.push_back(newpart);

		
		// start the recharge timer
		recharge = max_recharge;
	}
}

void spaceWeapon :: emit(spaceBullet *bit)
{
	if(bit == NULL)
		return;

	// first translate it to here...
	bit->pos = pos;

	// then orient it in the right direction
	// and add 'inaccuracy'
	bit->heading = parent->heading + (rand()%(int)(scatter*2) - (int)scatter);
	cout << "heading : " << bit->heading << " \n";

	// then give it some velocity to send it on its way!
	bit->vel = parent->vel;
	bit->push(parent->heading, bulletspeed);
}


void spaceWeapon::tick(unsigned int numticks)
{
	// first tick the recharge timer
	if(recharge > 0)
		recharge--;

	// if there are no particles, I'm 'dead' right now
	/*
	if(part.size() <= 0)
	{
		alive = false;
		//return;
	}
	*/

	vector<spaceBullet>::iterator part_i;
	for(part_i = part.begin() ; part_i != part.end() ; part_i++)
	{
		// kill off 'dead' particles
		if( part_i->alive && part_i->life <= 0 )
		{
			part_i->alive = false;
			part_i->life = 0;

			// erase the bullet from the vector
			part.erase(part_i);
			part_i--;	// NOTE : for some bizarre reason, doing this avoids nasty pointer issues
		}
		else
		{
			part_i->life -= numticks;
		
			// handle other per-tick stuff here
			part_i->move();
		}
	}
}

void spaceWeapon::draw()
{
	vector<spaceBullet>::iterator part_i;
	for(part_i = part.begin() ; part_i != part.end() ; part_i++)
	{
		// just in case...
		if(part_i != NULL)
			part_i->draw();
	}
}

void spaceWeapon::overview_draw()
{
	vector<spaceBullet>::iterator part_i;
	for(part_i = part.begin() ; part_i != part.end() ; part_i++)
	{
		// just in case...
		if(part_i != NULL)
			part_i->overview_draw();
	}
}


void spaceWeapon::move(spacePoint newpos)
{
	// FIXME : same problem as the turret updating its position
	pos = newpos;
}

void spaceWeapon::collide_list(spaceObj* list[], unsigned int listsize)
{
	unsigned int i;
	vector<spaceBullet>::iterator part_i;
	for(part_i = part.begin() ; part_i != part.end() ; part_i++)
	{
		for( i = 0; i < listsize ; i++)
		{
			part_i->collide(list[i]);
		}
	}
}


void spaceWeapon::damage(spaceBullet *bullet_i, spaceShip *victim)
{
	// HACK : this is so that the AI knows this ship has been hit
	victim->attacker = parent->parent;

	// first do damagey-stuff
	victim->life -= damage_amt;
	victim->handle_health();

	// if he's dead, give the killer some fame
	if(!victim->alive)
	{
		spaceShip *killer = parent->parent;
		
		// give the killer some fame
		//killer->fame += victim->fame * spaceGamevars["fame_perecent_per_kill"];
		killer->fame += spaceGamevars["fame_per_kill"];

		// also, transfer grog
		killer->grog += victim->grog * spaceGamevars["grog_percent_per_kill"];
		if(killer->grog > killer->maxgrog)
			killer->grog = killer->maxgrog;

		// now split up the spoils on the killer's team
		killer->group->redistribute();
	}

	// then drop an fx event
	// FIXME : oh god, to do this we need a sector pointer
	//spaceBoom boom
	//boom
}

spaceTurret :: spaceTurret(SPfloat turnrate, SPfloat size, spaceWeapon ngun)
{
    turnspeed = turnrate;
    size = size;
	parent = NULL;
	gun = ngun;

	// also bind the weapons
	gun.bind(this, spacePoint(0,0));

	color = spaceColor(1.0, 1.0, 1.0);

    //weapons = []
}

void spaceTurret :: bind(spaceShip *nparent, spacePoint noffset)
{
	if(nparent == NULL)
	{
		cout << "unable to bind to null ship pointer\n";
		return;
	}

    parent = nparent;
	pos = parent->pos;

	offset = noffset;
	rotation = parent->heading; // by default, turrets point 'forward'
}


void spaceTurret :: move()
{
        //''' Set the turret's position to its parent's location, plus offset '''
        pos = parent->pos;
		
		// this is here so bullets will know how to behave
		vel = parent->vel;

		// also correct for heading changes
		heading = wrap(parent->heading + rotation);

		// add the offsets so the turret shows up in the right place
		// FIXME : eventually delete this code, moving the offsetting over to the rendering engine
		// HACK : we need gameworld coordinates for shooting bullets. recalculate this turret's offset
		pos.x += offset.y * sin(radians(parent->heading));
		pos.y += offset.y * cos(radians(parent->heading));

		pos.x += offset.x * sin(radians(parent->heading));
		pos.y += offset.x * cos(radians(parent->heading));

		// more HACK
		team_id = parent->team_id;
}

void spaceTurret :: turn(spaceDirection direction)
{
    if(direction == RIGHT)    rotation -= turnspeed;
    if(direction == LEFT)     rotation += turnspeed;

    rotation = wrap(rotation); // make sure the angle is 0-360
}

void spaceTurret :: track(spacePoint target)
{
	// figure out where the target is
	// add 360 to both to make comparisons correct
	SPfloat desired = pos.angleto(target);
	SPfloat current = heading;

	// first see if we should turn at all
	if (abs(desired - current) < turnspeed) return;

	// turn to face the target
	if (desired > current) {
		if (wrap(desired - current) > 180) turn(RIGHT);
		else turn(LEFT);
	}
	else {
		if (wrap(current - desired) > 180) turn(LEFT);
		else turn(RIGHT);
	}
}


void spaceTurret :: fire()
{
	// DEBUG
	gun.bind(this, spacePoint(0, 0));

	// cycle through weapons and fire them
	gun.fire();
}
