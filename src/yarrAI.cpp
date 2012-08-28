#include "yarrAI.h"
#include "spaceobj.h"
#include "yarrcraft.h"
#include "spaceinc.h"

#include <math.h>
#include <fstream>

#include <map>


// map containing AI-related variables
map<string, SPfloat> yarrAIvars;


void yarrAI :: init()
{
	view_dist = yarrAIvars["view_distance"];
	feel_dist = yarrAIvars["feel_distance"];

	inaccuracy = yarrAIvars["inaccuracy"];
	standoff_range = yarrAIvars["standoff_range"];

	turndir = NEUTRAL;
	time = 0;

	// figure out optimum weapon range
	gunspeed = puppet->turrets[0].gun.bulletspeed;
	gunrange = (gunspeed * puppet->turrets[0].gun.max_life) * 0.8;
}

void yarrAI :: bind(yarrCraft *np)
{
	puppet = np;

	// unlock motors
	unlock_motors();
}


void yarrAI :: handle()
{
	// virtual function for doing stuff
	think();

	// unlock motors at end of cycle
	unlock_motors();


	time++;
	
	// reset timer as necessary
	if(time >= 10000)
		time = 0;
}

/*

	if(time % 100 == 0)
		if(rand()%100 >= 50)
			turndir = NEUTRAL;

	// every 50 cycles, decide if we're going to turn
	if(time % 50 == 0)
	{
		if(rand()%100 >= 50)
		{
			if(rand()%100 >= 50)
				turndir = RIGHT;
			else
				turndir = LEFT;
		}

		// RANDOM VIOLENCE!!!
		if(rand()%100 >= 50)
		{
			puppet->fire();
		}
	}

	if(turndir != NEUTRAL)
	{
		turn(turndir);
	}
	
	// constantly thrusting
	thrust();

*/


/// motor functions *******************************************************

void yarrAI :: stop()
{
	// basically, just lock the thrust function
	lock_thrust = true;
}

void yarrAI :: thrust()
{
	if(!lock_thrust)
	{
		puppet->thrust();
		lock_thrust = true;
	}
}

void yarrAI :: turn(spaceDirection dir)
{
	if(!lock_turn)
	{
		puppet->turn(dir);
		lock_turn = true;
	}
}

void yarrAI :: turn(SPfloat difference)
{
	if(!lock_turn)
	{
		// NOTE : this turning function does NOT lock the turning motor
		// FIXME : make a system which keeps track of how much we add/subtract
		//	each time
		puppet->turn(difference);
		//lock_turn = true;
	}
}


void yarrAI :: aimat(spacePoint target)
{
	if(!lock_turrets)
	{
		puppet->target = target;
		puppet->track();
		lock_turrets = true;
	}
}

// Sensor functions *********************************************************

bool yarrAI :: cansee(spacePoint thing)
{
	// You! Don't look at yourself!
	if(thing == puppet->pos)
		return false;

	if(fabs(puppet->pos.x - thing.x) <= view_dist)
		if(fabs(puppet->pos.y - thing.y) <= view_dist)
			return true;
			// FIXME : now add in a REAL distance check for circular vision
	
	return false;
}

bool yarrAI :: canfeel(spacePoint thing)
{
	// You! Quit touching yourself!
	if(thing == puppet->pos)
		return false;

	if(fabs(puppet->pos.x - thing.x) <= feel_dist)
		if(fabs(puppet->pos.y - thing.y) <= feel_dist)
			return true;
			// FIXME : now add in a REAL distance check for circular vision
	
	return false;
}

void yarrAI :: unlock_motors()
{
	lock_thrust = false;
	lock_turn = false;
	lock_turrets = false;
}


void yarrAI :: getvisible()
{
	// put all seen ships onto the 'seen'vector
	// HACK : for now, dump out the vector every frame.
	seen.clear();

	vector<spaceShip*>::iterator ships_i;
	for (ships_i = puppet->sector->ships.begin() ;
		ships_i != puppet->sector->ships.end() ;
		ships_i++)
	{
			// NOTE : ships do not "see" teammates. They can only "feel" them
			if( (*ships_i)->team_id != puppet->team_id)
				if( cansee((*ships_i)->pos) )
					seen.push_back((yarrCraft*)(*ships_i));
	}
}

spaceShip* yarrAI :: getclosest()
{
	/// Look around and find the ship closest to this one in its vision
	///
	/// Returns a ship pointer on success,
	/// returns a NULL POINTER on failure

	SPfloat closest_dist = view_dist + 1; // higher than vision to ensure we see things
	spaceShip* closest_ship = NULL;

	vector<spaceShip*>::iterator ships_i;
	for (ships_i = puppet->sector->ships.begin() ;
		ships_i != puppet->sector->ships.end() ;
		ships_i++)
	{
			if( (*ships_i)->team_id != puppet->team_id )
				if( cansee((*ships_i)->pos))
				{
					SPfloat this_dist = puppet->pos.dist( (*ships_i)->pos );
					
					if( this_dist < closest_dist)
						closest_ship = (*ships_i);
				}
	}

	return closest_ship;
}





