#if !defined(__SPACE_SECTOR_INCLUDED)
#define __SPACE_SECTOR_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <math.h>

// STL fun!
#include <vector>
#include <map>
#include <list>
using namespace std;

#include "spaceobj.h"

#define G_max_ships_in_sector	255
#define	G_labels				true



/********************************************************************************************************/
/* Container Objects (spaceTeam, spaceSector)                                                            */
/********************************************************************************************************/


class spaceSector
{
	/// a sector of space : handles collision, rendering, etc.
	/// the member functions should be called by a higher-level 'scene manager' (geometry-specific)
	/// to figure out what things should be rendered on the screen

	public:
		spaceSector(spacePoint npos=spacePoint(0,0));
		~spaceSector(){};
		void init();	// post-config-loading initialization

		void add_ship(spaceShip *neu);
		void add_weapon(spaceWeapon *neu);
		void add_fx(spaceEmitter *neu);
		void add_planet(spacePlanet *neu);

		void render_frame();
		void render_overview();

		void handle_ships();
		void handle_weapons();
		void handle_fx();
		void handle_planets();
		void handle_gamelogic();

		void lock_display();
		void unlock_display();

		spaceShip* check_collide(spaceBullet *bullet_i);

		
		/// location within the universe
		spacePoint univpos;
		SPfloat univfrict;

		// determines grid center
		spacePoint center;
		SPfloat gridspacing;
		void drawgrid(spacePoint target);
		void overview_drawgrid(spacePoint target, SPfloat zoomamt);

		bool labels;
		bool overview_enabled;
		bool ship_collisions;

		unsigned int teamcounter;
		
		// how many cycles have progressed in the game
		unsigned long cycle; // at 60 fps, the game will be able to play for ~800 days until this breaks

		vector< spaceShip* > ships;
		spaceShip* mostfamous;
		//unsigned int ships_present;
		//spaceShip* ships[G_max_ships_in_sector];

		vector< spaceWeapon* > weapons;
		//unsigned int weapons_present;
		//spaceWeapon* weapons[G_max_ships_in_sector];

		vector< spaceEmitter* > fx;

		vector< spacePlanet* > planets;
		//unsigned int planets_present;
		//spacePlanet* planets[G_max_ships_in_sector];
	protected:
		bool LockDraw;

		int ship_shading;
		
		// fun iterators so we don't have to keep redeclaring them
		vector< spaceShip* >::iterator ships_i;
		vector<spaceEmitter*>::iterator fx_i;
};


class spaceUniverse
{
	/// contains all space sectors as well as all objects in existence
	/// it handles which sector objects should be put in
	/// this is the closest thing to a frontend for the game logic world

	void handle();

	void makesectors(unsigned int univwidth, unsigned int univheight);

	// a vector of vectors of sectors... haaa
	vector < vector <spaceSector> > sectors;

	vector <spaceShip> ships;
	vector <spacePlanet> planets;
};


#endif