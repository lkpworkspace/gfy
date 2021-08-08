#include <iostream>
#include <boost/program_options.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/system/error_code.hpp>
#include <boost/filesystem.hpp>

#include "GWorld.hpp"
#include "GScene.hpp"
#include "GPhyWorld.hpp"
#include "GOpenGLView.hpp"
#include "GResourceMgr.hpp"

NS_G4Y_BEGIN

GWorld* GWorld::s_instance = nullptr;

int g_sleep_ms = 10;
auto g_begin_time = std::chrono::system_clock::now();
double g_frame_begin = 0.0f;
double g_delta_time = 0.0f;

GWorld* const GWorld::Instance()
{
    if(s_instance == nullptr)
        s_instance = new GWorld();
    return s_instance;
}

GWorld::GWorld()
{}

GWorld::~GWorld()
{
    m_gl_view->Exit();
}

void GWorld::Init(int argc, char** argv)
{
	namespace pt = boost::property_tree;
	try
	{
		std::error_code error;
		if (argc == 2 && std::filesystem::is_regular_file(argv[1], error)) {
			pt::read_json(argv[1], m_json_cfg);
		}
		else {
			pt::read_json("config.json", m_json_cfg);
		}
	}
	catch (const std::exception&)
	{
		m_json_cfg.clear();
	}
	
	pt::ptree item;

	try
	{
		m_assets_path = m_json_cfg.get<std::string>("assets_path", "../assets/");
		std::cout << "Assets floder: " << m_assets_path << std::endl;
		item = m_json_cfg.get_child("GOpenGLView");
	}
	catch (std::exception &e)
	{
		item.clear();
		std::cout << e.what() << std::endl;
	}
	m_gl_view = std::make_shared<GOpenGLView>();
	m_gl_view->Init(item);

	try
	{
		item = m_json_cfg.get_child("GResourceMgr");
	}
	catch (std::exception &e)
	{
		item.clear();
		std::cout << e.what() << std::endl;
	}
	m_resource_mgr = std::make_shared<GResourceMgr>();
	m_resource_mgr->Init(item);

	m_phy_world = std::make_shared<GPhyWorld>();
	m_phy_world->Init();
}

double GWorld::GetTime()
{
    using namespace std::chrono;
    auto tp = std::chrono::system_clock::now();
    auto d = std::chrono::duration_cast<microseconds>(tp - g_begin_time);
    return (d.count() * 1.0f / 1e6);
}

double GWorld::GetDeltaTime()
{
    return g_delta_time;
}

std::shared_ptr<GPhyWorld> GWorld::PhyWorld()
{
    return m_phy_world;
}
std::shared_ptr<GOpenGLView> GWorld::GLView()
{
    return m_gl_view;
}

std::shared_ptr<GResourceMgr> GWorld::ResourceMgr()
{
    return m_resource_mgr;
}

int GWorld::Run()
{
    float per_frame = 0.017f;
    float exec_time = 0.0f;
    while(!m_gl_view->WindowShouldClose()){
        g_frame_begin = GetTime();
        m_scene->Update();
        exec_time = (float)(GetTime() - g_frame_begin);
        if(exec_time < per_frame){
            std::this_thread::sleep_for(std::chrono::milliseconds((int)((per_frame - exec_time) * 1000)));
        }
        g_delta_time = GetTime() - g_frame_begin;
    }
    return 0;
}

void GWorld::Poll()
{
    m_scene->Update();
}

void GWorld::SetScene(std::shared_ptr<GScene> s)
{
    m_scene = s;
    s->m_cur_scene = s;
}

NS_G4Y_END
