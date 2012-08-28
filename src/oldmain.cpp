// -------------------------------------------------
// SpinSquare.cpp
// Creates a spinning square in a small GLUT window
// -------------------------------------------------

// HEADERS

#include <windows.h> // standard windows functions
#include <stdio.h>
#include "glut.h"	 // standard GLUT functions

#include <stdlib.h>
#include <time.h>

#include <iostream>
#include <string>

using namespace std;

#include "space.h"
#include "spacerender3d.h"
#include "spaceworld3d.h"



// GLOBAL VARIABLES


static GLfloat spin = 0.0; // spin = rotation of square in degrees
static GLfloat spinamt = 0.0;
static int thruststate = 0;

spaceDirection dir;

static GLfloat red   = GLfloat(rand() % 100 + 10) / 100;
static GLfloat green = GLfloat(rand() % 100 + 10) / 100;
static GLfloat blue  = GLfloat(rand() % 100 + 10) / 100;

static int G_resx = 800;
static int G_resy = 600;
const int xhalf = G_resx / 2;
const int yhalf = G_resy / 2;

SPfloat zoom = 1.0;

static int sleeptime = 15;


// camera for this client
spaceCamera3D worldcam;

/*
// tracks mouse pointer movement for turrets
spaceShip maus(spacePoint(0,0), 0,0,0,0, spaceColor(1,1,1), spaceColor(0,1,0));
spaceObj mousepointer;
spaceObj *pointer = &maus;
*/

//					x  y  h  turn imp  color: r g b
spaceShip fooship(spacePoint(0, 0), 0, 1.2, 0.03, 8, spaceColor(0.6, 0.4, 0.5),
													  spaceColor(0.8, 0.2, 1.0));
spaceShip barship(spacePoint(300, 300), 0, 1.2, 0.03, 8, spaceColor(0.5, 0.6, 0.7),
													  spaceColor(0.2, 1.0, 0.8));

spaceTurret footurret(1.4, 1); // turnrate, size?
spaceTrail fooship_trail(100, 5, spaceColor(0.8, 0.2, 1.0)); // max particles, recharge, color

spaceObj *focus = &fooship; // FIXME : put this and other camera data in a class

// PROTOTYPES OF FUNCTIONS

// Regular functions
void init(); // initializes the OpenGL system
void cleanup(void);

void GameLogic(void);

// Callback functions
void display(); // draws our square on the screen
void reshape(int w, int h); // handles window size changes
void keyboard(unsigned char ch, int x, int y); // keyboard events
void keyrelease(unsigned char ch, int x, int y);

//mouse callback function
void mousebuttons(int button, int state, int xmouse, int ymouse);



// -------------------------------------------------


// BEGIN PROGRAM


//  main function does the following
//  Open window with initial window size, title bar, 
//  RGBA display mode, and handle input events.
//  Most of this stuff will look obscure for now, but it should make sense later
//  Everything that starts with "glut" is a call to the glut library

int main(int argc, char **argv)
{
	glutInit(&argc, argv); // initializes GLUT and processes any command line arguments
							// Note: glutInit() should be called before any other GLUT routine
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB); // chooses double buffered and RGBA mode
	
	glutInitWindowPosition(100, 100); // specifies the upper-left corner of window
	glutInitWindowSize(G_resx, G_resy); // specifies the size, in pixels, of window
	glutCreateWindow("SPACE3d"); // creates a window of name given by input string

	/*
	// setting the game mode replaces the above
	// calls to set the window size and position.
	glutGameModeString("800x600:32");
	// enter full screen
	if (glutGameModeGet(GLUT_GAME_MODE_POSSIBLE)) 
		glutEnterGameMode();
	else {
		printf("The select mode is not available\n");
		exit(1);	
	}
	*/

	// Here, we call our initialization function
	init();
	

	// Now we call glutMainLoop, which basically runs the program for us
	glutMainLoop(); // shows the window and loops to accepts and processes input events

	//cleanup(); // call the cleanup function

	return 0;             // ANSI C requires main to return int. 
}



