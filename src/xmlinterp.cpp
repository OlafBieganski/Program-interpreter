#include <xercesc/util/PlatformUtils.hpp>
#include "xmlinterp.hh"
#include <cassert>
#include <sstream>
#include <cstdlib>
#include <iostream>
#include <map>
#include "Vector3D.hh"



using namespace std;


/*!
 * Konstruktor klasy. Tutaj należy zainicjalizować wszystkie
 * dodatkowe pola.
 */
XMLInterp4Config::XMLInterp4Config(Configuration &rConfig)
{
	_config_ptr = &rConfig;
}


/*!
 * Metoda wywoływana jest bezpośrednio przed rozpoczęciem
 * przetwarzana dokumentu XML.
 */
void XMLInterp4Config::startDocument()
{
  cout << "*** Rozpoczecie przetwarzania dokumentu XML." << endl;
}


/*!
 * Metoda wywoływana jest bezpośrednio po zakończeniu
 * przetwarzana dokumentu XML.
 */
void XMLInterp4Config::endDocument()
{
  cout << "=== Koniec przetwarzania dokumentu XML." << endl;
}





/*!
 * Analizuje atrybuty elementu XML \p "Lib" i odpowiednio je interpretuje.
 * \param[in] rAttrs - atrybuty elementu XML \p "Lib".
 */
void XMLInterp4Config::ProcessLibAttrs(const xercesc::Attributes  &rAttrs)
{
	if (rAttrs.getLength() != 1) {
		cerr << "Zla ilosc atrybutow dla \"Lib\"" << endl;
		exit(1);
	}

	char* sParamName = xercesc::XMLString::transcode(rAttrs.getQName(0));

	if (strcmp(sParamName,"Name")) {
		cerr << "Zla nazwa atrybutu dla Lib" << endl;
		exit(1);
	}         

	XMLSize_t  Size = 0;
	char* sLibName = xercesc::XMLString::transcode(rAttrs.getValue(Size));

	cout << "  Nazwa biblioteki: " << sLibName << endl;

	// dodawanie biblioteki dynamicznej do listy w configuracji
	_config_ptr->librariesNames.push_back(std::string(sLibName));
	//cout << "Zapisane: " << _config_ptr->librariesNames.back() << endl;


	xercesc::XMLString::release(&sParamName);
	xercesc::XMLString::release(&sLibName);
}


/*!
 * Analizuje atrybuty. Sprawdza czy ich nazwy są poprawne. Jeśli tak,
 * to pobiera wartości atrybutów (w postaci napisów) i przekazuje ...
 * \param[in] rAttrs - atrybuty elementu XML \p "Cube".
 */
