#include <iostream>
#include <dlfcn.h>
#include <cassert>
#include "AbstractInterp4Command.hh"
#include "LibInterface.hh"
#include <map>
#include <memory>
#include <array>
#include <fstream>
#include <sstream>
#include <cstdio>
#include "ProgramInterpreter.hh"


int main()
{
	ProgramInterpreter cmd2server_prog;
	cmd2server_prog.Read_XML_Config("config/config.xml");
	cmd2server_prog.ExecProgram("przyklad.txt");
	return 0;
}