void init(void) // initializes the OpenGL system
{
	// Now we tell GLUT how our program will work, by telling it what functions to use
	// when different things happen
	glutDisplayFunc(display); // registers display callback function, which is display()
	//glutPassiveMotionFunc(mousestuff); // registers mouse callback function, which is mouse()
	//glutMouseFunc(mousebuttons);
	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyrelease);

	// Game logic stuff
	//glutIdleFunc(GameLogic);

	//glutSetKeyRepeat(GLUT_KEY_REPEAT_DEFAULT);
	glutIgnoreKeyRepeat(1);

	srand(time(NULL));

	glClearColor (0, 0, 0, 0);	// set clear window color to black
	glShadeModel (GL_SMOOTH);		// set shading model to smooth 
	glPointSize(4.0);
	
	
	glMatrixMode(GL_MODELVIEW); // set to display stuff
	glLoadIdentity(); // resets matrix again
	
	glEnable(GL_DEPTH_TEST);
	
	Point3D eye, target;

	eye.x = 0;
	eye.y = 80.0;
	eye.z = 0; // values for gluLookAt eye

	target.x = 0;
	target.y = 0;
	target.z = 0;

	worldcam.set(eye, target);
	worldcam.setbounds(5, 1000); // near and far clipping planes

	// put non-GLUT stuff down here
				  //parent  //offsets
	footurret.bind(&fooship, spacePoint(20, 10));
	
	//glutSpecialUpFunc(void (*func)(int key,int x, int y));
}


void cleanup(void)
{
	// let's be good boys and girls and call our destructors
	//delete &fooship_trail;
	//~fooship_trail();

	exit(0);
}



void GameLogic(void)
{
	fooship.turn(dir);
	
	if(thruststate == 1)
	{
		fooship.thrust();
		fooship_trail.fire();
	}

	glutPostRedisplay(); // informs glutMainLoop() that the displayed content has been changed, and requests updating the display.

	
	fooship.move();
	barship.move();
	footurret.move(&fooship);
	//footurret.update();
	//footurret.track(pointer);

	fooship_trail.bind(&fooship, spacePoint(1, 1));
	fooship_trail.tick();

	//worldcam.track(&fooship);

	// zoom stuff
	//glOrtho(0, G_resx, 0, G_resy, -1,1);
}

// CALLBACK FUNCTIONS
// callback functions are called when specific events occur
// our program has one thing it does over and over, which is the display function
// our program needs to handle two events the user might try
//		- when the window gets reshaped,
//		- when the mouse is clicked


// the display callback functions
void display(void)
{
	int frame=0,time,timebase=0;

	//glClear(GL_COLOR_BUFFER_BIT); // clears the current (offscreen) buffer to the background color

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clears the current (offscreen) buffer to the background color and enables depth buffering


	// debug
	glPushMatrix();

	// terrain
	glBegin(GL_QUADS);
	
		// ground
		glColor3f(0.2,0.3,0.2); // green
		
		glVertex3f(0,0,0);
		glVertex3f(500,0,0);
		glVertex3f(500,0,500);
		glVertex3f(0,0,500);
	
	glEnd();

	glutSolidCube(50);

	glPopMatrix();

	worldcam.update();

	fooship.draw();
	barship.draw();
	//footurret.draw();
	//maus.draw();

	fooship_trail.draw();

	glutSwapBuffers(); // swaps the current (offscreen) buffer with the one that's onscreen, thereby displaying our buffer

	GameLogic();

	Sleep(sleeptime); // FIXME : eventually replace this with something cross-platform

	/*frame++;
	time=glutGet(GLUT_ELAPSED_TIME);
	if (time - timebase > 1000) {
		sprintf(s,"FPS:%4.2f",
			frame*1000.0/(time-timebase));
		timebase = time;		
		frame = 0;
	}*/

}


void keyboard(unsigned char ch, int x, int y)
{
	switch (ch) {
	  case 'a':
		dir = (spaceDirection)LEFT;
		break;
	  case 'd':
		dir = (spaceDirection)RIGHT;
		break;
	  case 'w':
		thruststate = 1;
		break;
	  case 27:             /* ESC */
		//cleanup();
		break;

	case GLUT_KEY_F1:  
		
		// define resolution, color depth
		glutGameModeString("800x600:32");
		// enter full screen
		if (glutGameModeGet(GLUT_GAME_MODE_POSSIBLE)) {
		
			glutEnterGameMode();
			
			// register callbacks again
			init();
		}			
		break;	
	case GLUT_KEY_F6: 
		// return to default window
		glutLeaveGameMode();
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
			thruststate = 0;
			break;
	}
}



//mouse callback function
void mousebuttons(int button, int state, int xmouse, int ymouse)
{

	switch (button) { // do stuff based on which button is hit
		case GLUT_LEFT_BUTTON: //if left mouse button generated event
			if (state == GLUT_DOWN)
			{
				// put fire fuction here
			}
			else	// mouse button must be up
			{
			}
			break;
		case GLUT_RIGHT_BUTTON:
			if (state == GLUT_DOWN)
			{
				// put fire fuction here
			}
			else	// mouse button must be up
			{
			}
			break;
		default:
			break;
	}
			
}