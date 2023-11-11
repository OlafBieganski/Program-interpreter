#include <iostream>
#include <dlfcn.h>
#include <cassert>
#include "AbstractInterp4Command.hh"

using namespace std;

class LibInterface
{
  private:
    AbstractInterp4Command *(*pCreateCmd)(void) = nullptr;
  public:
    void *pLibHnd = nullptr;
    bool OpenLib(const char* LibName);
    bool Init();
    AbstractInterp4Command* CreateCmdInterp() {assert(pCreateCmd); return pCreateCmd();}
    ~LibInterface() { dlclose(pLibHnd); }
};

bool LibInterface::Init()
{
  void *pFun = dlsym(pLibHnd,"CreateCmd");
  if (!pFun) return false;
  pCreateCmd = reinterpret_cast<AbstractInterp4Command* (*)(void)>(pFun);
  return true;
}

bool LibInterface::OpenLib(const char* LibName)
{
  return (pLibHnd = dlopen(LibName, RTLD_LAZY)) != nullptr;
}

/*
* otworz biblioteke dynmiczna
* sprawdz czy otwarta
* funkca crate cmd znajdz
* uzuskaj ta funkcje
* stowrz interpreted dla komendy
*/

int main()
{
  LibInterface move_interface;

  if (!move_interface.OpenLib("libInterp4Set.so"))
  {
    cerr << "!!! Brak biblioteki: Interp4Move.so" << endl;
    return 1;
  }

  if (!move_interface.Init())
  {
    cerr << "!!! Nie znaleziono funkcji CreateCmd" << endl;
    return 1;
  }

  AbstractInterp4Command *pCmd = move_interface.CreateCmdInterp();

  cout << endl;
  cout << pCmd->GetCmdName() << endl;
  cout << endl;
  pCmd->PrintSyntax();
  cout << endl;
  pCmd->PrintCmd();
  cout << endl;
  
  delete pCmd;
}
