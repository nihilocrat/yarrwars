/*
	Yarr Wars
	by Kenny Backus (c) 2006
	4/25/06

	main.cpp

	This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

// modify this for new releases
const char* windowstr = "Yarr Wars 0.1.0a";


// HEADERS

#include <windows.h> // for Sleep and wgl extensions
#include <math.h> // for sin and cos
#include <time.h>
#include "glut.h"	 // standard GLUT functions

#include "space.h"	// .includes for most of the SPACE stuff
#include "spacerender3d.h" // point class
#include "spaceworld3d.h" // camera stuff and so on
#include "yarrAI.h"		// AI for the game
#include "yarrAIPirate.h"
#include "yarrCraft.h"	// game objects based on SPACE objects

#include "spaceoverlay.h"	// HUD overlay

// Silly stolen stuff
#include "lib\BitmapTexture.h"	// texture loading functions
#include "lib\resource.h"
#include "lib\EasyFont.h"
//////

#include <iostream>

#include <list>
#include <algorithm>
using namespace std;


// WGL extension for windows boxen
#ifdef __WINDOWS

// Vsync stuff
typedef BOOL (APIENTRY *PFNWGLSWAPINTERVALFARPROC)( int );
PFNWGLSWAPINTERVALFARPROC wglSwapIntervalEXT = 0;

void setVSync(int interval)
{
  const char *extensions = (const char*)glGetString( GL_EXTENSIONS );

  if( strstr( extensions, "WGL_EXT_swap_control" ) == 0 )
  {
	cout << "Error: WGL_EXT_swap_control extension not supported on your computer.\n";
    return;
  }
  else
  {
    wglSwapIntervalEXT = (PFNWGLSWAPINTERVALFARPROC)wglGetProcAddress( "wglSwapIntervalEXT" );

    if( wglSwapIntervalEXT )
      wglSwapIntervalEXT(interval);
  }
}

#endif



// GLOBAL VARIABLES

GLfloat mousex,mousey;

float fovangle = 45;
float aspratio = 1.33;	// 4/3 aspect ratio that standard resolutions use

float sleeptime = 15;

unsigned int resx = 800;
unsigned int resy = 600;

unsigned int xhalf = (unsigned int) resx / 2;
unsigned int yhalf = (unsigned int) resy / 2;


// determine which AI subclass to use
typedef yarrAIPirate yarrwarsAI;

// game globals
extern map<string,SPfloat> spaceGamevars;
extern map<string,SPfloat> yarrAIvars;
extern map<string,SPfloat> yarrShipvars;

// GLOBAL SETTINGS ************************************************************************


// Display settings
bool lighting = true; // lighting
bool labels = true;

// camera stuff
float zoomamt = 750.0;	// distance of camera from its target
float camheading = 180.;
float camelev = 25.;

float zoomspeed = 5.0;
float rotspeed = 1.0;
float elevspeed = 0.25;
Point3D eye;
Point3D target;
float zmod = 0;



cameraMode cam_orientation = FREE_CHASE;

// variables for dumping projection, model, and viewport matrices to
double modelview[16], projection[16];
int viewport[4];

// view mode stuff
bool overview_enabled = false;
unsigned int gamespeed = 0;
unsigned int max_gamespeed = 28;
unsigned int old_gamespeed = 1;

// FPS stuff
unsigned long frame=0, cycle=0;
unsigned int timenow,timelastframe=0, frametime, ms_last_frame=0;
float fps;

// overlay stuff

extern int text_x;
extern int text_y;
bool instructions_enabled = true;

// setup universe

spaceSector starbucks_sector;
int sector_size;


yarrCraft fooship("FooShip", spacePoint(50, 50), 0, spaceColor(0.4, 0.2, 1.0),
														  spaceColor(0.2, 1.0, 0.4));

// this is fooship's false AI... it doesn't exist in the random ai list
// so it doesn't actually get called. This is here simply so fooship can be
// in a team
yarrwarsAI fooai;

// a little hacky, but it works
yarrCraft notarget("No Target", spacePoint(0,0), 0, spaceColor(1,1,1), spaceColor(1,1,1));


// vectors for random names
vector<string> pirateFirstnames;
vector<string> pirateLastnames;
vector<string> planetnames;

unsigned int num_rplanets;
const unsigned int possible_planets = 64;
yarrPlanet randomplanets[possible_planets];

unsigned int num_rships;
unsigned int teamcounter = 0;
const unsigned int possible_ships = 255;

list<yarrCraft> rships;
list<spaceGroup> rgroups;
list<yarrwarsAI> rai;

unsigned int spawn_interval=0;

// control stuff

spaceDirection dir = NEUTRAL;
int thruststate = 0;
int firestate = 0;
int zoomstate = 0;
int rotstate = 0;
spacePoint pointer;
spacePoint screenpointer;
Point3D pointer3d;			

// pointers to important ships for controls
yarrCraft *foocontrol = &fooship;
spaceTurret *footurret = &fooship.turrets[0];
yarrCraft *footarget = &notarget;



//parameters for global light
GLfloat amblight[] = {0.3, 0.3, 0.3, 1.0}; //ambient
GLfloat difflight[] = {1.0, 1.0, 1.0, 1.0}; //diffuse
GLfloat speclight[] = {0.9, 0.9, 0.9, 1.0}; //specular
GLfloat poslight0[] = {0.0, 750.0, 0.0, 1.0}; //position
GLfloat spot_cutoff0 = 200;

//Be able to hold onto values for ambient, diffuse and specular lighting when we toggle those off
GLfloat ambsave[4], diffsave[4], specsave[4];



// PROTOTYPES OF FUNCTIONS

// Regular functions
void init(); // initializes the OpenGL system
void initglutcallbacks();
void spaceInit();


// Callback functions
void updatecamera(spaceShip *ship);
void Controls();
void reshape(int w, int h);
void display();

void GameLogic();
void ManageFps();

// overlay draw functions
void DrawOverlay();

void keyboard(unsigned char ch, int x, int y);
void specialkeys(int ch, int x, int y);
void keyrelease(unsigned char ch, int x, int y);
void mousemove(int mousex, int mousey);
void mouseclick(int button, int state, int mousex, int mousey);

// misc functions
void spawn_rship(spacePoint startpoint);
void spawn_ai(yarrCraft* puppet);
void spawn_team(yarrwarsAI* controller);
void cleanup(void);


// overview mode functions
// HACK : sloppy way of adding in all overview-related functions
//		it does, however, make main.cpp look cleaner
#include "spaceoverview.h"

// ******************************************************************************


// BEGIN PROGRAM


// ******************************************************************************

int main(int argc, char **argv)
{
	glutInit(&argc, argv); // initializes GLUT and processes any command line arguments
	// Note: glutInit() should be called before any other GLUT routine
	
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); // chooses single buffered and RGBA mode
	glutInitWindowPosition(10, 10); // specifies the upper-left corner of window
	glutInitWindowSize(resx, resy); // specifies the size, in pixels, of window
	glutCreateWindow(windowstr); // creates a window of name given by input string


	// remove the mouse cursor
	glutSetCursor(GLUT_CURSOR_NONE);

	initglutcallbacks();

	// Here, we call our initialization function
	init();

	spaceInit();

	// Set the rendering to update with vsync
	setVSync();

	// Now we call glutMainLoop, which basically runs the program for us
	glutMainLoop(); // shows the window and loops to accepts and processes input events

	// When program is done, get rid of bitmaps and handles to prevent memory leak
	DestroyBitmaps();

	return 0;             // ANSI C requires main to return int.
}

void initglutcallbacks()
{
		// Now we tell GLUT how our program will work, by telling it what functions to use
	glutDisplayFunc(display); // registers display callback function, which is display()
	glutReshapeFunc(reshape); // registers reshape callback function, which is reshape()
	glutIdleFunc(display); // registers display as what to do when idle
	glutPassiveMotionFunc(mousemove);
	glutMouseFunc(mouseclick);
	glutKeyboardUpFunc(keyrelease);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(specialkeys);

	
	// be sure to tell the sector which mode we're in
	starbucks_sector.overview_enabled = false;
}



void init(void) // initializes the OpenGL system
{
	glClearColor (0, 0, 0, 0);
	glShadeModel (GL_SMOOTH);


	glMatrixMode(GL_MODELVIEW); // set to display stuff
	glLoadIdentity(); // resets matrix again

	// glEnables
	glEnable(GL_DEPTH_TEST);	// z-buffering	
	glClearDepth( 1.0 );

	glEnable(GL_LIGHT0);

	//Attach light properties to light 0
	glLightfv(GL_LIGHT0, GL_POSITION, poslight0);
	glLightfv(GL_LIGHT0, GL_AMBIENT, amblight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, difflight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, speclight);
	//glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spot_direction0);
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, spot_cutoff0);

	//have material diffuse reflectance change when new glColor command is issued
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_DIFFUSE);

	// now set up the font
	font_init();
}


void spawn_rship(spacePoint startpoint)
{
		// get a random name from the name vector
		unsigned int fname_loc = rand()%pirateFirstnames.size();
		unsigned int lname_loc = rand()%pirateLastnames.size();

		string fname = pirateFirstnames[fname_loc];
		string lname = pirateLastnames[lname_loc];
		string fullname = fname + " " + lname;

		//const char* randomname = fullname.c_str();

		SPfloat randhead = rand()%359;

		float cr = (rand()%80 / 80.0) + 0.1;
		float cg = (rand()%80 / 80.0) + 0.1;
		float cb = (rand()%80 / 80.0) + 0.1;

		float sr = (rand()%50 / 50.0) + 0.3;
		float sg = (rand()%50 / 50.0) + 0.3;
		float sb = (rand()%50 / 50.0) + 0.3;


		yarrCraft rship(fullname, startpoint, randhead,
			spaceColor(cr, cg, cb), spaceColor(sr, sg, sb));

		rships.push_back(rship);
		rships.back().init();
		rships.back().setsector(&starbucks_sector);
}

void spawn_ai(yarrCraft* puppet)
{
	// also spawn an AI for the ship
	yarrwarsAI my_rai;

	rai.push_back(my_rai);
	rai.back().bind(puppet);
	rai.back().init();
}

void spawn_group(yarrCraft *leader)
{		
	// also make a new team
	// now create a team for this ship
	// this is a little bit Yarr Wars specific, but it can be modified
	// later so we can add ships to existing teams

	// FIXME : this code looks ugly and hacky, or maybe it just looks that
	// way because it's too late at night
	spaceGroup neu;
	rgroups.push_back(neu);

	//rgroups.back().init();
	rgroups.back().id = teamcounter;
	rgroups.back().color = leader->scolor;
	teamcounter++;

	// the second arg makes sure it knows it's alone
	leader->setgroup(&rgroups.back());
}


void spaceInit()
{
	srand(time(NULL));	// fire up the randomizer

	// load config files
	loadcfg("game.cfg", "gamevars", spaceGamevars);
	loadcfg("ai.cfg", "aivars", yarrAIvars);
	loadcfg("ship.cfg", "shipvars", yarrShipvars);

	loadcfg("names.cfg", "fnames", pirateFirstnames);
	loadcfg("names.cfg", "lnames", pirateLastnames);
	loadcfg("names.cfg", "planetnames", planetnames);

	// initialize sector
	starbucks_sector.init();
	sector_size = spaceGamevars["sector_size"]; // HACK : this should probably be moved into the sector class

	int start_size = spaceGamevars["ship_start_size"];

	spawn_interval = spaceGamevars["respawn_interval"];

	// figure out how much stuff we should create
	// based on the config files
	num_rships = spaceGamevars["num_ships"];
	num_rplanets = spaceGamevars["num_planets"];

	// the 'no target' pseudo-ship. See hackiness above
	notarget.life = 0;
	notarget.grog = 0;
	notarget.fame = 0;

	//rships.push_back(fooship);
	//(rships.back()).init();
	//(rships.back()).setsector(&starbucks_sector);

	/*
	fooship.init();
	fooship.setsector(&starbucks_sector);
	foocontrol = &fooship;

	spaceTeam neuteam(teamcounter);
	teamcounter++;
	neuteam.color = fooship.scolor;
	neuteam.add_ship(&fooship);
	
	starbucks_sector.teams.push_back(neuteam);
	fooship.team = &starbucks_sector.teams.back();
	*/
	
	
	SPfloat p_randx = rand()% sector_size - sector_size / 2;
	SPfloat p_randy = rand()% sector_size - sector_size / 2;

	spawn_rship(spacePoint(p_randx, p_randy));
	
	// set up controls, too
	foocontrol = &rships.back();

	// and set up the dummy AI
	fooai.bind(foocontrol);
	spawn_group(foocontrol);

	// DEBUG : for flock testing
	foocontrol->fame = spaceGamevars["cheat_fame"];
	

	// make random ships!
	int i;
	for(i = 0; i < num_rships ; i++)
	{
		SPfloat randx = rand()% start_size - start_size / 2;
		SPfloat randy = rand()% start_size - start_size / 2;
			
		spawn_rship(spacePoint(randx, randy));
		spawn_ai(&rships.back());
		spawn_group(&rships.back());
	}


	// now make planets
	for(i = 0; i < num_rplanets ; i++)
	{
		bool overlap = true;

		unsigned int pname_loc;
		string pname;
		spacePoint planetpos;
		SPfloat randsize;
		
		// add something to make sure the distance of this planet is far from others
		do
		{
			// no overlap unless proven otherwise
			overlap = false;

			// get a random name from the name vector
			pname_loc = rand()%planetnames.size();
			pname = planetnames[pname_loc];

			SPfloat randx = rand()% sector_size - sector_size / 2;
			SPfloat randy = rand()% sector_size - sector_size / 2;
			randsize = rand()% 400 + 200;

			planetpos = spacePoint(randx, randy);

			// this looks sort of ugly but it works
			unsigned int j;
			for(j=0; j < i; j++)
			{
				// also check for duplicate names
				if( planetpos.dist( randomplanets[j].pos ) < randomplanets[j].size + randsize + 200 ||
				    pname == randomplanets[j].name )
					overlap = true;
			}

		}
		while( overlap == true );


		float cr = (rand()%50 / 50.0) + 0.1;
		float cg = (rand()%50 / 50.0) + 0.1;
		float cb = (rand()%50 / 50.0) + 0.1;

		yarrPlanet rplanet(pname, planetpos, randsize,
			spaceColor(cr, cg, cb));

		randomplanets[i] = rplanet;
		randomplanets[i].setsector(&starbucks_sector);
	}
}


