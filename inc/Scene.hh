#ifndef SCENE_HH
#define SCENE_HH

#include "AbstractScene.hh"
#include "MobileObj.hh"
#include <map>

class Scene : public AbstractScene
{
    private:
        std::map<std::string, std::shared_ptr<MobileObj>> _Set_MobileObjs;
    public:
        std::shared_ptr<AbstractMobileObj>  FindMobileObj(const char *sName) override;
        void AddMobileObj(AbstractMobileObj *pMobObj) override;
        Scene() = default;
        ~Scene() = default;
};

void Scene::AddMobileObj(AbstractMobileObj *pMobObj)
{
    std::string obj_name = pMobObj->GetName();

    if(_Set_MobileObjs.find(obj_name) == _Set_MobileObjs.end())
    {
        _Set_MobileObjs[obj_name] = std::make_shared<MobileObj>(*pMobObj);
    }
    else
    {
        std::cerr << "Error: objects with this name has already been added to the selected scene." << std::endl;
    }
}

std::shared_ptr<AbstractMobileObj>  Scene::FindMobileObj(const char *sName)
{
    return _Set_MobileObjs.at(std::string(sName));
}

#endif