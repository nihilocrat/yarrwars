#include <iostream>
#include <string>

#include <algorithm>

#include "spaceobj.h"

#include "spacesector.h"

#include "yarrCraft.h"


// map containing game logic variables
map<string, SPfloat> spaceGamevars;


// FIXME : HACK : these should only temporarily be here
//Material parameters for spheres
GLfloat sphereambient[] = {0.2, 0.2, 0.4, 1.0}; //ambient reflectance
//GLfloat spherediffuse[] = {0.8, 0.0, 0.2, 1.0}; //diffuse reflectance
GLfloat spherespecular[] = {0.2, 0.2, 0.2, 1.0}; //specular reflectance
GLfloat sphereshininess = 25.; //shininess: higher exponent means shinier


//Material parameters for cones
GLfloat coneambient[] = {0.1, 0.1, 0.1, 0.3}; //ambient reflectance
//GLfloat conediffuse[] = {0.6, 0.6, 0.6, 1.0}; //diffuse reflectance
GLfloat conespecular[] = {0.4, 0.4, 0.4, 1.0}; //specular reflectance
GLfloat coneshininess = 50.; //shininess: higher exponent means shinier



/********************************************************************************************************/
/* Container Objects (spaceSector, spaceUniverse)                                                       */
/********************************************************************************************************/


spaceSector :: spaceSector(spacePoint npos)
{
	univpos = npos;

	// HACK HACK HACK
	// ...or is it?
	teamcounter = 0;


	// render settings
	//render_lighting = true;
	ship_shading = GL_SMOOTH;
	labels = true;

	overview_enabled = false; // FIXME
	
	LockDraw = false;

	// game logic settings
	cycle = 0;
}

void spaceSector :: init()
{
	// add some friction...
	univfrict = spaceGamevars["friction"];

	// set grid size
	gridspacing = spaceGamevars["grid_spacing"];

	// set booleans
	ship_collisions = spaceGamevars["ship_collisions"];
}


void spaceSector :: add_ship(spaceShip *neu)
{
	// check for null pointers so that we don't have
	// to keep checking for them everywhere else
	//nullcheck(neu);
	if(neu == NULL)
	{
		cout << "attempt to insert null spaceShip pointer failed\n";
		return;
	}

	ships.push_back(neu);
}


void spaceSector :: add_weapon(spaceWeapon *neu)
{
	if(neu == NULL)
	{
		cout << "attempt to insert null spaceWeapon pointer failed\n";
		return;
	}

	weapons.push_back(neu);
}

void spaceSector :: add_fx(spaceEmitter *neu)
{
	if(neu == NULL)
	{
		cout << "attempt to insert null FX pointer failed\n";
		return;
	}

	fx.push_back(neu);
}

void spaceSector :: add_planet(spacePlanet *neu)
{
	if(neu == NULL)
	{
		cout << "attempt to insert null spacePlanet pointer failed\n";
		return;
	}

	planets.push_back(neu);
}

/*
void spaceSector :: add_group(spaceGroup neu)
{
	if(neu == NULL)
	{
		cout << "attempt to insert null spaceGroupt pointer failed\n";
		return;
	}

	groupts.push_back(neu);
}
*/


