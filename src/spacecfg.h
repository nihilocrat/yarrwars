#if !defined __SPACE_CFG_INCLUDED
#define __SPACE_CFG_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <vector>
#include <map>

#include "spaceinc.h"
using namespace std;

// functions for loading config files
void loadcfg(string filename, string delimiter, vector<string> &namevec);
void loadcfg(string filename, string delimiter, map<string, SPfloat> &datamap);

#endif