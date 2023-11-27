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

#define LINE_SIZE 500

using namespace std;

bool ExecPreprocesor(const char *NazwaPliku, istringstream &IStrm4Cmds)
{
  string Cmd4Preproc = "cpp -P ";
  char Line[LINE_SIZE];
  ostringstream OTmpStrm;

  Cmd4Preproc += NazwaPliku;
  FILE* pProc = popen(Cmd4Preproc.c_str(), "r");

  if(!pProc) return false;

  while(fgets(Line, LINE_SIZE, pProc))
  {
    OTmpStrm << Line;
  }

  IStrm4Cmds.str(OTmpStrm.str());
  return pclose(pProc) == 0;
}

int main()
{
	std::istringstream IStrm4Cmds;
	const char *file_name = "przyklad.txt";

	if (ExecPreprocesor(file_name, IStrm4Cmds))
	{
    // Zapisz wynik do pliku wyjściowego
    std::ofstream output("cpp_result_file.txt");
    output << IStrm4Cmds.rdbuf();
    output.close();
    std::cout << "Preprocesowanie zakończone sukcesem." << std::endl;
	}
	else
	{
	std::cerr << "Błąd podczas preprocesowania." << std::endl;
	}

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

	/*for(const std::string& key : cmd_names)
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
	}*/

	// czytanie pliku 
	std::ifstream program_file("cpp_result_file.txt");

	if (!program_file.is_open())
	{
        cerr << "Nie udało sie otworzyc pliku z przetworzonym programem" << endl;
        return 1;
    }

	std::string word;
	bool key_flag = false;
	while(program_file >> word)
	{
		std::cout << word << std::endl;
		// check if word is a command
		for(const string& cmd : cmd_names)
		{
			if(word == cmd)
			{ 
				key_flag = true;
				break;
			}
		}
		std::cout << key_flag << std::endl;
		if(key_flag)
		{
			cmdInterp = cmd_interfaces[word]->CreateCmdInterp();
			cmdInterp->ReadParams(program_file);
			cmdInterp->PrintCmd();
			key_flag = false;
			delete cmdInterp;
		}
	}
	program_file.close();
	std::cout << "Koniec" << std::endl;
	return 0;
}
