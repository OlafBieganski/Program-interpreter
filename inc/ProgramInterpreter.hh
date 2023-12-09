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

using namespace std;
using namespace xercesc;

class ProgramInterpreter
{   
    private:
        Scene _Scn;
        std::map<std::string, std::shared_ptr<LibInterface>> _LibManager;
        ComChannel _Chann2Serv;
        Configuration config;
    public:
        bool Read_XML_Config(const char* xml_filename);
        bool ExecProgram(const char* prog_filename);
};

bool Read_XML_Config(const char* xml_filename)
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
    DefaultHandler* pHandler = new XMLInterp4Config(rConfig);
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

#endif