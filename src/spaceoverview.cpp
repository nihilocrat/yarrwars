
void overviewDisplay();
void DrawOverviewOverlay();
void overviewKeyboard();

void overviewDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clears the current (offscreen) buffer to the background color and enables depth buffering

	// apply camera controls
	//updatecamera();
	//setcamera(foocontrol);

	//planet_starbucks.draw();
	starbucks_sector.handle_planets();

	starbucks_sector.handle_ships();

	//fooship_trail.draw();

	starbucks_sector.handle_weapons();
	starbucks_sector.handle_fx();

	starbucks_sector.drawgrid();

	DrawOverviewOverlay();

	/**** SWAP BUFFERS ******************************************/
	glutSwapBuffers();
	//glutPostRedisplay(); // informs glutMainLoop() that the displayed content has been changed, and requests updating the display.	
	/**** / SWAP BUFFERS ****************************************/
	

	// do game logicy stuff
	GameLogic();
	

	// manage the FPS
	
	frame++;

	// only update counter every certain number of frames
	if(frame == 30)
	{
		timenow=glutGet(GLUT_ELAPSED_TIME);// - sleeptime; // correct for the sleep time
		//if (timenow - timelastframe > 1000) {
		fps = frame * 1000.0/(timenow-timelastframe);

		timelastframe = timenow;

		frame = 0;
	}
	
	// FIXME : this isn't working right
	// make it so that every cycle takes about 15 milliseconds
	
	if(fps < 60.)
	{
		// for now, just presume that this ratio is right
		// 60 fps / 15 ms sleep
		sleeptime = fps * (15.0 / 60.0);

		// FIXME : make this smarter later
		//sleeptime = 0.;
	}

	else
		sleeptime = 15.;
	

	//sleeptime = 0.;

	Sleep(sleeptime);
}


void DrawOverviewOverlay()
{
}