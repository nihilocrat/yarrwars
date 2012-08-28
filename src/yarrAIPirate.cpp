#include "yarrAI.h"
#include "yarrAIPirate.h"

#include <map>
extern map<string, SPfloat> yarrAIvars;



void yarrAIPirate :: lock_target(spaceShip *enemy)
{
	if(enemy != NULL)
	{
		mEnemy = enemy;
		puppet->enemy = mEnemy;
	}
	else
	{
		free_target();
	}
}

void yarrAIPirate :: free_target()
{
	mEnemy = mNulltarget;

	puppet->enemy = NULL;
}



void yarrAIPirate :: join_team(spaceTeam *meat, bool loner)
{
	// You can't spell team without meat!
	team = meat;

	// more null checks
	if(team == NULL)
	{
		cout << "attempting to join null team\n";
		return;
	}

	// DEBUG : dumb repitition
	meat->alive = true;


	puppet->setgroup( (spaceGroup*)team );
	puppet->setteam(meat->id, meat->color);

	meat->add_ship(this);
	
	//puppet->team_longint = (long int)team;// HACK : BLASPHEMY
	//puppet->controller_rawpointer = (rawpointer)this;

	// now I'm grouped! yay!
	/*
	if(team->leader != this)
	{
		isGrouped = true;
		isLeader = false;
	}
	else
	{
		// if teamsize is 1 or less, we're our own leader
		isGrouped = false;
		//isLeader = true;
	}
	*/
}


void yarrAIPirate :: join_team(unsigned int tid, spaceColor tcolor)
{
	puppet->setteam(tid, tcolor);

	isGrouped = true;
	mEnemy = mNulltarget;

	// find the leader of this team and set him as our leader
}

void yarrAIPirate :: join_group(spaceGroup *ngroup)
{
	// leave the old group
	leave_team();

	puppet->setgroup(ngroup);

	// it doesn't count if we're the only guy in the group
	// FIXME : leader == puppet would work too, possibly safer?
	if(ngroup->leader != puppet)
	{	
		isGrouped = true;
	}
	else
	{
		isGrouped = false;
	}
		
	mEnemy = mNulltarget;

	// see if I'm the leader
	if(ngroup->leader == puppet)
	{
		isLeader = true;
	}
}

void yarrAIPirate :: leave_team()
{
	isGrouped = false;
	isLeader = false;

	// only leave if I'm not alone
	// otherwise, Bad Things can happen
	//if(team->size() > 1)
	//	team->remove_ship(this);

	puppet->leaveteam();
}


spaceShip* yarrAIPirate :: getclosest_mate()
{
	/// Look around and find the ship closest to this one in its vision
	///
	/// Returns a ship pointer on success,
	/// returns a NULL POINTER on failure

	SPfloat closest_dist = view_dist + 1; // higher than vision to ensure we see things
	spaceShip* closest_ship = NULL;

	vector<yarrAI*>::iterator mates_i;
	for (mates_i = team->begin() ;
		mates_i != team->end() ;
		mates_i++)
	{
			// NOTE : we don't bother with sight here... let's just assume all our teammates
			// can find one another no matter what
			//if( cansee((*mates_i)->pos))
			if( (*mates_i) != this)
			{
				SPfloat this_dist = puppet->pos.dist( (*mates_i)->puppet->pos );
				
				if( this_dist < closest_dist)
					closest_ship = (*mates_i)->puppet;
			}
	}

	return closest_ship;
}


spaceShip* yarrAIPirate :: getclosest_mate_tid()
{
	/// Look around and find the ship closest to this one in its vision
	///
	/// Returns a ship pointer on success,
	/// returns a NULL POINTER on failure

	SPfloat closest_dist = view_dist + 1; // higher than vision to ensure we see things
	spaceShip* closest_ship = NULL;

	vector<spaceShip*>::iterator mates_i;
	for (mates_i = puppet->sector->ships.begin() ;
		mates_i != puppet->sector->ships.end() ;
		mates_i++)
	{
			// NOTE : we don't bother with sight here... let's just assume all our teammates
			// can find one another no matter what
			if( canfeel((*mates_i)->pos) )
			{
				//if( (*mates_i) != puppet)
					if( (*mates_i)->team_id == puppet->team_id)
					{
						SPfloat this_dist = puppet->pos.dist( (*mates_i)->pos );
						
						if( this_dist < closest_dist)
							closest_ship = (*mates_i);
					}
			}
	}

	return closest_ship;
}

