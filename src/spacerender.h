#if !defined SPACE_RENDER_INCLUDED
#define SPACE_RENDER_INCLUDED

#include "glut.h"

#include "spaceobj.h"
#include "spaceship.h"


// put a clever define here that says we're compiling for the client target
// if I knew more about stuff I would do this at the makefile level


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

	virtual void emit(spacePoint *bit);


	unsigned int max_parts; // number of particles POSSIBLE
	unsigned int num_parts; // CURRENT number of particles
	spacePoint *part; // particle coordinates
	unsigned int *part_life; // particle lifetimes

	unsigned int max_life; // maximum particle lifetime FIXME yadda yadda
	unsigned int recharge, max_recharge; // limits the rate of particle emission

	spaceColor pcolor; // primary color (FIXME make this dynamic)
	SPfloat psize;	// particle size (FIXME make this dynamic)
};

// FIXME : it might be better, in the future, to use callback functions rather than child classes
class spaceTrail : public spaceEmitter
{
public:
	spaceTrail(unsigned int nmax_life, unsigned int nmax_recharge, spaceColor ncolor);

	void bind(spaceShip *nparent, spacePoint noffset);
	void emit(spacePoint *bit);

	spaceShip *parent;
	spacePoint offset;
};

#endif