void yarrAI :: avoid_bullets()
{
	/// Look around and find the ship closest to this one in its vision
	///
	/// Returns a ship pointer on success,
	/// returns a NULL POINTER on failure

	SPfloat closest_dist = feel_dist + 1; // higher than vision to ensure we see things
	spaceBullet* closest_bullet = NULL;

	vector<spaceWeapon*>::iterator weapons_i;
	for (weapons_i = puppet->sector->weapons.begin() ;
		 weapons_i != puppet->sector->weapons.end() ;
		 weapons_i++)
	{
		vector<spaceBullet>::iterator bullet_i;
		for(bullet_i = (*weapons_i)->part.begin() ;
			bullet_i != (*weapons_i)->part.end() ;
			bullet_i++)
		{
			
		//if( (*bullets_i)->alive == true )
		// don't collide with parent ship or allies
		if( bullet_i->team_id != puppet->team_id )
			if( canfeel(bullet_i->pos))
				if( puppet->pos.dist( bullet_i->pos ) < closest_dist)
					closest_bullet = bullet_i;
		}
	}

	if( closest_bullet != NULL)
	{
		avoid(closest_bullet->pos);
		thrust();
	}
}

void yarrAI :: avoid_ships()
{
	/// Look around and find the ship closest to this one in its vision
	///
	/// Returns a ship pointer on success,
	/// returns a NULL POINTER on failure

	SPfloat closest_dist = feel_dist + 1; // higher than vision to ensure we see things
	spaceShip* closest_ship = NULL;

	vector<spaceShip*>::iterator ships_i;
	for (ships_i = puppet->sector->ships.begin() ;
		 ships_i != puppet->sector->ships.end() ;
		 ships_i++)
	{
		// NOTE : this includes allies
		if( canfeel((*ships_i)->pos) )
		{
			if( puppet->pos.dist( (*ships_i)->pos ) < closest_dist)
				closest_ship = (*ships_i);
		}
	}

	if( closest_ship != NULL)
	{
		avoid(closest_ship->pos);
		thrust();
	}
}


void yarrAI :: align(SPfloat desired)
{
	SPfloat current = puppet->heading;

	// first see if we should turn at all
	//if (wrap(desired - current) < puppet->turnspeed) return;

	SPfloat turnamt = wrap(desired - current);

	// turn to face the target
	if (desired > current) {
		if (turnamt > 180) turn(-turnamt);
		else turn(turnamt);
	}
	else {
		if (turnamt < 180) turn(turnamt);
		else turn(-turnamt);
	}
}

void yarrAI :: seek(spacePoint dest)
{
	/// turn towards a point and keep thrusting to it until
	/// we get there

	SPfloat desired = puppet->pos.angleto(dest);
	SPfloat current = puppet->heading;

	
	// first see if we should turn at all
	//if (wrap(desired - current) < puppet->turnspeed) return;

	// turn to face the target
	if (desired > current) {
		if (wrap(desired - current) > 180) turn(RIGHT);
		else turn(LEFT);
	}
	else {
		if (wrap(current - desired) > 180) turn(LEFT);
		else turn(RIGHT);
	}

	// thrust towards the target
	//thrust();
}

void yarrAI :: avoid(spacePoint dest)
{
	/// turn towards the opposite direction from a point
	/// and keep thrusting... and thrusting...

	SPfloat desired = wrap( puppet->pos.angleto(dest) - 180 );
	SPfloat current = puppet->heading;

	// turn to face the target
	if (desired > current) {
		if (wrap(desired - current) > 180) turn(RIGHT);
		else turn(LEFT);
	}
	else {
		if (wrap(current - desired) > 180) turn(LEFT);
		else turn(RIGHT);
	}

	// thrust towards the target
	//thrust();
}

spacePoint yarrAI :: lead(spaceObj* target, SPfloat myspeed)
{
	// figure out the place where the target is GOING to be
	// and then return the point
	spacePoint leadpoint;

	// distance / speed = time to target
	SPfloat timetotarget = puppet->pos.dist( target->pos ) / myspeed;
	
	// FIXME : this is way too exact... the relative_vel should be offset by a random number 
	// to simulate inexact aiming
	spacePoint relative_vel = target->vel - puppet->vel;

	leadpoint.x = target->pos.x + (relative_vel.x * timetotarget);
	leadpoint.y = target->pos.y + (relative_vel.y * timetotarget);

	return leadpoint;
}