void yarrAIPirate :: think()
{
	// before you even START to think, try to avoid fire
	avoid_bullets();

	// also avoid ships
	//if(!isGrouped)
		avoid_ships();

	// target people who are targetting us
	if(puppet->attacker != 0)
		lock_target(puppet->attacker);

	// attack our target, if we have one
	if(mEnemy != mNulltarget)
	{
		attack(mEnemy);
		if(!isGrouped)
			pursue(mEnemy);
	}

	// either flock or wander, based on our social situation
	if(isGrouped)
	{
		flock();
		
		// target leader assailants
		spaceShip* leadership = (spaceShip*)puppet->group->leader;
		if(leadership->attacker != 0)
			lock_target( leadership->attacker );
	}
	else
	{
		wander();	// just wander if the above is null
	}
	
	
	// every highlevel interval, do these actions
	if( time % (int)yarrAIvars["highlevel_interval"] == 0)
	{
		// this ship is in a group
		if(isGrouped)
		{
			if(isLeader) // leading the way
			{
				highlevel_lonewolf();
			}
			else		// following the herd
			{
				highlevel_flocker();
			}
		}

		// this ship is not in a group
		else
		{
			highlevel_lonewolf();
		}
	}
}


/*
high-level lone wolf thought process


if I get hit, attack my attacker, that bastard!
If tenacity is high, only change targets if I don't have one

look for ships in long vision
if ship's fame and grog > arbitrary amt, and
	my grog < certain amt, join (x% chance?)
if ship's fame < another arbitrary amt, ignore
if ship's fame in between these two amounts and teamsize < ~2, pursue and attack
else wander
*/


void yarrAIPirate :: highlevel_lonewolf()
{
	getvisible();

	// the best target we have so far based on evaluations below
	yarrCraft *best_target;

	// first of all, don't ignore ANYTHING if our grog is really low
	if( puppet->grog < yarrAIvars["minimum_safe_grog"] )
	{
		lock_target(getclosest());
		return;
	}

	if(seen.size() > 0)
	{
		vector< yarrCraft* >::iterator seen_i;
		for(seen_i = seen.begin();
			seen_i != seen.end();
			seen_i++)
		{
			// let's see if we want to join
			// NOTE : only if we're not a leader. removing this test could create interesting
			//	flocking behaviors because now flocks can join other flocks
			if(!isLeader)
			{
				// don't join someone we're attacking
				if( (*seen_i) != mEnemy)
				{
					if( (*seen_i)->fame > puppet->fame + yarrAIvars["target_fame_fuzzy"])
						if( (*seen_i)->grog > yarrAIvars["target_minimum_grog"] )
							// see if this guy is allowed to have more mates
							//if( (*seen_i)->fame - yarrAIvars["fame_per_mate"] * (*seen_i)->group->size() > 0 )
							if( rand()%100 <= yarrAIvars["join_team_chance"] )
							{
								// BROKEN : oh why god why
								//join_team( (*seen_i)->team_id, (*seen_i)->scolor );
								join_group( (*seen_i)->group );

								//join_team( (spaceTeam*) (*seen_i)->group );
								//yarrAIPirate* controller = (yarrAIPirate*)(*seen_i)->controller_rawpointer;
								//join_team( controller->team );
								return;
							}
				}
			}

			else
			{
				//if(mEnemy == mNulltarget)
					lock_target(getclosest());

			}

			// DEBUG : if we already have a target, don't look for a new one
			//if( mEnemy != mNulltarget)
			//	return;
			

			// let's see if we want to attack
			if( (*seen_i)->fame > puppet->fame - yarrAIvars["target_fame_fuzzy"] )
			//if( (*seen_i)->grog >= yarrAIvars["target_high_grog"] )
			{
				// don't bite off more than you can chew
				int sizedifference = (*seen_i)->group->size() - puppet->group->size();
				if( sizedifference <= yarrAIvars["target_largest_group"] )
				{
					// prefer closer targets
					if(mEnemy != mNulltarget)
					{
						if( puppet->pos.dist( (*seen_i)->pos ) < puppet->pos.dist( mEnemy->pos ) )
							lock_target(*seen_i);
					}
					else
						lock_target(*seen_i);
				}
			}
		} // for seen vector
	} // if seen > 0

	// well, I guess just kill the closest thing around
	if(mEnemy == mNulltarget)
		lock_target(getclosest());


	// FIXME : don't make decisions above... modify weights and make
	// the final decision HERE so that we've evaluated everything in
	// vision

}




