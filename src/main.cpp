#include <iostream>
#include <dlfcn.h>
#include <cassert>
#include "AbstractInterp4Command.hh"
#include "LibInterface.hh"
#include <map>
#include <memory>
#include <array>

using namespace std;

int main()
{
	LibInterface move_interface;
	LibInterface set_interface;
	LibInterface rotate_interface;
	LibInterface pause_interface;

	if (!move_interface.Init("libInterp4Move.so"))
	{
		cerr << "Nie udalo sie wczytac wtyczki dla Move " << endl;
		return 1;
	}
	if(!set_interface.Init("libInterp4Set.so"))
	{
		cerr << "Nie udalo sie wczytac wtyczki dla Set " << endl;
		return 1;
	}
	if(!rotate_interface.Init("libInterp4Rotate.so"))
	{
		cerr << "Nie udalo sie wczytac wtyczki dla Rotate " << endl;
		return 1;
	}
	if(!pause_interface.Init("libInterp4Pause.so"))
	{
		cerr << "Nie udalo sie wczytac wtyczki dla Pause " << endl;
		return 1;
	}

	std::map<std::string, std::shared_ptr<LibInterface>> cmd_interfaces;

	cmd_interfaces.insert({"Move", std::make_shared<LibInterface>(move_interface)});
	cmd_interfaces.insert({"Set", std::make_shared<LibInterface>(set_interface)});
	cmd_interfaces.insert({"Rotate", std::make_shared<LibInterface>(rotate_interface)});
	cmd_interfaces.insert({"Pause", std::make_shared<LibInterface>(pause_interface)});
	// moze byc tez cmd_interfaces["Set"] = std::make_shared< ...

	std::array<std::string, 4> cmd_names = {"Move", "Set", "Rotate", "Pause"};

	AbstractInterp4Command *cmdInterp;

	for(const std::string& key : cmd_names)
	{
		// create interpreter
		cmdInterp = cmd_interfaces[key]->CreateCmdInterp();
		// use functionality
		cout << endl;
		cout << cmdInterp->GetCmdName() << endl;
		cout << endl;
		cmdInterp->PrintSyntax();
		cout << endl;
		cmdInterp->PrintCmd();
		cout << endl;
	}
}