void XMLInterp4Config::ProcessCubeAttrs(const xercesc::Attributes  &rAttrs)
{
	if (rAttrs.getLength() < 1) {
		cerr << "Zla ilosc atrybutow dla \"Cube\"" << endl;
		exit(1);
	}

	/*
	*  Tutaj pobierane sa nazwy pierwszego i drugiego atrybuty.
	*  Sprawdzamy, czy na pewno jest to Name i Value.
	*/

	char* sName_Name = xercesc::XMLString::transcode(rAttrs.getQName(0));

	if(std::string(sName_Name) != "Name")
	{
		cerr << "Brak atrybutu Name dla \"Cube\"." << endl;
		exit(1);
	}

	/*std::array<std::string, 6> attr_names = {"Name", "Shift", "Scale", "RotXYZ_deg", "Trans_m", "RGB"};

	for (XMLSize_t i = 0; i < rAttrs.getLength(); i++)
    {
        char *tmp = xercesc::XMLString::transcode(rAttrs.getQName(i));
        
    }*/

	std::map<std::string, XMLSize_t> attributes;

    for (XMLSize_t i = 0; i < rAttrs.getLength(); i++)
    {
        char *tmp = xercesc::XMLString::transcode(rAttrs.getQName(i));
        attributes.emplace(std::string(tmp), i); // bylo move
        xercesc::XMLString::release(&tmp);
    }

    char *Name_value = xercesc::XMLString::transcode(rAttrs.getValue(attributes.at("Name")));
    char *Shift_value = xercesc::XMLString::transcode(rAttrs.getValue(attributes.at("Shift")));
    char *Scale_value = xercesc::XMLString::transcode(rAttrs.getValue(attributes.at("Scale")));
    char *RotXYZ_deg_value = xercesc::XMLString::transcode(rAttrs.getValue(attributes.at("RotXYZ_deg")));
    char *Trans_m_value = xercesc::XMLString::transcode(rAttrs.getValue(attributes.at("Trans_m")));
    char *RGB_value = xercesc::XMLString::transcode(rAttrs.getValue(attributes.at("RGB")));

    istringstream tmp1, tmp2, tmp3, tmp4, tmp5;
    std::string Name = std::string(Name_value);
    Vector3D Shift, Scale, RotXYZ_deg, Trans_m, RGB;
    tmp1.str(Shift_value);
    tmp1 >> Shift;
    tmp2.str(Scale_value);
    tmp2 >> Scale;
    tmp3.str(RotXYZ_deg_value);
    tmp3 >> RotXYZ_deg;
    tmp4.str(Trans_m_value);
    tmp4 >> Trans_m;
    tmp5.str(RGB_value);
    tmp5 >> RGB;

	// tworzymy obiekt cuboid i dodajemy do konfiguracji sceny
    MobileObj cuboid;
	cuboid.SetName(Name.c_str());
	cuboid.SetAng_Roll_deg(RotXYZ_deg[0]);
	cuboid.SetAng_Pitch_deg(RotXYZ_deg[1]);
	cuboid.SetAng_Yaw_deg(RotXYZ_deg[2]);
	cuboid.SetPosition_m(Trans_m);
	_config_ptr->mobileObjs.push_back(cuboid);

    std::cout << "Attributes read:\n";
    std::cout << "Name: "
              << "Name"
              << " | value: " << Name << std::endl;
    std::cout << "Name: "
              << "Shift"
              << " | value: " << Shift << std::endl;
    std::cout << "Name: "
              << "Scale"
              << " | value: " << Scale << std::endl;
    std::cout << "Name: "
              << "RotXYZ_deg"
              << " | value: " << RotXYZ_deg << std::endl;
    std::cout << "Name: "
              << "Trans_m"
              << " | value: " << Trans_m << std::endl;
    std::cout << "Name: "
              << "RGB"
              << " | value: " << RGB << std::endl;

	// tworzymy skladnie komendy ktora bedzie wyslana do serwera graficznego
	// w celu narysownia wstepnej konfiguracji obiketu
    std::stringstream command;
    command << "AddObj Name="
            << Name
            << " RGB="
            << RGB
            << " Scale="
            << Scale
            << " Shift="
            << Shift
            << " RotXYZ_deg="
            << RotXYZ_deg
            << " Trans_m="
            << Trans_m
            << "\n";
    _config_ptr->commands.push_back(command.str());

	xercesc::XMLString::release(&sName_Name);
	xercesc::XMLString::release(&Name_value);
	xercesc::XMLString::release(&Shift_value);
	xercesc::XMLString::release(&Scale_value);
	xercesc::XMLString::release(&RotXYZ_deg_value);
	xercesc::XMLString::release(&Trans_m_value);
	xercesc::XMLString::release(&RGB_value);
}







/*!
 * Wykonuje operacje związane z wystąpieniem danego elementu XML.
 * W przypadku elementu \p "Action" będzie to utworzenie obiektu
 * reprezentującego odpowiednią działanie robota.
 * W przypadku elementu \p "Parameter" będzie to zapisanie parametrów
 * związanych z danym działaniem.
 * \param[in] rElemName - nazwa elementu XML.
 * \param[in] rAttrs - atrybuty napotkanego elementu XML.
 */
void XMLInterp4Config::WhenStartElement( const std::string           &rElemName,
		                         const xercesc::Attributes   &rAttrs
                                       )
{
  if (rElemName == "Lib") {
    ProcessLibAttrs(rAttrs);   return;   
  }

  if (rElemName == "Cube") {
    ProcessCubeAttrs(rAttrs);  return;
  }
}




/*!
 * Metoda jest wywoływana po napotkaniu nowego elementu XML, np.
 * gdy zostanie napotkany element:
   \verbatim
     <Action Name="Rotate">
   \endverbatim
 *  to poprzez parametr \e xscElemName jest dostęp do nazwy elementu
 *  tzn. \p Action, 
 *  \param[in] pURI - (Uniform Resource Identifier) jeżeli nie jest on wyspecyfikowany
 *                    poprzez użycie atrybutów \p xmlns (o ile jego użycie jest przewidziane w gramatyce,
 *                    w tym przykładzie nie jest to przewidziane), to będzie to napis pusty.
 *  \param[in] pLocalName -  umożliwia dostęp do nazwy elementu XML.
 *                 W podanym przykładzie nazwą elementu XML jest "Action".
 *  \param[in] pQName - pełna kwalifikowana nazwa. To ma znaczenie, gdy użyta jest przestrzeń nazwa.
 *                      Wówczas poprzez ten parametr można otrzymać nazwę elementu wraz z prefiksem
 *                      przestrzeni nazwa. Jeśli przestrzeń nazw nie jest użyta (taka jak w tym
 *                      przykładzie), to \p pQName i \p pLocalName dostaczają identyczne napisy.
 *  \param[in] rAttrs -  lista atrybutów danego symbolu XML.
 *                 W przykładzie pokazanym powyżej listę atrybutów
 *                 będą stanowiły napisy:
 */
/*
 * Te metode nalezy odpowiednio zdekomponowac!!!
 */