/*
high-level grouped thought process

flock to follow leader

if leader is hit, look for target of the same team as the bullet
if leader hits something, look for targetof the same team as the victim
look at how much grog I have

in other words, make sure our target is on the same team as the Leader's
If we have no target, check the leader's target and pick one out if he
has one

if lower than safe minimum, leave group

*/

void yarrAIPirate :: highlevel_flocker()
{
	//flock();


	if(puppet->grog < yarrAIvars["minimum_safe_grog"])
	{
		leave_team();
		return;
	}

	/*
	// pick the same target as our leader
	// FIXME : right now, we're only picking his assailant
	//spaceShip *leadertarg = puppet->group->leader->attacker;
	spaceShip *leadership = (spaceShip*) puppet->group->leader;
	spaceShip *leadertarg = leadership->enemy;
	if(leadertarg != NULL)
	{
		lock_target(leadertarg);
		return; // DEBUG
	}
	*/
	
	getvisible();
	

	if(seen.size() > 0)
	{
		vector< yarrCraft* >::iterator seen_i;
		for(seen_i = seen.begin();
			seen_i != seen.end();
			seen_i++)
		{

			
			// let's see if we want to attack
			//if( (*seen_i)->fame > puppet->fame - yarrAIvars["target_fame_fuzzy"] )
			//if( (*seen_i)->grog >= yarrAIvars["target_high_grog"] )
			{
				// avoid low-grog targets
				/*
				if( (*seen_i)->grog < yarrAIvars["target_minimum_grog"])
				{
					// don't return, just look for the next target
					continue;
				}
				*/

				// don't bite off more than you can chew
				if( (*seen_i)->group->size() - puppet->group->size() <= yarrAIvars["target_largest_group"] )
				{
					lock_target(*seen_i);
					return;
				}
			}
		}
	}
}


/*
high-level group leader thought process

look for ships in long vision
if ship's fame < another arbitrary amt or
	teamsize > My teamsize + ~2, ignore
if ship's fame greater than arbitrary amount and
	teamsize < My teamsize - ~2, pursue and attack
else wander
*/

void yarrAIPirate :: highlevel_leader()
{
	//getvisible();
}




