#ifndef __SPACE_TEAM_INCLUDED
#define __SPACE_TEAM_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "spaceobj.h"
#include "yarrAI.h"

class spaceTeam
{
public:
	spaceTeam(unsigned int nid=0){ id = nid; alive = true; };
	
	void add_ship(yarrAI* newpirate);
	void remove_ship(yarrAI* oldpirate);

	void handle();
	
	yarrAI* leader();
	unsigned int size();

	bool alive;
	unsigned int id;
	spaceColor color;

	vector<yarrAI*> mates;	// this might seem dumb, but it's safer than overriding vector<>
};

#endif