#include "spaceinc.h"

void toggle(bool &foobar)
{
	/// really simple boolean toggle function
	if(foobar == true)
		foobar = false;
	else
		foobar = true;
}


template <class T>
bool nullcheck(T pointer)
{
	if( pointer == NULL)
	{
		cout << "null pointer";
		return true;
	}
	else
		return false;
}