
void ovrDisplay();
void ovrOverlay();
void ovrControls();
void ovrKeyboard(unsigned char ch, int x, int y);
void ovrKeyrelease(unsigned char ch, int x, int y);
void ovrMousemove(int mousex, int mousey);
void ovrMouseclick(int button, int state, int mousex, int mousey);

// focus of the overview camera
spacePoint ovrFocus(0,0);
SPfloat panspeed = 75.0;

// variables for panning the camera
short v_panstate = 0;
short h_panstate = 0;

float ovr_zoomamt = 1000.;
SPfloat ovr_zoomspeed = 30.0;

void overviewcallbacks()
{
	glDisable(GL_DEPTH_TEST);	// disable z-buffering for 2d graphics
	

		// Now we tell GLUT how our program will work, by telling it what functions to use
	glutDisplayFunc(ovrDisplay); // registers display callback function, which is display()
	glutIdleFunc(ovrDisplay);
	glutPassiveMotionFunc(ovrMousemove);
	glutMouseFunc(ovrMouseclick);
	glutKeyboardUpFunc(ovrKeyrelease);
	glutKeyboardFunc(ovrKeyboard);

	// callbacks common with the world view mode
	glutReshapeFunc(reshape); // registers reshape callback function, which is reshape()
	

	// now set up the rendering stuff
	glClearColor (0, 0, 0, 0);	// set clear window color black
	glShadeModel (GL_SMOOTH);		// set shading model to smooth

	glMatrixMode(GL_PROJECTION); // set orthographic projection with 0-100 coordinates in x and y
	glLoadIdentity(); // resets matrix again


	//glMatrixMode(GL_MODELVIEW);
	
	// be sure to tell the sector which mode we're in
	starbucks_sector.overview_enabled = true;
}


void ovrDisplay()
{
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clears the current (offscreen) buffer to the background color and enables depth buffering

	glClear(GL_COLOR_BUFFER_BIT); // clears the current (offscreen) buffer to the background color

	//glOrtho(view_x, ovrFocus.x + (zoomamt), ovrFocus.y + (zoomamt), view_y, -1, 1);
	

	glMatrixMode(GL_PROJECTION); // set orthographic projection with 0-100 coordinates in x and y
	//glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();


	// set up variables for the viewport
	// this makes sure it zooms into the CENTER of the screen
	// FIXME : implement zooms with trigonometry stuff
	GLfloat view_x = (xhalf) + ovrFocus.x;
	GLfloat view_y = -(yhalf) + ovrFocus.y;

	
	// figure out the projection from zoom amount
	glOrtho(-ovr_zoomamt + ovrFocus.x, ovr_zoomamt + ovrFocus.x,
		-ovr_zoomamt + ovrFocus.y, ovr_zoomamt + ovrFocus.y, 1, -1);

	// changes the zoom (ovr_zoomamt) and location (pan) of what is seen
	//glViewport(view_x, view_y, resx, resy);
	glViewport(0, 0, resx, resy);

	// apply camera controls
	if(zoomstate != 0)
		ovr_zoomamt += ovr_zoomspeed * zoomstate;

	// so moust panning
	if(screenpointer.x < (resx / 8) )
		ovrFocus.x -= panspeed;

	else if(screenpointer.x > (resx - resx / 8))
		ovrFocus.x += panspeed;

	// elevation is handled in a stricter way, and rotates slower
	if(screenpointer.y < (resy / 10) )
		ovrFocus.y += panspeed;

	else if(screenpointer.y > (resy - resy / 10))
		ovrFocus.y -= panspeed;

	

	
	glDisable(GL_LIGHTING);
	glShadeModel(GL_FLAT);

	//starbucks_sector.overview_drawgrid(ovrFocus, ovr_zoomamt);

	starbucks_sector.render_overview();
	
	
	glShadeModel(GL_SMOOTH);

	ovrOverlay();

	
	glEnable(GL_LIGHTING);

	/**** SWAP BUFFERS ******************************************/
	glutSwapBuffers();
	glutPostRedisplay(); // informs glutMainLoop() that the displayed content has been changed, and requests updating the display.	
	/**** / SWAP BUFFERS ****************************************/
	
	GameLogic();

	// manage the FPS
	ManageFps();
}


