/****************************************************/
/* space.h											*/
/*													*/
/*	SPACE - Simple Physics And Combat Engine		*/
/****************************************************/

#if !defined(__SPACE_INCLUDED)
#define __SPACE_INCLUDED

// if for some bizarre reason platform portability becomes an issue,
// put all the preprocessor directives and typedefs here


// FIXME : I'm dumb and don't know how to tell what the operating system is!
#ifndef __WINDOWS
#define __WINDOWS
#endif

// another layer of abstraction for nice portability
#ifdef __glut_h__
typedef GLfloat SPfloat; 
#else
typedef float SPfloat;
#endif

void setVSync(int interval=1);


// yay for STL goodness
#include <vector>

// now for the SPACE-specific includes
#include "spaceobj.h"
#include "spacesector.h"
#include "spaceinc.h"
#include "spacecfg.h"

#endif