void updatecamera()
{

	yarrCraft *ship = foocontrol;

	
	// scale the zoom amount for certain views
	float halfzoom = zoomamt / 2;
	float quarterzoom = zoomamt / 4;

	// do control stuff first

	// camera zoom
	if(zoomstate != 0)
	{
		zoomamt += zoomspeed * zoomstate;
	
		// make sure the camera doesn't go out of bounds
		if(zoomamt < 200)
			zoomamt = 200;
		if(zoomamt > 1500)
			zoomamt = 1500;
	}

	

	// figure out where the mouse is in the gameworld
	foocontrol->target = pointer;



	glMatrixMode(GL_MODELVIEW); // Using modelview as a convenient matrix stack for our math

	// figure out which way is up (in case we roll all the way over)
	int up=1;


	target.x = ship->pos.x;
	target.y = 0;
	target.z = ship->pos.y;


	// math for chase cam
	switch(cam_orientation)
	{
		case LOCKED_CHASE:
			eye.x = ship->pos.x;
			eye.z = ship->pos.y;
			eye.x += sin(radians(ship->heading + 180)) * halfzoom;	// FIXME : not so hacky, please
			eye.y = quarterzoom * .75;
			eye.z += cos(radians(ship->heading + 180)) * halfzoom;
			break;

		case FREE_CHASE:
			// control stuff for free chase cam
			if(screenpointer.x < (resx / 8) )
				camheading += rotspeed;

			else if(screenpointer.x > (resx - resx / 8))
				camheading -= rotspeed;

			// elevation is handled in a stricter way, and rotates slower
			if(screenpointer.y < (resy / 10) )
				camelev -= elevspeed;

			else if(screenpointer.y > (resy - resy / 10))
				camelev += elevspeed;

			eye.x = ship->pos.x;
			eye.z = ship->pos.y;
			eye.y = 0;
			eye.x += sin(radians(camheading)) * halfzoom;
			eye.y += sin(radians(camelev)) * halfzoom;
			eye.z += cos(radians(camheading)) * halfzoom;
			break;

		case TURRET_CHASE:
			eye.x = ship->pos.x;
			eye.z = ship->pos.y;
			eye.x += sin(radians((int)ship->turrets[0].heading + 180)) * halfzoom;	// FIXME : not so hacky, please
			eye.y = quarterzoom;
			eye.z += cos(radians((int)ship->turrets[0].heading + 180)) * halfzoom;
			break;

		case OVERHEAD:
			eye.x = ship->pos.x - .1; // need to do this
			eye.y = zoomamt * 2;
			eye.z = ship->pos.y;
			break;
	}

	// FIXME : put in math here to figure out distance from
	// camera to target (or just use zoomamt) to adjust
	// the scaling factor for glPointSize
	// so particles show up right

	// now set the camera the right way
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Now, set a 3-D perspective view, with:
	// 45 degree FOV in y direction,
	// 1:33 aspect ratio for X,
	// starting at Z = 5,
	// ending at Z=3500
	gluPerspective(fovangle,aspratio,5,3500);

	gluLookAt(eye.x, eye.y,eye.z, target.x,target.y,target.z, 0.0, up, 0.0);


	/*Read the projection, modelview and viewport matrices
	using the glGet functions.*/
	glGetDoublev( GL_PROJECTION_MATRIX, projection );
	glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
	glGetIntegerv( GL_VIEWPORT, viewport );


	glMatrixMode(GL_MODELVIEW); // set to display stuff
	glLoadIdentity(); // resets matrix again
}


