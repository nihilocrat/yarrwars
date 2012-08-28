/********************/
/* spaceinc.h       */
/********************/

#if !defined(__SPACEINC_INCLUDED)
#define __SPACEINC_INCLUDED

// if for some bizarre reason platform portability becomes an issue,
// put all the preprocessor directives here

#include "glut.h"

// if for some bizarre reason platform portability becomes an issue,
// put all the preprocessor directives and typedefs here

// another layer of abstraction for nice portability
#ifdef __glut_h__
typedef GLfloat SPfloat; 
#else
typedef float SPfloat;
#endif


// Change this based on how the platform stores pointers
// assuming that anyone does it with anything but a long
typedef long int rawpointer;

// now for the SPACE-specific includes

// Simple functions *******************************************

// really simple boolean toggle function
void toggle(bool &foobar);

// check for null pointers in a quick and consistent way
template <class T>
bool nullcheck(T pointer);

#endif
