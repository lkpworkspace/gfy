#ifndef __GOPENGLVIEW_H__
#define __GOPENGLVIEW_H__
#include <memory>
#include <boost/property_tree/ptree.hpp>
#include <SDL2/SDL.h>

#include "GConfig.hpp"

NS_G4Y_BEGIN

class GShader;
class G4Y_DLL GOpenGLView
{
public:
    int Init(const boost::property_tree::ptree& cfg);

	std::shared_ptr<GShader> GetShader();

	bool WindowShouldClose();

    int PollEvents();

    void BeginRender();

    void EndRender();

    void Exit();

private:
	void SetFont();
	void LoadConf(const boost::property_tree::ptree& cfg);

    std::shared_ptr<GShader> m_main_shader;
	bool            m_quit;
	SDL_Window*     m_wnd;
	SDL_GLContext   m_glContext;

	std::string m_font_path = "";
	int m_font_size = 16;
	std::string m_app_title = "G4Y";
	int m_window_width = 1920;
	int m_window_height = 1080;
	int m_window_pos_x = -1;
	int m_window_pos_y = -1;
	bool m_window_resize = true;
	bool m_fullscreen = false;
	bool m_maximized = false;
};

NS_G4Y_END

#endif