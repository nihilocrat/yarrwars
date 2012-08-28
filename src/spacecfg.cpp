#include <string>
#include <vector>
#include <map>

#include <fstream>
#include <iostream>

using namespace std;

#include "spacecfg.h"

void loadcfg(string filename, string delimiter, vector<string> &namevec)
{
	ifstream file;

	file.open(filename.c_str(), ios::in);

	// check the file
	if(!file.is_open())
	{
		cout << "Cannot open " << filename << "\n";
		return;
	}

	// figure out tags
	string opentag = "<" + delimiter + ">";
	string closetag = "</" + delimiter + ">";

	while(!file.eof())
	{
		string line;

		file >> line;

		// delimits game logic variables
		// (starting grog, various rates of change)
		if(line == opentag)
		{
			// NOTE : <gamevars> gets ignored
			// make sure there is NO DATA on the line
			// with <gamevars>

			while(line != closetag && !file.eof())
			{
				file >> line;
				
				// little HACK : apparently bad data likes to hang out at the end 
				if(line != closetag)
					namevec.push_back(line);
			}
			
		}
	}

	file.close();
}


void loadcfg(string filename, string delimiter, map<string, SPfloat> &datamap)
{
	ifstream file;

	file.open(filename.c_str(), ios::in);

	// check the file
	if(!file.is_open())
	{
		cout << "Cannot open " << filename << "\n";
		return;
	}

	// figure out tags
	string opentag = "<" + delimiter + ">";
	string closetag = "</" + delimiter + ">";

	while(!file.eof())
	{
		string line;
		SPfloat data;

		file >> line;

		// delimits game logic variables
		// (starting grog, various rates of change)
		if(line == opentag)
		{
			// NOTE : <gamevars> gets ignored
			// make sure there is NO DATA on the line
			// with <gamevars>

			while(line != closetag && !file.eof())
			{
				file >> line >> data;

				datamap[line] = data;
			}
		}
	}

	file.close();
}