void reshape(int w, int h)
{
	// reset resolution
	resx = w;
	resy = h;

	// recalculate center
	xhalf = resx / 2;
	yhalf = resy / 2;
}

// the display callback functions
void display(void) // draws our square on the screen
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clears the current (offscreen) buffer to the background color and enables depth buffering

	// apply camera controls
	updatecamera();
	//setcamera(foocontrol);

	starbucks_sector.render_frame();
	
	starbucks_sector.drawgrid(foocontrol->pos);


	// DEBUG
	DrawOverlay();


	/**** SWAP BUFFERS ******************************************/
	glutSwapBuffers();
	//glutPostRedisplay(); // informs glutMainLoop() that the displayed content has been changed, and requests updating the display.	
	/**** / SWAP BUFFERS ****************************************/	

	GameLogic();

	// manage the FPS
	ManageFps();
}


void GameLogic()
{
	// FIXME : make a smarter version of this that doesn't require integers
	unsigned int i;
	for(i=0 ; i < gamespeed; i++)
	{

		// do control stuff
		Controls();

		starbucks_sector.handle_gamelogic();
		starbucks_sector.handle_ships();
		starbucks_sector.handle_weapons();
		starbucks_sector.handle_fx();

		
		// HACK : please please put this somewhere else
		// respawn if needed
		//if( spaceGamevars["limit_num_ships"] == 0)
		//{
		if(spawn_interval <= 0)
		{
			if( starbucks_sector.ships.size() < num_rships )
			{
				// pick a random planet
				unsigned int picked = rand()%num_rplanets;

				// add a small offset to (hopefully) avoid ships spawning on top of one another
				SPfloat xoffset = rand()%300 - 150;
				SPfloat yoffset = rand()%300 - 150;

				spawn_rship( randomplanets[picked].pos + spacePoint(xoffset, yoffset) );
				spawn_ai(&rships.back());
				spawn_group(&rships.back());
				//spawn_team(&rai.back());
			}
		}
		else
		{
			if(cycle % spawn_interval == 0)
			{
				// pick a random planet
				unsigned int picked = rand()%num_rplanets;

				// add a small offset to (hopefully) avoid ships spawning on top of one another
				SPfloat xoffset = rand()%300 - 150;
				SPfloat yoffset = rand()%300 - 150;

				spawn_rship( randomplanets[picked].pos + spacePoint(xoffset, yoffset) );
				spawn_ai(&rships.back());
				spawn_group(&rships.back());
				//spawn_team(&rai.back());
			}
		}


		// figure out who the most famous pirate is
		/*
		if( (*ships_i)->fame > mostfame)
		{
			mostfame = (*ships_i)->fame;
			mostfamous = (*ships_i);
		}
		*/

		cycle++;
	}
}

