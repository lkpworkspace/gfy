#ifndef __GWORLD_H__
#define __GWORLD_H__
#include <unordered_set>
#include <memory>
#include <thread>
#include <chrono>
#include <filesystem>
#include <boost/property_tree/ptree.hpp>

#include "GConfig.hpp"

NS_G4Y_BEGIN

class GScene;
class GPhyWorld;
class GOpenGLView;
class GResourceMgr;
class G4Y_DLL GWorld
{
    friend class GScene;
public:
    static GWorld* const Instance();

	void Init(int argc, char** argv);

    void SetScene(std::shared_ptr<GScene> s);

    std::shared_ptr<GPhyWorld> PhyWorld();
    std::shared_ptr<GOpenGLView> GLView();
    std::shared_ptr<GResourceMgr> ResourceMgr();

    /* second */
    static double GetTime();

    /* second */
    static double GetDeltaTime();

	const std::string GetAssetpath() { return m_assets_path.string(); }

    int Run();
    void Poll();

    ~GWorld();
private:
    GWorld();

    static GWorld*                         s_instance;
	boost::property_tree::ptree            m_json_cfg;
    std::shared_ptr<GScene>                m_scene;
    std::shared_ptr<GPhyWorld>             m_phy_world;
    std::shared_ptr<GOpenGLView>           m_gl_view;
    std::shared_ptr<GResourceMgr>          m_resource_mgr;
	std::filesystem::path                  m_assets_path;
};

NS_G4Y_END

#endif
