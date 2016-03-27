/*
Impact Physics
by Spearminty
*/

#include <string>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <map>

#define _USE_MATH_DEFINES
#include <math.h>
#include "script.h"

#include <sstream>
#include <ctime>
#include <list>
#include "config.h"
#include <windows.h>
#include <direct.h>
using namespace std;

std::string line;
float force;
bool forceBool = false;
float physDamage;
bool physDamageBool = false;
float physRadius;
bool physRadiusBool = false;
Hash hash2;

string ExePath() {
	char buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);
	string::size_type pos = string(buffer).find_last_of("\\/");
	return string(buffer).substr(0, pos);
}

list<WeaponStats> LoadConfigFile()
{
	char actualpath[2048];
	list<WeaponStats> wList;
	_getcwd(actualpath, 2048);

	string path = actualpath;
	path = path + "\\ImpactPhysics.ini";

	char *cstr = new char[path.length() + 1];
	strcpy(cstr, path.c_str());
	char * iniFilePath = cstr;
	int lines = 0;

	/*checking whether file could be opened or not. If file does not exist or don't have read permissions, file
	stream could not be opened.*/
	std::ifstream is_file(iniFilePath);

	while (std::getline(is_file, line))
	{
		std::istringstream is_line(line);
		std::string key;
		std::string::size_type sz;     // alias of size_t
		if (std::getline(is_line, key))
		{
			string uh = key.substr(0, 1);
			if (uh == "#" || uh == " ")
			{
				lines++;
				continue;
			}
		}
		string h = line.substr(0, 1);
		if (h == "[")
		{
			std::size_t found = line.find_first_of("]");
			if (found != std::string::npos)
			{
				std::string str2 = key.substr(1, found - 1);
				char *cstr = new char[str2.length() + 1];
				strcpy(cstr, str2.c_str());
				hash2 = GAMEPLAY::GET_HASH_KEY(cstr);
				lines++;
				continue;
			}
		}
		else
		{
			std::size_t found = key.find_first_of("=");
			if (found != std::string::npos)
			{
				h = key.substr(found + 1, string::npos);
				if (forceBool == false)
				{
					force = std::stof(h, &sz);
					forceBool = true;
					lines++;
					continue;
				}
				if (physDamageBool == false)
				{
					physDamage = std::stof(h, &sz);
					physDamageBool = true;
					lines++;
					continue;
				}
				if (physRadiusBool == false)
				{
					physRadius = std::stof(h, &sz);
					physRadiusBool = true;
				}
				//if we have all properties, add it
				if (forceBool && physDamageBool && physRadiusBool)
				{
					wList.push_front(WeaponStats(hash2, force, physDamage, physRadius));

					//reset flags 
					forceBool = physDamageBool = physRadiusBool = false;
					lines++;
					continue;
				}
			}
		}
	}
	return wList;
}