void ManageFps()
{
	frame++;


	timenow=glutGet(GLUT_ELAPSED_TIME); // correct for the sleep time
	float ms_this_frame = (timenow - timelastframe);
	
	// figure out how long we should sleep each frame
	// FIXME : this method works, for now, but doesn't actually lock
	//	the frametime to 17
	sleeptime = 17 - (ms_this_frame);
	
	if(sleeptime < 0)
		sleeptime = 0;


	ms_last_frame = ms_this_frame;
	timelastframe = timenow;

	// only update counter every certain number of frames
	if(frame == 30)
	{
		frametime = ms_this_frame;
		fps = 1000.0/ms_this_frame;
		frame = 0;
	}

	Sleep(sleeptime);
}


void DrawOverlay()
{
	//if we don't disable z-buffering, then flashing rectangle is not
	//drawn correctly--remember that we're in 2-d now
	glDisable(GL_DEPTH_TEST);

	glPushMatrix();

	//Set viewport to be entire screen, but we're only going to use left side of it
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, resx, resy);
	gluOrtho2D(0, resx, resy, 0);

	// number of lines of text to expect
	unsigned int textlines = 6;

	// foreground color
	spaceColor fg_text(foocontrol->scolor.r, foocontrol->scolor.g, foocontrol->scolor.b);
	overlaySetTextColor(fg_text);

	// everything else is 'primary'
	glColor4f(fg_text.r, fg_text.g, fg_text.b, 1);


	char data_string[80];

	// setup cursor position
	text_x = 20;
	text_y = 20;

	sprintf(data_string,"FPS: %4.2f", fps);
	writeoverlay(data_string);

	sprintf(data_string,"frame time: %i ms", frametime);
	writeoverlay(data_string);

	sprintf(data_string,"SleepTime: %4.2f", sleeptime);
	writeoverlay(data_string);

	sprintf(data_string,"gamespeed: %i", gamespeed);
	writeoverlay(data_string);

	// DEBUG

	sprintf(data_string,"turret_rotation: %4.2f", foocontrol->turrets[0].rotation);
	writeoverlay(data_string);

	// Target data box
	//					x   y   width textlines
	overlayBox("Target info", 10, resy - 120, 140, 4, foocontrol->color, foocontrol->scolor);

	// target's name and fame
	text_x = 14;
	text_y = resy - 85;
	writeoverlay((char*)footarget->name.c_str());
	sprintf(data_string,"Fame: %i  Team: %i", footarget->fame, footarget->team_id);
	writeoverlay(data_string);
	writeoverlay("H");
	writeoverlay("G");

	// draw target's status bars
	drawbar(25, resy - 55, 120, 10,
		footarget->life, footarget->maxlife, spaceColor(1, .5, .5));
	drawbar(25, resy - 42, 120, 10,
		footarget->grog, footarget->maxgrog, spaceColor(.5, 1, .5));
	

	/// Player's status bars *******************************************

	// status box
	overlayBox((char*)foocontrol->name.c_str() , xhalf - 204, resy - 64, 370, 3,  foocontrol->color, foocontrol->scolor);

	// fame
	text_x = xhalf;
	text_y = resy - 50; 
	sprintf(data_string,"Fame: %i  Team: %i", foocontrol->fame, foocontrol->team_id);
	writeoverlay(data_string);

	// status bar descriptors
	text_x = xhalf - 200;
	text_y = resy - 30;
	writeoverlay("health");
	writeoverlay("grog");

	// draw player's status bars
	drawbar(xhalf - 150, resy - 30, 300, 10,
		foocontrol->life, foocontrol->maxlife, spaceColor(1, .5, .5));
	drawbar(xhalf - 150, resy - 15, 300, 10,
		foocontrol->grog, foocontrol->maxgrog, spaceColor(.5, 1, .5));

	
	/// Player's location info *******************************************

	overlayBox("Sector info", resx - 150, resy - 120, 140, 4, foocontrol->color, foocontrol->scolor);
	
	// stuff and crap
	text_x = resx - 146;
	text_y = resy - 85;
	// HACK : notice how our gameworld is more like a y, x universe...
	//		so let's lie to the user
	sprintf(data_string,"x: %4.0f y: %4.0f", foocontrol->pos.y, foocontrol->pos.x);
	writeoverlay(data_string);
	sprintf(data_string,"Heading: %4.0fº", foocontrol->heading);
	writeoverlay(data_string);
	sprintf(data_string,"Ships present: %i", starbucks_sector.ships.size());
	writeoverlay(data_string);
	

	/// Instructions box ************************************************

	if(instructions_enabled)
	{
		overlaySetTextColor(spaceColor(1,1,0));

		overlayBox("Instructions - press F1 to toggle", xhalf - 210, yhalf + 100, 420, 6, spaceColor(1,0,0), spaceColor(1,1,0));

		sprintf(data_string,"W - Thrust     + - Zoom In     Right Mouse - Fire");
		writeoverlay(data_string);

		sprintf(data_string,"A - Turn left     - - Zoom Out     Left Mouse - Target");
		writeoverlay(data_string);

		sprintf(data_string,"D - Turn right     / - Change view     P - Pause");
		writeoverlay(data_string);

		sprintf(data_string,"O - Change to sandbox (overview) mode   ESC - Quit");
		writeoverlay(data_string);
		
		sprintf(data_string,"Kill other pirates, steal their grog and acquire fame!");
		writeoverlay(data_string);
		sprintf(data_string,"Attract other pirates to ye cause and become the most");
		writeoverlay(data_string);
		sprintf(data_string,"infamous space pirate of all time, matey!");
		writeoverlay(data_string);
		
	}

	// Paused game box ***************************************************
	if(gamespeed == 0)
	{
		overlaySetTextColor(spaceColor(0,1,0.5));
		overlayBox("GAME PAUSED - press P to unpause", xhalf - 130, yhalf - 100, 260, 1, spaceColor(0,1,0.5), spaceColor(0,1,0.5));
	}

	// 'You be dead' box ***************************************************
	if(!foocontrol->alive)
	{
		overlaySetTextColor(spaceColor(1,0,0));

		overlayBox("Arrr matey! Ye walked the space plank!", xhalf - 150, yhalf, 300, 3, spaceColor(0.5,0.5,0.5), spaceColor(1,0,0));
		writeoverlay(""); // newline
		sprintf(data_string,"Press the space barrrrrr to respawn");
		writeoverlay(data_string);
		sprintf(data_string,"Yo ho!");
		writeoverlay(data_string);
	}


	// draw a crosshair!
	// go for a 'broken diamond' look
	// HACK for a cool effect
	float scale = foocontrol->turrets[0].gun.recharge / 2;
	
	// primary color
	glColor3f(fg_text.r, fg_text.g, fg_text.b);

	glBegin(GL_LINES);
		// topleft
		glVertex2f(screenpointer.x - 10 - scale, screenpointer.y + 2 + scale);
		glVertex2f(screenpointer.x - 2  - scale, screenpointer.y + 10 + scale);
		
		// topright
		glVertex2f(screenpointer.x + 2 + scale, screenpointer.y + 10 + scale);
		glVertex2f(screenpointer.x + 10 + scale, screenpointer.y + 2 + scale);

		// bottomright
		glVertex2f(screenpointer.x + 10 + scale, screenpointer.y - 2 - scale);
		glVertex2f(screenpointer.x + 2 + scale, screenpointer.y - 10 - scale);
		
		// bottomleft
		glVertex2f(screenpointer.x - 2 - scale, screenpointer.y - 10 - scale);
		glVertex2f(screenpointer.x - 10 - scale, screenpointer.y - 2 - scale);

	glEnd();


	glPopMatrix();
	glEnable(GL_DEPTH_TEST);
	
}