void ovrOverlay()
{
	glPushMatrix();

	//Set viewport to be entire screen, but we're only going to use left side of it
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	gluOrtho2D(0, resx, resy, 0);
	glViewport(0, 0, resx, resy);

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

	sprintf(data_string,"SleepTime: %4.2f", sleeptime);
	writeoverlay(data_string);

	sprintf(data_string,"gamespeed: %i", gamespeed);
	writeoverlay(data_string);

	// DEBUG
	sprintf(data_string,"pointer: %4.2f, %4.2f", pointer.x, pointer.y);
	writeoverlay(data_string);

	sprintf(data_string,"debug_var: %4.2f", foocontrol->turrets[0].rotation);
	writeoverlay(data_string);

	// Target data box
	//					x   y   width textlines
	overlayBox("Target info", 10, resy - 120, 140, 4, foocontrol->color, foocontrol->scolor);

	// target's name and fame
	text_x = 14;
	text_y = resy - 85;
	writeoverlay((char*)footarget->name.c_str());
	sprintf(data_string,"Fame: %i", footarget->fame);
	writeoverlay(data_string);
	writeoverlay("H");
	writeoverlay("G");

	// draw target's status bars
	drawbar(25, resy - 55, 120, 10,
		footarget->life, footarget->maxlife, spaceColor(1, .5, .5));
	drawbar(25, resy - 42, 120, 10,
		footarget->grog, footarget->maxgrog, spaceColor(.5, 1, .5));


	/// Player's status bars *******************************************


	overlayBox("Sandbox controls", xhalf - 220, resy - 80, 440, 3,
		spaceColor(0.7,0.7,0.7), spaceColor(0.7, 1.0, 0.5));
	
	// stuff and crap
	text_x = xhalf - 208;
	text_y = resy - 50;
	// HACK : notice how our gameworld is more like a y, x universe...
	//		so let's lie to the user
	writeoverlay("H - Inhabit target   [ - Slow down game   ] - Speed up game");
	

	/// The "mosts" **************************************************
	/*
	overlayBox("Most Famous Pirate", resx - 150, resy - 200, 140, 3, foocontrol->color, foocontrol->scolor);
	
	if(starbucks_sector.mostfamous != NULL)
	{
		text_x = resx - 146;
		text_y = resy - 140;
		sprintf(data_string,"%s", starbucks_sector.mostfamous->name);
		writeoverlay(data_string);
		sprintf(data_string,"Fame: %i", starbucks_sector.mostfamous->fame);
		writeoverlay(data_string);
	}*/


	/// Totals ******************************************************
	/*
	overlayBox("Totals", resx - 150, resy - 200, 140, 3, foocontrol->color, foocontrol->scolor);
	
	text_x = resx - 146;
	text_y = resy - 140;
	sprintf(data_string,"%s", starbucks_sector.mostfamous->name);
	writeoverlay(data_string);
	sprintf(data_string,"Fame: %i", starbucks_sector.mostfamous->fame);
	writeoverlay(data_string);
	*/

	/// Camera's location and sector info *******************************************

	overlayBox("Sector info", resx - 150, resy - 120, 140, 4, foocontrol->color, foocontrol->scolor);
	
	// stuff and crap
	text_x = resx - 146;
	text_y = resy - 85;
	// HACK : notice how our gameworld is more like a y, x universe...
	//		so let's lie to the user
	sprintf(data_string,"x: %4.0f y: %4.0f", ovrFocus.x, ovrFocus.y);
	writeoverlay(data_string);
	sprintf(data_string,"Ships present: %i", starbucks_sector.ships.size());
	writeoverlay(data_string);
	sprintf(data_string,"Cycle: %i", cycle);
	writeoverlay(data_string);

	
	// Paused game box ***************************************************
	if(gamespeed == 0)
	{
		overlaySetTextColor(spaceColor(0,1,0.5));
		overlayBox("GAME PAUSED - press P to unpause", xhalf - 130, yhalf - 100, 260, 1, spaceColor(0,1,0.5), spaceColor(0,1,0.5));
	}

	// draw a crosshair!
	// go for a 'broken diamond' look
	// HACK for a cool effect
	float scale = 0;
	
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
	
}


void ovrControls()
{
	// handle AI's
	list<yarrwarsAI>::iterator rai_i;
	for(rai_i = rai.begin();
		rai_i != rai.end();
		rai_i++)
	{
			rai_i->handle();
	}
}


void ovrKeyboard(unsigned char ch, int x, int y)
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



	  case 'w':
		v_panstate = 1;
		break;
	  case 's':
		v_panstate = -1;
		break;	  
	  case 'a':
		h_panstate = -1;
		break;
	  case 'd':
		h_panstate = 1;
		break;


	  case ',':
		  cam_orientation = FREE_CHASE;
		  break;
	  case '.':
		  cam_orientation = LOCKED_CHASE;
		  break;
	  case '/':
		  cam_orientation = OVERHEAD;
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
			  gamespeed *= .5;
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
				overview_enabled = true;
			}
			else
			{
				// pop fooai from the ai vector
				list<yarrwarsAI>::iterator rai_i;
				for(rai_i = rai.begin(); rai_i != rai.end(); rai_i++)
					if(rai_i->puppet == foocontrol)
					{
						rai.erase(rai_i);
						break;
					}

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

void ovrKeyrelease(unsigned char ch, int x, int y)
{
	switch (ch) {
		
		case '-':
		case '_':
		case '=':
		case '+':
			zoomstate = 0;
			break;

		case 'w':
		case 's':
			v_panstate = 0;
			break;
		case 'a':
		case 'd':
			h_panstate = 0;
			break;
		case 'r':
		case 'v':
			zoomstate = 0;
			break;
	}
}



void ovrMousemove(int mousex, int mousey)
{
	// dump mouse coords to screen pointer
	screenpointer.x = mousex;
	screenpointer.y = mousey;

	
	// these two variables should represent the center of the camera
	GLfloat offsetx = ovrFocus.x - ovr_zoomamt;
	GLfloat offsety = ovrFocus.y - ovr_zoomamt;

	
	//	-ovr_zoomamt + ovrFocus.x, ovr_zoomamt + ovrFocus.x,
	//			-ovr_zoomamt + ovrFocus.y, ovr_zoomamt + ovrFocus.y, 1, -1);


	// figure out where the center should be through a process that took
	// lots of trial-and-error
	pointer.x = offsetx + mousex;
	pointer.y = offsety - mousey;
}


//mouse callback function
void ovrMouseclick(int button, int state, int mousex, int mousey)
{	
	switch (button) { // do stuff based on which button is hit
		case GLUT_LEFT_BUTTON:		// left mouse button is for targetting. Blip!
			if(state == GLUT_DOWN)
			{
				// see if the pointer's current position is 'colliding' with any ships
				spaceBullet testbullet;
				testbullet.pos.x = pointer.y; // make sure to do wacky commie coordinates
				testbullet.pos.y = pointer.x;


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
			
			break;
		default:
			break;
	}
			
}