void spaceSector :: render_frame()
{

	glEnable(GL_LIGHTING);

	//apply material properties for planets
	glMaterialfv(GL_FRONT, GL_AMBIENT, sphereambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, spherespecular);
	glMaterialf(GL_FRONT, GL_SHININESS, sphereshininess);

	vector<spacePlanet*>::iterator planet_i;
	for (planet_i = planets.begin() ;
		 planet_i != planets.end() ;
		 planet_i++)
	{
		if(!overview_enabled)
			(*planet_i)->draw();
		// FIXME : else... overview_draw the planet


		if(labels == true)
		{
			// FIXME : put these disables and enables elsewhere
			glDisable(GL_LIGHTING);
			(*planet_i)->drawlabel();
			glEnable(GL_LIGHTING);
		}
	}


	//apply material properties for ships
	glShadeModel(ship_shading);
	glMaterialfv(GL_FRONT, GL_AMBIENT, coneambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, conespecular);
	glMaterialf(GL_FRONT, GL_SHININESS, coneshininess);


	for (ships_i = ships.begin() ;
		 ships_i != ships.end() ;
		 ships_i++)
	{

		if((*ships_i)->alive == true)
		{
			// FIXME : move draw-related stuff to Camera / Scene manager when it exists
			if(!overview_enabled)
				(*ships_i)->draw();
			else
				(*ships_i)->overview_draw();
		}
	}


	glShadeModel(GL_SMOOTH);


	glDisable(GL_LIGHTING);

	// draw nonlit stuff here

	vector<spaceWeapon*>::iterator weapons_i;
	for (weapons_i = weapons.begin() ;
		 weapons_i != weapons.end() ;
		 weapons_i++)
	{
		// FIXME : move draw-related stuff to Camera / Scene manager when it exists
		if(!overview_enabled)
			(*weapons_i)->draw();
		else
			(*weapons_i)->overview_draw();

	}


	
	for (fx_i = fx.begin() ;
		 fx_i != fx.end() ;
		 fx_i++)
	{
		// FIXME : move draw-related stuff to Camera / Scene manager when it exists
		if(!overview_enabled)
			(*fx_i)->draw();
	}
}



void spaceSector :: render_overview()
{
	glDisable(GL_LIGHTING);
	
	glShadeModel(GL_FLAT);

	vector<spacePlanet*>::iterator planet_i;
	for (planet_i = planets.begin() ;
		 planet_i != planets.end() ;
		 planet_i++)
	{
		// FIXME : else... overview_draw the planet
		(*planet_i)->overview_draw();

		if(labels == true)
			(*planet_i)->drawlabel();
	}


	for (ships_i = ships.begin() ;
		 ships_i != ships.end() ;
		 ships_i++)
	{

		if((*ships_i)->alive == true)
		{
				(*ships_i)->overview_draw();
		}
	}





	vector<spaceWeapon*>::iterator weapons_i;
	for (weapons_i = weapons.begin() ;
		 weapons_i != weapons.end() ;
		 weapons_i++)
	{
			(*weapons_i)->overview_draw();

	}


	// FX doesn't get drawn! haha!
	
	// let's clean up after ourselves
	glEnable(GL_LIGHTING);
	glShadeModel(GL_SMOOTH);
}




void spaceSector :: handle_ships()
{
	// go through the array and call draw functions

	for (ships_i = ships.begin() ;
		 ships_i != ships.end() ;
		 ships_i++)
	{

		if((*ships_i)->alive == true)
		{
			// tell this ship it's not hit to refresh AI
			(*ships_i)->attacker = 0;

			// physics, moving, handling, and other stuff for this ship
			(*ships_i)->move();
			if(univfrict > 0) (*ships_i)->friction(univfrict); // friction for frictioned universes
			(*ships_i)->handle();
		
		
			// now handle inter-ship collisions
			if(ship_collisions)
			{
				vector<spaceShip*>::iterator collide_ship;
				for( collide_ship = ships_i + 1;
					 collide_ship != ships.end();
					 collide_ship++)
				{
					if( (*ships_i)->collide((*collide_ship)) )
					{
						// HACK : quick and dirty : just switch their vels
						spacePoint tempvel = (*ships_i)->vel;
						(*ships_i)->vel = (*collide_ship)->vel;
						(*collide_ship)->vel = tempvel;

						(*collide_ship)->vel.x *= 1.2;
						(*collide_ship)->vel.y *= 1.2;
							
						(*ships_i)->vel.x *= 1.2;
						(*ships_i)->vel.y *= 1.2;
					}
				}
			}
		}
		else
		{
			// erase dead ships - same idea as killing dead particles
			//(*ships_i)->team->remove_ship((*ships_i));  // this looks really silly, doesn't it?
			ships.erase(ships_i);
			ships_i--;	// NOTE : for some bizarre reason, doing this avoids nasty pointer issues
		
		}
	}
}


