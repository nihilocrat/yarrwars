#if !defined YARR_AI_INCLUDED
#define YARR_AI_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "spaceobj.h"
#include "yarrCraft.h"

#include <list>


class yarrAI
{
public:
	yarrCraft *puppet;
	unsigned long int time; // replace with something real later

	// these are variables I should cleverly implement later
	spaceDirection turndir;

	yarrAI(yarrCraft *np = NULL){ bind(np);  time = 0; };
	~yarrAI(){};

	void init();	// for setting up variables at spawn time
	void bind(yarrCraft *np);
	virtual void think(){};
	void handle();

	// sensor fuctions
	bool cansee(spacePoint thing);
	bool canfeel(spacePoint thing);

	void getvisible();
	spaceShip* getclosest();



	// motor functions
	void stop();
	void thrust();
	void turn(spaceDirection dir);
	void turn(SPfloat difference);
	void aimat(spacePoint target);

	// simple functions
	void align(SPfloat desired);
	void seek(spacePoint dest);
	void avoid(spacePoint dest);

	spacePoint lead(spaceObj* target, SPfloat myspeed);

	// 'advanced' functions
	void pursue(spaceShip* enemy);
	void attack(spaceShip* enemy);
	void flock();
	void wander();

	void avoid_bullets();
	void avoid_ships();
	
	/*
	'high level' functions
	void hunt();
	void attack();
	void defend();
	*/

	// Moved from yarrAIPirate
	bool isGrouped, isLeader;
	
	// pointers to *sad animals*
	spaceShip *mEnemy, *mNulltarget;


protected:		// modularize!
	
	void unlock_motors();

	vector< yarrCraft* > seen;

	spacePoint dest, target;
	SPfloat view_dist, feel_dist;
	SPfloat gunrange, gunspeed;
	SPfloat inaccuracy, standoff_range;
	bool lock_thrust, lock_turn, lock_turrets;
};

class yarrAISeeker : public yarrAI
{
public:
	void lockon(spaceObj* obj){ lockon_target = obj; };
	void think();

	spaceObj* lockon_target;
};

class yarrAIWanderer : public yarrAI
{
public:
	void think();

};

class yarrAIPsycho : public yarrAI
{
public:
	void think();

};




class spaceTeam : public vector<yarrAI*>
{
public:
	spaceTeam(unsigned int nid=0){ id = nid; alive = true; clear(); };
	void init();

	void add_ship(yarrAI* newpirate);
	void remove_ship(yarrAI* oldpirate);

	void handle();
	

	bool alive;
	unsigned int id;
	spaceColor color;
	yarrAI* leader;

	//vector<yarrAI*> mates;	// this might seem dumb, but it's safer than overriding vector<>
};


#endif