void Controls(void)
{
	/// NOTE : camera controls go in the camera function so that
	/// we can control the camera while the game is paused

	foocontrol->turn(dir);

	if(thruststate != 0)
	{
		if(thruststate == 1)
			foocontrol->thrust();
		if(thruststate == -1)
			foocontrol->retro();
	}

	if(firestate != 0)
		foocontrol->fire();

	// If our target dies, set it to notarget
	if(!footarget->alive)
		footarget = &notarget;

	//worldcam.track(&fooship);
	
	// handle AI's
	list<yarrwarsAI>::iterator rai_i;
	for(rai_i = rai.begin();
		rai_i != rai.end();
		rai_i++)
	{
			rai_i->handle();

			
			// another HACK: remove dead ships from the list
			
			if(!rai_i->puppet->alive)
			{
				// FIXME : also kill actual ship
				//	before we can do this, we have to also remove
				//	the fx and weapon pointers related to this ship
				//  in spaceSector
				//  god i hate pointers


				// don't forget to remove it from its team
				// thankfully, puppet is a yarrCraft so this isn't hard
				//spaceTeam *thisteam = rai_i->puppet->team;
				//thisteam->remove_ship(rai_i->puppet);
				
				rai.erase(rai_i);
				rai_i--;
				break; // HACK
			}
	}

	// handle groups
	list<spaceGroup>::iterator g_i;
	for(g_i = rgroups.begin();
		g_i != rgroups.end();
		g_i++)
	{
			// calc avg heading for flocking routines
			//g_i->calc_avg_heading();

			if(!g_i->alive && !g_i->leader->alive)
			{
				rgroups.erase(g_i);
				break; //g_i--;
			}
	}
}


