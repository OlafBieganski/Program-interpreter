#ifndef PROGRAM_INTERPRETER_HH
#define PROGRAM_INTERPRETER_HH

#include "Scene.hh"
#include "LibInterface.hh"
#include <map>
#include <memory>
#include "ComChannel.hh"
#include "Configuration.hh"
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/util/XMLString.hpp>
#include "xmlinterp.hh"
#include <thread>

#define LINE_SIZE 500
#define PORT 6217

using namespace std;
using namespace xercesc;

class ProgramInterpreter
{   
    private:
        Scene _Scn;
        std::map<std::string, std::shared_ptr<LibInterface>> _LibManager;
        ComChannel _Chann2Serv;
        std::list<std::thread> threads_list;
        Configuration config;
        bool LoadLibraries();
        void CreateScene();
        bool ExecPreprocesor(const char *NazwaPliku, istringstream &IStrm4Cmds);
    public:
        bool Read_XML_Config(const char* xml_filename);
        bool ExecProgram(const char* prog_filename);
       ProgramInterpreter() : _Chann2Serv(PORT) {}
};

bool ProgramInterpreter::ExecPreprocesor(const char *NazwaPliku, istringstream &IStrm4Cmds)
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

bool ProgramInterpreter::Read_XML_Config(const char* xml_filename)
{
    try 
    {
        XMLPlatformUtils::Initialize();
    }
    catch (const XMLException& toCatch) 
    {
        char* message = XMLString::transcode(toCatch.getMessage());
        cerr << "Error during initialization! :\n";
        cerr << "Exception message is: \n" << message << "\n";
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

    // ladujemy nasz handler do parsera
    DefaultHandler* pHandler = new XMLInterp4Config(config);
    pParser->setContentHandler(pHandler);
    pParser->setErrorHandler(pHandler);

    try 
    {
        // tutaj ewwentulana zmiana nazwy pliku konfigutacji xsd
        if (!pParser->loadGrammar("config/config.xsd",xercesc::Grammar::SchemaGrammarType,true)) 
        {
            cerr << "!!! Plik grammar/actions.xsd, '" << endl
                << "!!! ktory zawiera opis gramatyki, nie moze zostac wczytany."
                << endl;
            return false;
        }
        pParser->setFeature(XMLUni::fgXercesUseCachedGrammarInParse,true);
        // proces parsowania pliku konfigutacji xml
        pParser->parse(xml_filename);
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

bool ProgramInterpreter::ExecProgram(const char* prog_filename)
{
    if(!LoadLibraries())
    {
        std::cerr << "Error during library loading proccess\n";
        exit(1);
    }

    CreateScene();
    _Chann2Serv.Init();
    // send all initial object positions to server
    for(const std::string& cmd: config.commands)
    {
        _Chann2Serv.Send(cmd.c_str());
    }

    std::istringstream IStrm4Cmds;
    if(!ExecPreprocesor(prog_filename, IStrm4Cmds))
    {
        std::cerr << "Error during preprocessing program file.\n";
        exit(1);
    }

    // czytanie strumienia pliku z preprocesora
    std::string word;
    bool key_flag = false;
    bool concurrency = false;
    // list of interpreters to delete after concurrent actions
    std::list<AbstractInterp4Command*> interps_to_delete;
	while(IStrm4Cmds >> word)
	{
        std::cout << "Wejscie petla: " << word << std::endl;
        // determine if the block should be run concurrently
        if(word == "End_Parallel_Actions") 
        {   
            std::cout << "->tutaj<-" << std::endl;
            //std::this_thread::sleep_for(1000ms);
            // wait for threads
            for(std::thread &th : threads_list)
            {
                std::cout << th.get_id() << std::endl;
                if(th.joinable()) th.join();
            }
            threads_list.clear();

            // delete dynamic mememory
            for(AbstractInterp4Command* interp : interps_to_delete)
            {
                delete interp;
            }
            interps_to_delete.clear();

            concurrency = false;
            IStrm4Cmds >> word;
            std::cout << "End: " << word << std::endl;
        }
        if(word == "Begin_Parallel_Actions")
        {
            concurrency = true;
            IStrm4Cmds >> word;
            std::cout << "Begin: " << word << std::endl;
        }
		// check if word is a command
        std::shared_ptr<LibInterface> LibInter;
        try
        {
            LibInter = _LibManager.at(word);
            key_flag = true;
        }
        catch(std::out_of_range)
        {
            key_flag = false;
            std::cerr << "Unrecognized command in file " << std::string(prog_filename)
                << std::endl;
        }

		if(key_flag)
		{
			AbstractInterp4Command *cmdInterp = LibInter->CreateCmdInterp();
            cmdInterp->ReadParams(IStrm4Cmds);
            if(concurrency)
            {
                std::cout << "Watek tworzony dla: " << word << std::endl;
                // Create a lambda function to be executed in the new thread
                auto threadFunction = [&](){ cmdInterp->ExecCmd(_Scn, _Chann2Serv); };
                // Create a shared pointer to the thread and pass the lambda function
                //std::shared_ptr<std::thread> thd = std::make_shared<std::thread>(threadFunction);
                // Add the thread to threads_list
                threads_list.emplace_back(std::thread(threadFunction));
                // dynamically alloc memeory to delete later
                interps_to_delete.push_back(cmdInterp);
            }
            else // non-concurrently
            {
                std::cout << "Bez watku dla: " << word << std::endl;
                cmdInterp->ExecCmd(_Scn, _Chann2Serv);
                delete cmdInterp;
            }
            key_flag = false;
		}
	}

    return true;
}

bool ProgramInterpreter::LoadLibraries()
{
    for(std::string lib_name : config.librariesNames)
    {
        std::shared_ptr<LibInterface> new_interface = std::make_shared<LibInterface>();
        bool ok = new_interface->Init(lib_name.c_str());
        if(!ok) return false;
        AbstractInterp4Command *cmdInterp = new_interface->CreateCmdInterp();
        const char* cmd_name = cmdInterp->GetCmdName();
        _LibManager.insert({std::string(cmd_name), new_interface});
        delete cmdInterp;
    }
    return true;
}

void ProgramInterpreter::CreateScene()
{
    for(MobileObj& obj : config.mobileObjs)
    {
        /*std::cout << obj.GetName() << std::endl;
        std::cout << obj.GetAng_Roll_deg() << std::endl;
        std::cout << obj.GetAng_Pitch_deg() << std::endl;
        std::cout << obj.GetAng_Yaw_deg() << std::endl;
        std::cout << obj.GetPosition_m() << std::endl;*/
        _Scn.AddMobileObj(&obj);
    }

    /*for(MobileObj& obj : config.mobileObjs)
    {
        AbstractMobileObj* cube = _Scn.FindMobileObj(obj.GetName().c_str());
        std::cout << "Name from Abstract: " << cube->GetName() << std::endl;
    }*/
}


#endif