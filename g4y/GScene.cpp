#include "GScene.hpp"
#include "GObj.hpp"
#include "GWorld.hpp"
#include "GPhyWorld.hpp"
#include "GOpenGLView.hpp"
#include "GCom.hpp"
#include "GCamera.hpp"

NS_G4Y_BEGIN

GScene::GScene() :
    std::enable_shared_from_this<GScene>()
{
}

GScene::~GScene()
{}

std::weak_ptr<GScene> GScene::m_cur_scene;
std::shared_ptr<GScene> GScene::CurScene()
{
    assert(!m_cur_scene.expired());
    return m_cur_scene.lock();
}

void GScene::AddChild(std::shared_ptr<GObj> obj)
{
    m_objs.insert(obj);
}

void GScene::DelChild(std::shared_ptr<GObj> obj)
{
    m_objs.erase(obj);
}

void GScene::ClearObjHelper(std::shared_ptr<GObj> obj)
{
    auto destroy_objs = obj->Children();
    for(auto begin = destroy_objs.begin(); begin != destroy_objs.end(); ++begin){
        if((*begin)->m_destroy) continue;
        ClearObjHelper((*begin));
    }
    
    auto destroy_coms = obj->GetComs();
    for(auto begin = destroy_coms.begin(); begin != destroy_coms.end(); ++begin){
        (*begin)->Obj()->DelCom((*begin));
    }
}

void GScene::ClearObjAndCom()
{
    // 1. call destroyed coms function
    // 2. destroy coms
    auto& destroy_coms = GObj::s_destroy_coms;
    for(auto begin = destroy_coms.begin(); begin != destroy_coms.end(); ++begin){
        assert(!(*begin).expired());
        (*begin).lock()->Obj()->DelCom((*begin).lock());
    }
    // 3. recursive call destroyed obj's coms OnDestroy
    // 4. recursive destroy objs
    auto& destroy_objs = GObj::s_destroy_objs;
    for(auto begin = destroy_objs.begin(); begin != destroy_objs.end(); ++begin){
        assert(!(*begin).expired());
        assert((*begin).lock()->m_destroy_flag);
        if((*begin).lock()->m_destroy) continue;
        ClearObjHelper((*begin).lock());
        if((*begin).lock()->Parent() == nullptr){
            DelChild((*begin).lock());
        }else{
            (*begin).lock()->Parent()->DelChild((*begin).lock());
        }
    }

    destroy_coms.clear();
    destroy_objs.clear();
}

void GScene::Update()
{
    auto glview = GWorld::s_instance->GLView();
    auto phyworld = GWorld::s_instance->PhyWorld();
    // awake
    for( const auto& go : m_objs ) {
        go->Start();
    }

    // update physics
    phyworld->UpdateDynamicsWorld();
    
    // process event
    glview->PollEvents();
    
    glview->BeginRender();

    // update logic
    for( const auto& go : m_objs ) {
        go->UpdateComAndChildren();
    }

    // late update logic
    for( const auto& go : m_objs ) {
        go->UpdateLate();
    }

    // rendeing
	phyworld->DebugDraw(GCamera::MainCamera()->Projection() * GCamera::MainCamera()->View());

    for(const auto& go : m_objs){
        go->UpdateRender();
    }

    glview->EndRender();

    // clear destroy objs and coms
    ClearObjAndCom();
}

NS_G4Y_END