#include "yarrAI.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class yarrAIPirate : public yarrAI
{
public:
	yarrAIPirate()
	{
		yarrAI::yarrAI();

		// starting social situation : everyone is alone in THIS universe
		isGrouped = false;
		isLeader = false;	
	};

	void think();

	void highlevel_lonewolf();
	void highlevel_flocker();
	void highlevel_leader();

	// quack quack
	void flock();

	// *special sauce* for *smelling* *happy campers*
	spaceShip* getclosest_mate();
	spaceShip* getclosest_mate_tid();
	SPfloat avg_heading_towards_mates();

	// for acquiring targets in a pointer-safe way
	void lock_target(spaceShip *enemy);
	void free_target();

	void join_team(spaceTeam *meat, bool loner = false);
	void join_team(unsigned int tid, spaceColor tcolor);
	void join_group(spaceGroup *ngroup);
	void leave_team();


//protected:

	spaceTeam* team;

	yarrAIPirate* leader;
	vector<yarrAIPirate*> mates;

	vector<yarrAIPirate*> seen_ai;
};
