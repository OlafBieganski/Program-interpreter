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
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/util/XMLString.hpp>
#include "xmlinterp.hh"

#define LINE_SIZE 500

using namespace std;
using namespace xercesc;

/*!
 * Czyta z pliku opis poleceń i dodaje je do listy komend,
 * które robot musi wykonać.
 * \param sFileName - (\b we.) nazwa pliku z opisem poleceń.
 * \param CmdList - (\b we.) zarządca listy poleceń dla robota.
 * \retval true - jeśli wczytanie zostało zrealizowane poprawnie,
 * \retval false - w przeciwnym przypadku.
 */
bool ReadFile(const char* sFileName, Configuration &rConfig)
{
   try {
            XMLPlatformUtils::Initialize();
   }
   catch (const XMLException& toCatch) {
            char* message = XMLString::transcode(toCatch.getMessage());
            cerr << "Error during initialization! :\n";
            cerr << "Exception message is: \n"
                 << message << "\n";
            XMLString::release(&message);
            return 1;
   }

   SAX2XMLReader* pParser = XMLReaderFactory::createXMLReader();

   pParser->setFeature(XMLUni::fgSAX2CoreNameSpaces, true);
   pParser->setFeature(XMLUni::fgSAX2CoreValidation, true);
   pParser->setFeature(XMLUni::fgXercesDynamic, false);
   pParser->setFeature(XMLUni::fgXercesSchema, true);
   pParser->setFeature(XMLUni::fgXercesSchemaFullChecking, true);

   pParser->setFeature(XMLUni::fgXercesValidationErrorAsFatal, true);

   DefaultHandler* pHandler = new XMLInterp4Config(rConfig);
   pParser->setContentHandler(pHandler);
   pParser->setErrorHandler(pHandler);

   try {
     
     if (!pParser->loadGrammar("config/config.xsd",
                              xercesc::Grammar::SchemaGrammarType,true)) {
       cerr << "!!! Plik grammar/actions.xsd, '" << endl
            << "!!! ktory zawiera opis gramatyki, nie moze zostac wczytany."
            << endl;
       return false;
     }
     pParser->setFeature(XMLUni::fgXercesUseCachedGrammarInParse,true);
     pParser->parse(sFileName);
   }
   catch (const XMLException& Exception) {
            char* sMessage = XMLString::transcode(Exception.getMessage());
            cerr << "Informacja o wyjatku: \n"
                 << "   " << sMessage << "\n";
            XMLString::release(&sMessage);
            return false;
   }
   catch (const SAXParseException& Exception) {
            char* sMessage = XMLString::transcode(Exception.getMessage());
            char* sSystemId = xercesc::XMLString::transcode(Exception.getSystemId());

            cerr << "Blad! " << endl
                 << "    Plik:  " << sSystemId << endl
                 << "   Linia: " << Exception.getLineNumber() << endl
                 << " Kolumna: " << Exception.getColumnNumber() << endl
                 << " Informacja: " << sMessage 
                 << endl;

            XMLString::release(&sMessage);
            XMLString::release(&sSystemId);
            return false;
   }
   catch (...) {
            cout << "Zgloszony zostal nieoczekiwany wyjatek!\n" ;
            return false;
   }

   delete pParser;
   delete pHandler;
   return true;
}

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

	// testowo
	Configuration Config;
	if(!ReadFile("config/config.xml",Config)) return 1;

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

	std::shared_ptr<LibInterface> move_interface = std::make_shared<LibInterface>();
	std::shared_ptr<LibInterface> set_interface = std::make_shared<LibInterface>();
	std::shared_ptr<LibInterface> rotate_interface = std::make_shared<LibInterface>();
	std::shared_ptr<LibInterface> pause_interface = std::make_shared<LibInterface>();

	if (!move_interface->Init("libInterp4Move.so"))
	{
		cerr << "Nie udalo sie wczytac wtyczki dla Move " << endl;
		return 1;
	}
	if(!set_interface->Init("libInterp4Set.so"))
	{
		cerr << "Nie udalo sie wczytac wtyczki dla Set " << endl;
		return 1;
	}
	if(!rotate_interface->Init("libInterp4Rotate.so"))
	{
		cerr << "Nie udalo sie wczytac wtyczki dla Rotate " << endl;
		return 1;
	}
	if(!pause_interface->Init("libInterp4Pause.so"))
	{
		cerr << "Nie udalo sie wczytac wtyczki dla Pause " << endl;
		return 1;
	}

	std::map<std::string, std::shared_ptr<LibInterface>> cmd_interfaces;

	cmd_interfaces.insert({"Move", move_interface});
	cmd_interfaces.insert({"Set", set_interface});
	cmd_interfaces.insert({"Rotate", rotate_interface});
	cmd_interfaces.insert({"Pause", pause_interface});

	std::array<std::string, 4> cmd_names = {"Move", "Set", "Rotate", "Pause"};

	AbstractInterp4Command *cmdInterp;

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
		//std::cout << key_flag << std::endl;
		if(key_flag)
		{
			cmdInterp = cmd_interfaces.at(word)->CreateCmdInterp();
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
