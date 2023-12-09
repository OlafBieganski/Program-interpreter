#ifndef CONFIGURATION_HH
#define CONFIGURATION_HH

#include <list>
#include "MobileObj.hh"

struct Configuration
{
	std::list<std::string> librariesNames;
	std::list<std::string> commands;
	std::list<MobileObj> mobileObjs;
};


#endif