void spaceSector :: handle_weapons()
{
	// go through the array and call draw functions

	vector<spaceWeapon*>::iterator weapons_i;
	for (weapons_i = weapons.begin() ;
		 weapons_i != weapons.end() ;
		 weapons_i++)
	{
		(*weapons_i)->tick();

		// remove this weapon if no bullets exist AND the owner is dead
		/*
		if( !(*weapons_i)->parent->alive )
			if( !(*weapons_i)->alive )
			{
				weapons.erase(weapons_i);
				weapons_i--;
				continue;
			}
		*/

		// do collision detection here
		// DEBUG : let's be lazy and do it with EVERY ship!
		vector<spaceBullet>::iterator bullet_i;
		spaceShip* victim;

		for(bullet_i = (*weapons_i)->part.begin() ;
			bullet_i != (*weapons_i)->part.end() ;
			bullet_i++)
		{
			victim = check_collide( bullet_i );

			if(victim != NULL)
			{
				(*weapons_i)->damage(bullet_i, victim);
				
				// then remove this bullet
				// NOTE / HACK : instead of erasing it here and messing up the iterator,
				//	let's just set it's life to 0 so it will automatically get removed
				bullet_i->life = 0;
			}
		}// bullet collision loop
	} // weapon handle loop
}

spaceShip* spaceSector :: check_collide(spaceBullet *bullet_i)
{
	for (ships_i = ships.begin() ;
		ships_i != ships.end() ;
		ships_i++)
	{
		if( (*ships_i)->alive == true )
			// don't collide with parent ship or allies
			if( (*ships_i)->team_id != bullet_i->team_id )
				if( bullet_i->collide((*ships_i)) )
				{
					return (*ships_i);
				}
	} // ship loop

	return NULL;
}

void spaceSector :: handle_fx()
{	
	for (fx_i = fx.begin() ;
		 fx_i != fx.end() ;
		 fx_i++)
	{

		(*fx_i)->tick();

		// remove this fx if no particles exist AND the owner is dead
		// FIXME : spruce this up a little to account for parentless FX
		/*
		if( !(*fx_i)->alive )
			if( !(*fx_i)->parent->alive )
			{
				fx.erase(fx_i);
				fx_i--;
				continue;
			}
		*/
	}
}

void spaceSector :: handle_planets()
{
	// go through the array and call draw functions

	// FIXME : nothing to be done for planets right now
	//		I guess handle respawn stuff here
}



void spaceSector :: lock_display()
{
	// tell everything else that we've drawn for this frame
	LockDraw = true;
}

void spaceSector :: unlock_display()
{
	// unlock drawing privleges
	LockDraw = false;
}

void spaceSector :: handle_gamelogic()
{
	// every so often (10 seconds at 60 fps), decrease grog
	if(cycle % 600 == 0)
	{	

		for (ships_i = ships.begin() ;
			ships_i != ships.end() ;
			ships_i++)
		{
			(*ships_i)->grog -= spaceGamevars["grog_rate"];
			(*ships_i)->fame += spaceGamevars["fame_rate"];

			
			// heal ships that need it
			if((*ships_i)->life < (*ships_i)->maxlife)
			{
				(*ships_i)->life += spaceGamevars["heal_per_turn"];
				(*ships_i)->grog -= spaceGamevars["grog_per_heal"];
			}


			// kill off grogless ships
			(*ships_i)->handle_health();
		}
	}

	
	
	cycle++;
}




void spaceUniverse :: makesectors(unsigned int univwidth, unsigned int univheight)
{
	// FIXME: throw an exception if univwidth/height is < 1

	unsigned int x, y;
	for(y = 0; y < univheight ; y++)
	{
		for(x = 0; x < univwidth ; x++)
		{
			spaceSector newsec(spacePoint(x, y));

			sectors[y].push_back(newsec);
		}
	}
}

/*
spaceShip* getship(string query)
{
	unsigned int i;
	for (shipiter = ships.begin(); shipiter != ships.end(); shipiter++)
	{
		if(shipiter->name == query)
			return shipiter;
	}
}
*/
