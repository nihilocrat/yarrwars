#include <stdlib.h>
#include "spaceobj.h"

#include "spacerender.h"


spaceEmitter::spaceEmitter(unsigned int nmax_parts, spaceColor ncolor)
{
	max_parts = nmax_parts;
	
	num_parts = 0;
	recharge = 0;

	max_life = 100;
	max_recharge = 5;

	// FIXME : perhaps slightly dangerous
	// calculate the maximum POSSIBLE number of particles
	// based on life and recharge rate
	max_parts = unsigned int(max_life / max_recharge) + 1;

	// set up the particle arrays
	part = new spacePoint[max_parts];
	part_life = new unsigned int[max_parts];

	pcolor = ncolor;
}

spaceEmitter::~spaceEmitter()
{
	delete part;
	delete part_life;
}

void spaceEmitter::fire()
{
	if(recharge <= 0)
	{
		// find an unused particle
		for (unsigned int i = 0 ; part_life[i] > 0 && i <= max_parts ; i++){}

		if (i==num_parts) return; // all particles are in use

		// give it LIFE!!!!
		part_life[i] = max_life;
		num_parts++;

		// start the recharge timer
		recharge = max_recharge;

		// now pass this particle to the emit function, where fun things happen!
		emit(&part[i]);
	}
}

void spaceEmitter :: emit(spacePoint *bit)
{
	bit->x = pos.x + 1.0*(rand()%11-5);
	bit->y = pos.y + 1.0*(rand()%11-5);
}

void spaceEmitter::tick(unsigned int numticks)
{
	// first tick the recharge timer
	if(recharge > 0)
		recharge--;

	unsigned int i;
	for(i=0; i < num_parts ; i++)
	{
		// kill off 'dead' particles : FIXME! this might be dangerous...
		if( part_life[i] <= 0 )
		{
			num_parts--;
			part_life[i] = 0;
		}
		else
			part_life[i] -= numticks;
	}
}

void spaceEmitter::draw()
{
	int i;	for(i=0; i < num_parts ; i++)
	{
		if(part_life[i] > 0) // only draw 'living' particles
		{
			glPointSize((part_life[i] / 100.0) * 5.0);

			float colorscale = (part_life[i] / 100.0) / 1.0;

			glBegin(GL_POINTS);
				glColor3f(pcolor.r * colorscale, pcolor.g * colorscale, pcolor.b * colorscale);
				glVertex3f(part[i].x, part[i].y, 0.0);
			glEnd();
		}
	}
}

void spaceEmitter::move(spacePoint newpos)
{
	// FIXME : same problem as the turret updating its position
	pos = newpos;
}


spaceTrail::spaceTrail(unsigned int nmax_life, unsigned int nmax_recharge, spaceColor ncolor)
{
	num_parts = 0;
	recharge = 0;

	max_life = nmax_life;
	max_recharge = nmax_recharge;
	max_parts = (unsigned int) (max_life / max_recharge) + 1;

	// set up the particle arrays
	part = new spacePoint[max_parts];
	part_life = new unsigned int[max_parts];

	pcolor = ncolor;
}

void spaceTrail :: bind(spaceShip *nparent, spacePoint noffset)
{
    parent = nparent;
	pos = parent->pos;

	offset = noffset;
}

void spaceTrail :: emit(spacePoint *bit)
{
	// instead of updating the position every cycle to fit the offset,
	// why not just do it when we create new bits?
	SPfloat myoffsetx = parent->pos.x + ( offset.x * cos(radians(parent->heading)) );
	SPfloat myoffsety = parent->pos.y + ( offset.y * sin(radians(parent->heading)) );

	bit->x = pos.x + 1.0*(rand()%11-5);
	bit->y = pos.y + 1.0*(rand()%11-5);
}


