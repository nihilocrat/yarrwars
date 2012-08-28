#ifndef SPACEOBJ_INCLUDED
#define SPACEOBJ_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "spaceinc.h"
#include <math.h>

#include <iostream>
#include <string>
#include <vector>

using namespace std;


/********************************************************************************************************/
/* a few globals                                                                            */
/********************************************************************************************************/

#define G_default_parts		100 // default number of particles in spaceEmitters


/********************************************************************************************************/
/* some extra math functions                                                                            */
/********************************************************************************************************/

// degree and radian conversion.... using ratios is quicker!
// I would macro these if I didn't hate macros
SPfloat radians(SPfloat degrees);
SPfloat degrees(SPfloat radians);

SPfloat wrap(SPfloat angle);

/********************************************************************************************************/
/* Attribute Objects (Point, Vector, etc.)                                                              */
/********************************************************************************************************/
typedef enum spaceDirection
{
	NEUTRAL,
	LEFT,
	RIGHT
};

class spacePoint
{
	public:
		SPfloat x, y; // FIXME : make these proctected eventually

		spacePoint(SPfloat nx=0, SPfloat ny=0);

		// operator overrides
		bool operator== (spacePoint p2);
		spacePoint spacePoint :: operator+ (spacePoint p2);
		spacePoint spacePoint :: operator- (spacePoint p2);
		
		/* for STL
		spacePoint(pair<SPfloat, SPfloat>)
		{
			x = nx;
			y = ny;
		};
		*/

		// find the heading to another object
		SPfloat angleto(spacePoint obj2);
		
		// find the distance to another object
		SPfloat dist(spacePoint obj2);

};

// colors!
class spaceColor
{
	public:
		SPfloat r, g, b;

		spaceColor(SPfloat nr = 1.0, SPfloat ng = 1.0, SPfloat nb = 1.0);
};



/********************************************************************************************************/
/* Basic Objects (spaceObj, spaceShip)                                                                */
/********************************************************************************************************/

class spaceBit
{
	/// in between a spacePoint and a spaceObj
	/// a point with various other attributes that
	/// make it handy for particle systems
	spaceBit(){};
	~spaceBit(){};

	bool alive;
	unsigned int life;

	spaceColor color;
};

class spaceObj
{
	public:
		//Basic immobile game object
		spaceObj();
		~spaceObj(){};


		// defined in render functions
		virtual void draw();
		void drawlabel();

		// handle various developer-defined stuff
		virtual void handle(){};

	//protected:
		spacePoint pos;
		spacePoint vel;
		SPfloat heading;
		SPfloat maxvel;
		SPfloat size;
		spaceColor color;
		bool alive;
		unsigned int life;
		unsigned int maxlife;
		string name;
};


class spaceGroup : public vector<spaceObj*>
{
public:
	void append(spaceObj* newguy);
	void remove(spaceObj* oldguy);
	void calc_avg_heading();
	void redistribute();

	unsigned int id;
	spaceColor color;
	spaceObj* leader;
	bool alive;

	SPfloat avg_heading;
};

class spacePlanet : public spaceObj
{
	/// Big, immobile things!
public:
	spacePlanet(string nname = "Strongbadia", spacePoint npos = spacePoint(), SPfloat nsize=200,
		spaceColor ncolor=spaceColor());


	void draw();
	void overview_draw();

};



class spaceShip : public spaceObj
{
    ///Basic mobile game object
	/// a class for any sort of mobile object that can turn, thrust, and so on
public:
    spaceShip();
	spaceShip(string newname, spacePoint npos, SPfloat nheading, SPfloat nturnspeed, SPfloat nimpulse, SPfloat nmaxvel,
			spaceColor ncolor, spaceColor nscolor);

    void move();
    void friction(SPfloat amount);
	void push(SPfloat theta, SPfloat magnitude);

	void thrust();
	void limitvel();
	void turn(spaceDirection direction);
	void turn(SPfloat maxturn);
	
	void handle_health();
	bool collide(spaceObj* other);

	virtual void draw(); //FIXME : move to render header
	virtual void overview_draw();

	//protected:
		SPfloat impulse, turnspeed;
		spaceColor scolor;

		unsigned int team_id;
		spaceShip *attacker;		// HACK	: thing for telling AI when we're hit
		spaceShip *enemy;			// HACK : thing for telling AI who we're targetting
		spaceShip *parent;

		// HACK : this is to get spacesector to cooperate
		unsigned int grog, maxgrog;
		unsigned int fame;
		spaceGroup *group;
};


class spaceEmitter : public spaceObj
{
public:
	/// something that emits particles of some sort
	spaceEmitter(){};
	spaceEmitter(unsigned int nmax_life, unsigned int nmax_recharge,
						 spaceColor ncolor);
	~spaceEmitter();