void keyboard(unsigned char ch, int x, int y)
{
	switch (ch) {
	  case '-':
	  case '_':
			zoomstate = 1;
		break;
	  case '=':
	  case '+':
			zoomstate = -1;
		break;

	  case 'a':
		dir = (spaceDirection)LEFT;
		break;
	  case 'd':
		dir = (spaceDirection)RIGHT;
		break;

	  case 'w':
		thruststate = 1;
		break;
	  case 's':
		// so long as friciton exists, no retro
		//thruststate = -1;
		break;


	  case '/':
		  if(cam_orientation == OVERHEAD)
		  {
			cam_orientation = FREE_CHASE;
		    break;
		  }
		  if(cam_orientation == FREE_CHASE)
		  {
			cam_orientation = LOCKED_CHASE;
		    break;
		  }
		  if(cam_orientation == LOCKED_CHASE)
		  {
			cam_orientation = OVERHEAD;
		    break;
		  }
		  break;

	  case ' ':
		  // respawn if we're dead
		  if(!foocontrol->alive)
		  {
			  // pick a random planet
			unsigned int picked = rand()%num_rplanets;

			spawn_rship( randomplanets[picked].pos );
			spawn_group( &rships.back() );
			
			// set up controls, too
			foocontrol = &rships.back();
			fooai.bind(foocontrol);
		 }
		  break;

	  case 'h':
		  // 'haunt targetted ship
		  
		  if(footarget != &notarget)
		  {
			  yarrCraft* temp;

			  temp = foocontrol;
			  foocontrol = footarget;
			  footarget = temp; // for shits and giggles
			  
			  // switch the AI out, too
			  fooai.bind(foocontrol);

			  // HACK : sort of dirty, but it should work
				list<yarrwarsAI>::iterator rai_i;
				for(rai_i = rai.begin();
					rai_i != rai.end();
					rai_i++)
				{
					if(rai_i->puppet == foocontrol)
					  rai_i->bind(footarget);
				}
		  }
		  
		  break;


	  case ']':			// change game speed
		  if (gamespeed < max_gamespeed)
			  gamespeed *= 2;
		  break;
	  case '[':
		  if(gamespeed > 1)
			  gamespeed /= 2;
		  break;

		case 'p':		// pause game
			if( gamespeed != 0)
			{
				old_gamespeed = gamespeed;
				gamespeed = 0;
			}
			else
			{
				gamespeed = old_gamespeed;
			}
			break;

		case 'o':
			/// switch to overview mode
			if(!overview_enabled)
			{
				// tell the game to control the guy we were controlling
				spawn_ai(foocontrol);
				
				overview_enabled = true;
				overviewcallbacks();
			}
			else
			{
				overview_enabled = false;
				/// world display mode
				// register all the normal callbacks again
				initglutcallbacks();
			}
			break;

		case 27:             /* ESC */
			cleanup();
			break;
	}
}