/*#!/usr/bin/python

# currently using pygame to render
import pygame
from pygame.locals import *

import math
import space

class Bit:
    '''
    Basic particle object

    Bit (x[float], y[float], color[rgb tuple])
    '''
    def __init__(_, xy=(0,0), size=2, newcolor=(255,255,255)):
        _.pos = space.Point(xy[0], xy[1])
        _.size = size
        _.color = newcolor
        _.life = 0
        
    def draw(_, screen):
        myrect = pygame.Rect(_.pos.x, _.pos.y, _.size, _.size)
        screen.realscreen.fill(_.color, myrect)

    def clear(_, screen):
        myrect = pygame.Rect(_.pos.x, _.pos.y, _.size, _.size)
        screen.realscreen.fill((0,0,0), myrect)

class BitStorm:
    ''' Basic class for a particle system '''
    def __init__(_):
        _.bits = []
        _.colors = []

class SpaceVPic:
    def __init__(_, pointlist):
        # in case you were wondering, we keep pics and objects seperate
        # so that when there are 20 of a given unit on screen, we only
        # precache the picture and other crap once, rather than 20 times.
        _.base = pointlist
        _.predraw(360)
    
    def predraw(_, num_of_frames):
        # lets only precache each even one and leave references to the one before if it's odd...
        _.rotated = []
        vectorlist = []
        
        # first make a list of vectors from the points (cartesian -> polar)
        for point in _.base:            
            pointvec = space.findvec(space.Point(0,0), point)
            vectorlist.append(pointvec)
    
        # now rotate the vectors and put their cartesian equivalents in a list
        for i in range(0, num_of_frames):
            newpointlist = []
            
            for vector in vectorlist:
                newpoint = vector.xyvel()
                newpointlist.append(newpoint)
                
                vector.angle += 1
                
            # append this pre-calculated list of points for this given frame to the 'rotated' pic
            _.rotated.append(newpointlist)
                
    def blit(_, screen, obj, zoomfactor):
        # we need to crap around with the points to make them
        # agree with the zoom factor and object size
        # FIXME : we're returning a LIST of TUPLES now... please make this data munging less random
        nlist = []
        for point in _.rotated[int(obj.thrustvec.angle)]:
            nx = (point.x + obj.pos.x) * zoomfactor
            ny = (point.y + obj.pos.y) * zoomfactor
            npoint = ( nx, ny )

            nlist.append(npoint)
        
        surfrect = pygame.draw.lines(screen.realscreen, obj.color, True, nlist)
        
        return surfrect
        
    def clear(_, screen, rect):
        # let's do it the lazy way
        screen.realscreen.fill((0,0,0), rect)


class SpacePic(pygame.sprite.Sprite):
    def __init__(_, filename, colorkey=None):
        pygame.sprite.Sprite.__init__(_) #call Sprite intializer
        
        # ripped from chimp example, woo
        try:
            _.baseimage = pygame.image.load(filename)
        except pygame.error, message:
            print 'Cannot load image: ', filename
            raise SystemExit, message
    
        _.baseimage = _.baseimage.convert()
        #if colorkey is not None:
        #    if colorkey is -1:
        #        colorkey = _.baseimage.get_at((0,0))
        #    _.baseimage.set_colorkey(colorkey, RLEACCEL)
    
        # hey, let's auto-precache!
        # lets only precache each even one and leave references to the one before if it's odd...
        _.rotated = []
        for i in range(0, 360):
            if i % 2 == 0:
                _.rotated.append(pygame.transform.rotate(_.baseimage, -i))
            else:
                _.rotated.append(_.rotated[i-1])
            
    def blit(_, screen, obj):
        #surface
        finalsurface = _.rotated[int(obj.thrustvec.angle)]
        
        #tuple
        # subtract the width and height from the object's position to correctly display it
        surfrect = finalsurface.get_rect()
        finalpos = (obj.pos.x - surfrect.width, obj.pos.y - surfrect.height)
        
        screen.blit( finalsurface, finalpos )
        
class SpaceScreen:
    '''
    A screen upon which SPACE objects are rendered
    '''
    def __init__(_, resolution, bpp, newflags):

        print "SPACE engine %s"    % space.versionstring    
        print "SPACE rendering engine initializing..."
        
        #pygame.init() # initialize SDL via Pygame
                
        flags = 0
        flags |= newflags #don't forget HWSURFACE = 1
        real_bpp = pygame.display.mode_ok(resolution , flags, bpp)
            
        if real_bpp == 0:
            print "Could not initialize SDL with current options"
                
        _.realscreen = pygame.display.set_mode(resolution, flags, real_bpp)
                
        # visible mouse cursor for now...
        pygame.mouse.set_visible(1)
            
        print "... created SDL screen"
            
        pygame.display.set_caption("SPACE Engine %s" % space.versionstring)
                
        _.layers = [] # list of layers on this screen
            
    def quit(_):
        pygame.quit()
        
    def addlayer(_, newlayer):
        # put in support for explicitly defining the draw order of layers later....
        # currently a first-in-first-drawn procedure
        _.layers.append(newlayer)
    
    def refresh(_, desired_fps):
        #_.realscreen.flip()
        #pygame.display.flip()
        pygame.display.update()

    def render(_):
        for layer in _.layers:
            layer.render()
            layer.blit(_.realscreen)
        
    def screenshot(_):
        pygame.image.save(_.realscreen, "space_grab.png")
        


class SpaceLayer(pygame.sprite.Group):
    '''
    A dict of objects ( object reference : pic reference )
    '''
    def __init__(_):
#        pygame.sprite.Group.__init__(_)
        _.objects = []
    
    def add(_, newobj, newpic):
        _.objects.append(newobj)
#        pygame.sprite.Group.add(_, newpic)
    
    def move(_):
        #localize for wacko efficiency
        #lmove = space.Mobile.move()
    
        for obj in _.objects:
            obj.move()
    
    def render(_, destscreen):
        ''' just yet another wrapper function '''
        pygame.sprite.Group.draw(_, destscreen.realscreen)
        
*/