void yarrAIPirate :: flock()
{
	// variables needed

	//avg_flockmate_heading
	// NOTE : using the leader's heading instead in hopes that it will help the leader control the flock

	//closest_flockmates
	//avg_flockmate_distance
	spaceShip *nearest_neighbor = getclosest_mate_tid();
	//nearest_neighbor = (spaceShip*)puppet->group->leader;

	// if there is no nearest neighbor, make it the leader
	if(nearest_neighbor == NULL)
	{
		nearest_neighbor = (spaceShip*)puppet->group->leader;
	}
	
	SPfloat minimum_separation = feel_dist;
	SPfloat maximum_separation = standoff_range;
	SPfloat nearest_neighbor_dist = puppet->pos.dist( nearest_neighbor->pos );
	SPfloat leader_dist = puppet->pos.dist( puppet->group->leader->pos );

	// first and foremost, our target is our leader's target
	// FIXME : I don't really know how to do this

	
	if (leader_dist < minimum_separation)
	{
		//separate
		//align( wrap(nearest_neighbor->heading + 180) );
		//avoid_ships();
		//avoid(nearest_neighbor->pos);
		stop();
	}
	if (nearest_neighbor_dist > maximum_separation)
	{
		//align
		// instead of average flockmate heading, we use the leader's heading
		//align(puppet->group->leader->heading);
		//align(puppet->group->avg_heading);
		//align(avg_heading_towards_mates());

		//seek(nearest_neighbor->pos);
		seek(puppet->group->leader->pos);
		//pursue(puppet->group->leader);

		//cohere
		//align(nearest_neighbor->heading);
		//seek(nearest_neighbor->pos);
	}
	else
	{
		align(puppet->group->leader->heading);
	}
	
	thrust();


/*
turtles-own [
  flockmates         ;; agentset of nearby turtles
  nearest-neighbor   ;; closest one of our flockmates
]


to flock  ;; turtle procedure
  find-flockmates
  if any? flockmates
    [ find-nearest-neighbor 
      ifelse distance nearest-neighbor < minimum-separation
        [ separate ]
        [ align
          cohere ] ]
  fd 1
end

;;; SEPARATE

to separate  ;; turtle procedure
  turn-away (heading-of nearest-neighbor) max-separate-turn
end

;;; ALIGN

to align  ;; turtle procedure
  turn-towards average-flockmate-heading max-align-turn
end

to-report average-flockmate-heading  ;; turtle procedure
  ;; We can't just average the heading variables here.
  ;; For example, the average of 1 and 359 should be 0,
  ;; not 180.  So we have to use trigonometry.
  ;; Theoretically this could fail if both sums are 0
  ;; since atan 0 0 is undefined, but in practice that's
  ;; vanishingly unlikely.
  report atan sum values-from flockmates [sin heading]
              sum values-from flockmates [cos heading]
end

;;; COHERE

to cohere  ;; turtle procedure
  turn-towards average-heading-towards-flockmates max-cohere-turn
end

to-report average-heading-towards-flockmates  ;; turtle procedure
  ;; "towards myself" gives us the heading from the other turtle
  ;; to me, but we want the heading from me to the other turtle,
  ;; so we add 180
  report atan mean values-from flockmates [sin (towards myself + 180)]
              mean values-from flockmates [cos (towards myself + 180)]
end

;;; HELPER PROCEDURES

to turn-towards [new-heading max-turn]  ;; turtle procedure
  turn-at-most (subtract-headings new-heading heading) max-turn
end

to turn-away [new-heading max-turn]  ;; turtle procedure
  turn-at-most (subtract-headings heading new-heading) max-turn
end

;; turn right by "turn" degrees (or left if "turn" is negative),
;; but never turn more than "max-turn" degrees
to turn-at-most [turn max-turn]  ;; turtle procedure
  ifelse abs turn > max-turn
    [ ifelse turn > 0
        [ rt max-turn ]
        [ lt max-turn ] ]
    [ rt turn ]
end

*/

}

SPfloat yarrAIPirate :: avg_heading_towards_mates()
{
	SPfloat average = 0;
	unsigned int nearmates = 0;

	vector<spaceShip*>::iterator mates_i;
	for (mates_i = puppet->sector->ships.begin() ;
		mates_i != puppet->sector->ships.end() ;
		mates_i++)
	{
			// NOTE : we don't bother with sight here... let's just assume all our teammates
			// can find one another no matter what
			if( cansee((*mates_i)->pos) )
			{
				//if( (*mates_i) != puppet)
					if( (*mates_i)->team_id == puppet->team_id)
					{
						average += puppet->pos.angleto((*mates_i)->pos);
						nearmates++;
					}
			}
	}

	// divide by zero avoidance
	if(nearmates == 0)
		nearmates = 1;

	return average / nearmates;
}