void specialkeys(int ch, int x, int y)
{
	switch(ch)
	{
		case GLUT_KEY_F1:
			toggle(instructions_enabled);
			break;

			
		case GLUT_KEY_F5:
			// define resolution, color depth
			glutGameModeString("800x600:16@70");
			// enter full screen
			if (glutGameModeGet(GLUT_GAME_MODE_POSSIBLE)) {

				glutEnterGameMode();

				// register callbacks again
				//init();
			}
			else
				cout << "fullscreen not possible\n";

			// return to default window
			//glutLeaveGameMode();
			break;

	}
}
 

void keyrelease(unsigned char ch, int x, int y)
{
	switch (ch) {
		case 'a':
		case 'd':
			dir = NEUTRAL;
			break;
		case 'w':
		case 's':
			thruststate = 0;
			break;
		case 'e':
			firestate = 0;
			break;
		case '-':
		case '_':
		case '=':
		case '+':
			zoomstate = 0;
			break;
		case ',':
		case '.':
		  rotstate = 0;
		  break;
	}
}

void mousemove(int mousex, int mousey)
{
	// dump mouse coords to screen pointer
	screenpointer.x = mousex;
	screenpointer.y = mousey;

	
	// these two variables should represent the center of the camera
	GLfloat offsetx = foocontrol->pos.x - xhalf * zoomamt;
	GLfloat offsety = foocontrol->pos.y - yhalf * zoomamt;


	/* 
		Unproject routine borrowed from NeHe OpenGL tutorials 
		http://nehe.gamedev.net/data/articles/article.asp?article=13
	*/

	GLdouble objx, objy, objz;
	Point3D dest, ray;
	float z = 0;

	//Read the window z value from the z-buffer	
	glReadPixels( mousex, viewport[3]-mousey, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z );	

	z = 1;
	//Use the gluUnProject to get the world co-ordinates of
	//the point the user clicked and save in objx, objy, objz.
	gluUnProject( mousex, viewport[3]-mousey, z, modelview, projection, viewport, &objx, &objy, &objz);


	pointer.x = objx;
	pointer.y = objz;

	// transfer the ray values over
	// NOTE : glUnProject only allows doubles as args,
	// thus we have to take this extra step
	dest.x = objx;
	dest.y = objy;
	dest.z = objz;

	// construct the ray vector
	ray.x = eye.x - dest.x;
	ray.y = eye.y - dest.y;
	ray.z = eye.z - dest.z;

	// P = P1 + k * (P2-P1)
	// we need to find the value of k for which
	// P's y component is 0
	// so let's solve the equation in 1-dimensional
	// space along the y axis
	// k = P - P1 / (P2-P1)
	float k;
	if( ray.y != 0)
		k = (0 - eye.y) / ray.y;
	else
		k = 1;	// if ray.y is zero, the ray is
				// directly above

	// now figure out what the x and z should be
	pointer.x = eye.x + k * ray.x; 
	pointer.y = eye.z + k * ray.z;
}


