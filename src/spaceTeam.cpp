#include "spaceTeam.h"
#include "yarrAI.h"

yarrAI* spaceTeam :: leader()
{
	return mates[0];	// whoever's at the beginning of the 'queue' is the leader
}

unsigned int spaceTeam :: size()
{
	return mates.size();
}


void spaceTeam :: add_ship(yarrAI* newpirate)
{
	if(newpirate == NULL)
	{
		cout << "Error : attempted to add null pointer to spaceTeam.\n";
		return;
	}

	// make the new pirate fly the team colors
	//newpirate->team = this;
	newpirate->puppet->team_id = id;
	newpirate->puppet->scolor = color;
	
	mates.push_back(newpirate);

	// now, redistribute grog
	// for now, we're doing it commie style!
	unsigned int totalgrog = 0;

	vector<yarrAI*>::iterator matey;
	for(matey = mates.begin();
		matey != mates.end();
		matey++)
	{
			totalgrog += (*matey)->puppet->grog;
	}

	unsigned int grog_per_ship = totalgrog / mates.size();

	for(matey = mates.begin();
		matey != mates.end();
		matey++)
	{
			(*matey)->puppet->grog = grog_per_ship;
	}
}

void spaceTeam :: remove_ship(yarrAI* oldpirate)
{
	// first, find the ship
	vector<yarrAI*>::iterator pirate_i;
	// FIXME : why is this broken!?!?
	
	for(pirate_i = mates.begin(); pirate_i != mates.end(); pirate_i++)
	{
		if((*pirate_i) == oldpirate)
		{
			mates.erase(pirate_i);
			return;
		}
	}

	/*
	pirate_i = find(mates.begin(), mates.end(), oldpirate);
	
	if(pirate_i != mates.end()) // sort of like != NULL in STL world
	{
		mates.erase(pirate_i);
		return;
	}
	*/

	
	// 'kill' the team if it has no more members
	if(size() <= 0)
		alive = false;
}

void spaceTeam :: handle()
{
	// clean out null ships
	// HACK : this should be done when the ship decides that it dies,
	//	but I guess that just can't happen
	vector<yarrAI*>::iterator ships_i;
	for (ships_i = mates.begin();
		 ships_i != mates.end();
		 ships_i++)
	{
		 if((*ships_i) == NULL)
		 {
			 mates.erase(ships_i);
			 ships_i--;
		 }
	}
}