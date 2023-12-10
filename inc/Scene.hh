#ifndef SCENE_HH
#define SCENE_HH

#include "AbstractScene.hh"
#include "MobileObj.hh"
#include <map>

class Scene : public AbstractScene
{
    private:
        std::map<std::string, MobileObj*> _Set_MobileObjs;
    public:
        AbstractMobileObj*  FindMobileObj(const char *sName) override;
        void AddMobileObj(AbstractMobileObj *pMobObj) override;
        Scene() = default;
        ~Scene() = default;
};

void Scene::AddMobileObj(AbstractMobileObj *pMobObj)
{
    std::string obj_name = pMobObj->GetName();
    std::cout << "nazwa: " << obj_name << std::endl;

    if(_Set_MobileObjs.find(obj_name) == _Set_MobileObjs.end())
    {
        MobileObj* mobile_obj = dynamic_cast<MobileObj*>(pMobObj);
        if(!mobile_obj)
        {
            std::cerr << "Dynamic cast in void Scene::AddMobileObj(AbstractMobileObj *pMobObj) failed\n";
            exit(1);
        }
        _Set_MobileObjs[obj_name] = mobile_obj;
        std::cout << "Name (addmobileobj): " << _Set_MobileObjs.at(obj_name)->GetName() << std::endl;
    }
    else
    {
        std::cerr << "Error: objects with this name has already been added to the selected scene." << std::endl;
    }
}

AbstractMobileObj*  Scene::FindMobileObj(const char *sName)
{
    return _Set_MobileObjs.at(std::string(sName));
}

#endif