//mouse callback function
void mouseclick(int button, int state, int mousex, int mousey)
{	
	switch (button) { // do stuff based on which button is hit
		case GLUT_LEFT_BUTTON:		// left mouse button is for targetting. Blip!
			if(state == GLUT_DOWN)
			{
				// see if the pointer's current position is 'colliding' with any ships
				spaceBullet testbullet;
				testbullet.pos = pointer;

				// dirty hack function from spacesector
				// NOTE: apparently, THERE CAN BE ONLY ONE!
				spaceShip* targetted;
				targetted = starbucks_sector.check_collide(&testbullet);
			
				if(targetted != NULL)
				{
					footarget = (yarrCraft*)targetted;
				
					// DEBUG : also, (indirectly) tell my mates to track this target 
					foocontrol->enemy = targetted;
				}
				else
				{
					footarget = &notarget;
					foocontrol->enemy = NULL;
				}

			}
			break;
		case GLUT_RIGHT_BUTTON:		// right mouse button is for shooting. Pow!
			if(state == GLUT_DOWN)
				firestate = 1;
			if(state == GLUT_UP)
				firestate = 0;
			break;
		default:
			break;
	}
			
}




void cleanup(void)
{
	// let's be good boys and girls and call our destructors
	//delete &fooship_trail;
	//~fooship_trail();

	exit(0);
}


