#ifndef GLUT
#include "glut.h"
#define GLUT
#endif

class spaceControl
{
}

class spaceMouseKeys : public spaceControl
{
	// integrates member functions with glut controls
	void activate()
	{
		glutMouseFunc(mouse); // registers mouse callback function, which is mouse()
		glutKeyboardFunc(keyboard);
		glutKeyboardUpFunc(keyrelease);

		//glutSetKeyRepeat(GLUT_KEY_REPEAT_DEFAULT);
		glutIgnoreKeyRepeat(1);
	}

	// the mouse callback function - handles when mouse stuff is done
	void mouse(int button, int state, int x, int y)
	{
		switch (button) { // do stuff based on which button is hit
		case GLUT_LEFT_BUTTON:
			if (state == GLUT_DOWN) // if left button is down
				spinDisplay(0.2); // we need to spin our box
			if (state == GLUT_UP) // otherwise left button is up
				spinamt = 0.0; // don't do nothin'*/
			break;
		case GLUT_RIGHT_BUTTON:
			if (state == GLUT_DOWN) // if left button is down
				spinDisplay(-0.2); // we need to spin our box
			if (state == GLUT_UP) // otherwise left button is up
				spinamt = 0.0; // don't do nothin'*/
			break;
		default:
			break;
		}
	}

	void keyboard(unsigned char ch, int x, int y)
	{
		switch (ch) {
		  case GLUT_KEY_F1:
			random_color();
			break;
		  case 'a':
			spinamt = 1.0;//spinDisplay(4);
			break;
		  case 'd':
			spinamt = -1.0;//spinDisplay(-4);
			break;
		  case 27:             /* ESC */
			exit(0);
			break;
		}
	}

	void keyrelease(unsigned char ch, int x, int y)
	{
		switch (ch) {
			case 'a':
			case 'd':
				spinamt = 0.0;
			break;
		}
	}
}