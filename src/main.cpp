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


int main(int argc, char* argv[])
{
	if(argc < 3)
	{
		std::cerr << "Not enough arguments\n";
		exit(1);
	}
	
	ProgramInterpreter cmd2server_prog;
	cmd2server_prog.Read_XML_Config(argv[2]);
	cmd2server_prog.ExecProgram(argv[1]);
	return 0;
}
