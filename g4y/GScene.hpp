#ifndef __GSCENE_H__
#define __GSCENE_H__
#include <memory>
#include <queue>
#include <unordered_set>

#include "GConfig.hpp"

NS_G4Y_BEGIN

class GObj;
class GWorld;
class GOpenGLView;
class GPhyWorld;
class G4Y_DLL GScene : public std::enable_shared_from_this<GScene>
{
    friend class GWorld;
public:
    GScene();
    virtual ~GScene();

    void Update();

    void AddChild(std::shared_ptr<GObj> obj);

    static std::shared_ptr<GScene> CurScene();

private:
    void DelChild(std::shared_ptr<GObj> obj);
    void ClearObjAndCom();
    void ClearObjHelper(std::shared_ptr<GObj>);
    void SetCurScene() { m_cur_scene = shared_from_this(); }

    std::unordered_set<std::shared_ptr<GObj>> m_objs;
	static std::weak_ptr<GScene>              m_cur_scene;
};

NS_G4Y_END

#endif