	void fire();
	void tick(unsigned int numticks = 1);
	void move(spacePoint newpos);
	void draw();
	
	// functions to be overloaded by child classes
	virtual void particle_draw(spaceObj *p_part) {}
	virtual void emit(spaceObj *bit) {}


	unsigned int max_parts; // number of particles POSSIBLE
	unsigned int num_parts; // CURRENT number of particles
	
	//spacePoint part[G_default_parts];
	//unsigned int part_life[G_default_parts];
	
	// array of particles
	vector<spaceObj> part;
	
	unsigned int max_life; // maximum particle lifetime FIXME yadda yadda
	unsigned int recharge, max_recharge; // limits the rate of particle emission

	spaceColor pcolor; // primary color (FIXME make this dynamic)
	SPfloat psize;	// particle size (FIXME make this dynamic)
	
	spaceObj *parent;
};


/*
// non-STL
class spaceEmitter : public spaceObj
{
public:
	// something that emits particles of some sort
	spaceEmitter(unsigned int nmax_parts = G_default_parts, spaceColor ncolor = spaceColor(0,0,0));
	~spaceEmitter();

	void fire();
	void tick(unsigned int numticks = 1);
	void move(spacePoint newpos);
	void draw();
	
	// functions to be overloaded by child classes
	virtual void particle_draw(unsigned int i){};
	virtual void emit(spacePoint *bit){};


	unsigned int max_parts; // number of particles POSSIBLE
	unsigned int num_parts; // CURRENT number of particles
	
	// array of particles
	vector<spaceObj> part;

	unsigned int max_life; // maximum particle lifetime FIXME yadda yadda
	unsigned int recharge, max_recharge; // limits the rate of particle emission

	spaceColor pcolor; // primary color (FIXME make this dynamic)
	SPfloat psize;	// particle size (FIXME make this dynamic)
};
*/


class spaceBullet : public spaceShip
{
    ///''' Projectile object '''
	/// FIXME : might want to derive this from a simpler class for performance boost (?)
public:
	virtual void draw();
	virtual void overview_draw();

	spaceShip *owner;
};


/********************************************************************************************************/
/* Advanced Objects (spaceCraft, spaceTurret)                                                            */
/********************************************************************************************************/

// we're not using a templateized spaceEmitter because, apparently,
// using templates craps stuff up down the line
 class spaceWeapon : public spaceObj
{
public:
	/// something that emits particles of some sort
	spaceWeapon(){};
	spaceWeapon(unsigned int nmax_life, unsigned int nmax_recharge,
						 SPfloat nbulletspeed, SPfloat nscatter, spaceColor ncolor);
	~spaceWeapon();

	void bind(spaceShip *nparent, spacePoint noffset);

	void fire();
	void tick(unsigned int numticks = 1);
	void move(spacePoint newpos);

	void draw();
	void overview_draw();
	
	// functions to be overloaded by child classes
	//virtual void particle_draw(unsigned int i){};
	virtual void emit(spaceBullet *bit);

	

	// FIXME : use a better collide function! PLEASE!
	void collide_list(spaceObj* list[], unsigned int listsize);
	void damage(spaceBullet *bullet_i, spaceShip *victim);

	unsigned int max_parts; // number of particles POSSIBLE
	unsigned int num_parts; // CURRENT number of particles
	
	vector<spaceBullet> part; // particle objects

	// make iterators so I don't have to keep defining them later on
	//vector<spaceBullet>::iterator part_i;

	
	unsigned int max_life; // maximum particle lifetime FIXME yadda yadda
	unsigned int recharge, max_recharge; // limits the rate of particle emission

	spaceColor pcolor; // primary color (FIXME make this dynamic)
	SPfloat psize;	// particle size (FIXME make this dynamic)

	spaceShip* parent;
	spacePoint offset;
	SPfloat bulletspeed, damage_amt;
	SPfloat scatter;
};

class spaceTurret : public spaceShip
{
public:
	spaceTurret(){ parent = NULL; };
	spaceTurret(SPfloat turnrate, SPfloat size, spaceWeapon ngun);
	virtual ~spaceTurret(){};

    void bind(spaceShip *nparent, spacePoint noffset);

    //void fire()
	//{
    //    for weapon in _.weapons:
    //        weapon.fire()
	//}

    void move();

	void turn(spaceDirection direction);
	void track(spacePoint target);
	void fire();

	virtual void draw();

	//spaceShip *parent; // pointer to the parent object
	spacePoint offset;
	SPfloat rotation;	// how much the turret is rotation in comparison to its parent

	spaceWeapon gun;	// FIXME : for now, only one gun
};


#endif