void XMLInterp4Config::startElement(  const   XMLCh* const            pURI,
                                      const   XMLCh* const            pLocalName,
                                      const   XMLCh* const            pQName,
				      const   xercesc::Attributes&    rAttrs
                                    )
{
  char* sElemName = xercesc::XMLString::transcode(pLocalName);
  cout << "+++ Poczatek elementu: "<< sElemName << endl;

  WhenStartElement(sElemName, rAttrs);

  xercesc::XMLString::release(&sElemName);
}




/*!
 * Metoda zostaje wywołana w momencie zakończenia danego elementu
 * XML, np. jeżeli w pliku jest wpis:
   \verbatim
     <Lib Name="Rotate">
     </Lib>
   \endverbatim
 * to metoda ta zostanie wywołana po napotkaniu znacznika:
 * \verbatim</Lib>\endverbatim
 * Jeżeli element XML ma formę skróconą, tzn.
   \verbatim
     <Parametr Name="Rotate"/>
   \endverbatim
 * to wówczas wywołana metoda wywołana zostanie w momencie
 * napotkania sekwencji "/>"
 *  \param[in] pURI - (Uniform Resource Identifier) jeżeli nie jest on wyspecyfikowany
 *                    poprzez użycie atrybutów \p xmlns (o ile jego użycie jest przewidziane w gramatyce,
 *                    w tym przykładzie nie jest to przewidziane), to będzie to napis pusty.
 *  \param[in] pLocalName -  umożliwia dostęp do nazwy elementu XML.
 *                 W podanym przykładzie nazwą elementu XML jest "Lib".
 *  \param[in] pQName - pełna kwalifikowana nazwa. To ma znaczenie, gdy użyta jest przestrzeń nazwa.
 *                      Wówczas poprzez ten parametr można otrzymać nazwę elementu wraz z prefiksem
 *                      przestrzeni nazwa. Jeśli przestrzeń nazw nie jest użyta (taka jak w tym
 *                      przykładzie), to \p pQName i \p pLocalName dostaczają identyczne napisy.
 *                      
 */
void XMLInterp4Config::endElement(  const   XMLCh* const    pURI,
                                    const   XMLCh* const    pLocalName,
                                    const   XMLCh* const    pQName
                                 )
{
   char* sURI =  xercesc::XMLString::transcode(pURI);
   char* sElemName = xercesc::XMLString::transcode(pLocalName);
   char* sQName =  xercesc::XMLString::transcode(pQName);
   cout << "   URI: " << sURI << endl;
   cout << " QName: " << sQName << endl;
   cout << "----- Koniec elementu: "<< sElemName << endl;

   xercesc::XMLString::release(&sElemName);
}




/*!
 * Metoda wywoływana jest, gdy napotkany zostanie błąd fatalny,
 * np.
  \verbatim
    <Action Name="Rotate">
    </Parametr>
  \endverbatim
 * \param[in] rException - zawiera informacje dotyczące błędu w dokumencie,
 *                         linii, kolumny itp.
 */
void XMLInterp4Config::fatalError(const xercesc::SAXParseException&  rException)
{
   char* sMessage = xercesc::XMLString::transcode(rException.getMessage());
   char* sSystemId = xercesc::XMLString::transcode(rException.getSystemId());

   cerr << "Blad fatalny! " << endl
        << "    Plik:  " << sSystemId << endl
        << "   Linia: " << rException.getLineNumber() << endl
        << " Kolumna: " << rException.getColumnNumber() << endl
        << " Informacja: " << sMessage 
        << endl;

   xercesc::XMLString::release(&sMessage);
   xercesc::XMLString::release(&sSystemId);
}

/*!
 * Metoda jest wywoływana, gdy napotkany zostanie błąd, który nie
 * jest traktowany jako fatalny. W ten sposób traktowane są błędy
 * występujące w opisie gramatyki dokumentu.
 * \param[in] rException - zawiera informacje dotyczące błędu. Informacje
 *                     te to opis błędu oraz numer linii, w której
 *                     wystąpił błąd.
 */
void XMLInterp4Config::error(const xercesc::SAXParseException&  rException)
{
  cerr << "Blad ..." << endl;

  /*
   * Tutaj należy wstawić odpowiedni kod. Tekst wyświetlany powyżej
   * jest tylko "atrapą".
   */
}


/*!
 * Metoda wywołana jest w przypadku błędów, które nie są fatalne
 * i mogą być traktowane jako ostrzeżenia.
 * \param[in] rException - zawiera informacje dotyczące błędu w dokumencie,
 *                         linii, kolumny itp.
 */
void XMLInterp4Config::warning(const xercesc::SAXParseException&  rException)  
{
  cerr << "Ostrzezenie ..." << endl;

  /*
   * Tutaj należy wstawić odpowiedni kod. Tekst wyświetlany powyżej
   * jest tylko "atrapą".
   */
}
