#ifndef LIBINTERFACE_HH
#define LIBINTERFACE_HH

#include <iostream>
#include <cassert>
#include <dlfcn.h>

class LibInterface
{
  private:
    AbstractInterp4Command *(*pCreateCmd)(void) = nullptr;
  public:
    void *pLibHnd = nullptr;
    bool Init(const char* LibName);
    AbstractInterp4Command* CreateCmdInterp() {assert(pCreateCmd); return pCreateCmd();}
    ~LibInterface() { if(pLibHnd) dlclose(pLibHnd); }
};

bool LibInterface::Init(const char* LibName)
{
    pLibHnd = dlopen(LibName, RTLD_LAZY);
    if(!pLibHnd)
    {
        std::cerr << "LibInterface::Init - nie udało sie otworzyc biblioteki dynamicznej" << std::endl;
        return false;
    }

    void *pFun = dlsym(pLibHnd,"CreateCmd");
    if (!pFun)
    {
        std::cerr << "LibInterface::Init - nie udalo sie znalezc CreateCmd" << std::endl;
        return false;
    }
    pCreateCmd = reinterpret_cast<AbstractInterp4Command* (*)(void)>(pFun);
    return true;
}

#endif