void yarrAI :: attack(spaceShip* enemy)
{
	/// Lead an enemy with turrets and attack. Does not make the ship pursue
	/// uses : turrets, fire
	// FIXME BROKEN : leading for guns is currently broken, need to make it
	//		take into account the attacker's velocity
	SPfloat gaugedspeed = gunspeed + (rand()%(int)(inaccuracy * 2) - inaccuracy);

	spacePoint leadpos = lead(enemy, gaugedspeed);
	
	aimat(leadpos);

	if( puppet->pos.dist(leadpos) <= gunrange )
		puppet->fire();
}

void yarrAI :: wander()
{
	// find a random point in space nearby and seek it
	// every 10 seconds
	if(time % 600 == 0)
	{
		if(rand()%100 <= 80)
		{
			target.x = (rand()%int(yarrAIvars["wander_distance"] * 2)
				- yarrAIvars["wander_distance"]) + puppet->pos.x;

			target.y = (rand()%int(yarrAIvars["wander_distance"] * 2)
				- yarrAIvars["wander_distance"]) + puppet->pos.y;
		}

		dest.x = (rand()%int(yarrAIvars["wander_distance"] * 2)
				- yarrAIvars["wander_distance"]) + puppet->pos.x;

		dest.y = (rand()%int(yarrAIvars["wander_distance"] * 2)
				- yarrAIvars["wander_distance"]) + puppet->pos.y;
	}

	seek(dest);
	aimat(target);
	thrust();	// keep on truckin'
}






void yarrAISeeker :: think()
{
	/// just seek the target!
	seek(lockon_target->pos);
}

void yarrAIWanderer :: think()
{
	// avoiding attacks is more important than being a vagrant
	avoid_bullets();
	wander();

	// look for something to kill
	spaceShip* enemy = getclosest();

	if( enemy != NULL )
	{
		attack(enemy);
	}
}

void yarrAI :: pursue(spaceShip* enemy)
{
	// lead the enemy
	spacePoint leadpos = lead(enemy, puppet->maxvel);
		
	// seek the lead, unless we get too close
	//if(puppet->pos.dist(enemy->pos) > standoff_range)
	seek(leadpos);
	//else
	//	avoid(leadpos);
	thrust();
}

void yarrAIPsycho :: think()
{
	// save yourself!
	avoid_bullets();

	// look for something to kill
	//getvisible();
	//spaceShip* enemy = seen.back();
	spaceShip* enemy = getclosest();

	if( enemy != NULL )
	{
		pursue(enemy);
		attack(enemy);
	}
	else
		wander();	// or just wander around if nothing's around
}







void spaceTeam :: init()
{
	id = 0;
	alive = true;
	color = spaceColor(1,1,1);
}


void spaceTeam :: add_ship(yarrAI* newpirate)
{
	if(newpirate == NULL)
	{
		cout << "Error : attempted to add null pointer to spaceTeam.\n";
		return;
	}

	// make the new pirate fly the team colors
	//newpirate->team = this;
	newpirate->puppet->team_id = id;
	newpirate->puppet->scolor = color;
	
	push_back(newpirate);

	// now, redistribute grog
	// for now, we're doing it commie style!
	unsigned int totalgrog = 0;

	/*
	vector<yarrAI*>::iterator matey;
	for(matey = begin();
		matey != end();
		matey++)
	{
			totalgrog += (*matey)->puppet->grog;
	}

	unsigned int grog_per_ship = totalgrog / mates.size();

	for(matey = begin();
		matey != end();
		matey++)
	{
			(*matey)->puppet->grog = grog_per_ship;
	}
	*/
}

void spaceTeam :: remove_ship(yarrAI* oldpirate)
{
	// first, find the ship
	vector<yarrAI*>::iterator pirate_i;
	// FIXME : why is this broken!?!?
	
	for(pirate_i = begin(); pirate_i != end(); pirate_i++)
	{
		if((*pirate_i) == oldpirate)
		{
			erase(pirate_i);
			return;
		}
	}

	/*
	pirate_i = find(mates.begin(), mates.end(), oldpirate);
	
	if(pirate_i != mates.end()) // sort of like != NULL in STL world
	{
		mates.erase(pirate_i);
		return;
	}
	*/

	
	// 'kill' the team if it has no more members
	if(size() <= 0)
		alive = false;
}

void spaceTeam :: handle()
{
	// clean out null ships
	// HACK : this should be done when the ship decides that it dies,
	//	but I guess that just can't happen
	vector<yarrAI*>::iterator ships_i;
	for (ships_i = begin();
		 ships_i != end();
		 ships_i++)
	{
		 if((*ships_i) == NULL)
		 {
			 erase(ships_i);
			 ships_i--;
		 }
	}
}
