#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#include "GOpenGLView.hpp"
#include "GShader.hpp"
#include "GWorld.hpp"
#include "GResourceMgr.hpp"

NS_G4Y_BEGIN

void GOpenGLView::LoadConf(const boost::property_tree::ptree& cfg)
{
	if (cfg.empty()) return;
	m_app_title = cfg.get("app_title", m_app_title);
	m_window_width = cfg.get("window_width", m_window_width);
	m_window_height = cfg.get("window_height", m_window_height);
	m_window_pos_x = cfg.get("window_pos_x", m_window_pos_x);
	m_window_pos_y = cfg.get("window_pos_y", m_window_pos_y);
	m_window_resize = cfg.get("window_resize", m_window_resize);
	m_fullscreen = cfg.get("fullscreen", m_fullscreen);
	m_maximized = cfg.get("maximized", m_maximized);
	m_font_path = cfg.get("font_name", m_font_path);
	m_font_size = cfg.get("font_size", m_font_size);
}

int GOpenGLView::Init(const boost::property_tree::ptree& cfg)
{
	LoadConf(cfg);

	// init sdl2
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO) < 0) {
		std::cout << "Failed to initialize SDL2" << std::endl;
		return -1;
	}
	else
		std::cout << "Initialized SDL2" << std::endl;

	// load window size
	short wndWidth = m_window_width, wndHeight = m_window_height, wndPosX = m_window_pos_x, wndPosY = m_window_pos_y;
	bool fullscreen = m_fullscreen, maximized = m_maximized;

	// clamp to desktop size
	SDL_DisplayMode desk;
	SDL_GetCurrentDisplayMode(0, &desk);
	if (wndWidth > desk.w)
		wndWidth = desk.w;
	if (wndHeight > desk.h)
		wndHeight = desk.h;

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); // double buffering
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

	Uint32 win_flag = SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI;
	if (m_window_resize) win_flag |= SDL_WINDOW_RESIZABLE;
	// open window
	m_wnd = SDL_CreateWindow(m_app_title.c_str(),
		(wndPosX == -1) ? SDL_WINDOWPOS_CENTERED : wndPosX,
		(wndPosY == -1) ? SDL_WINDOWPOS_CENTERED : wndPosY,
		wndWidth, wndHeight,
		win_flag);

	SDL_SetWindowMinimumSize(m_wnd, 200, 200);

	if (maximized) SDL_MaximizeWindow(m_wnd);
	if (fullscreen) SDL_SetWindowFullscreen(m_wnd, SDL_WINDOW_FULLSCREEN_DESKTOP);

	// get GL context
	m_glContext = SDL_GL_CreateContext(m_wnd);
	SDL_GL_MakeCurrent(m_wnd, m_glContext);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);

	// init glew
	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}
	else
		std::cout << "Initialized GLEW" << std::endl;

	// Initialize imgui
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	//io.Fonts->AddFontDefault();
	SetFont();

	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard
		| ImGuiConfigFlags_NoMouseCursorChange
		| ImGuiConfigFlags_DockingEnable
		/*| ImGuiConfigFlags_ViewportsEnable TODO: allow this on windows? test on linux?*/;
	io.ConfigDockingWithShift = false;

	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowMenuButtonPosition = ImGuiDir_Right;

	ImGui_ImplOpenGL3_Init("#version 330");
	ImGui_ImplSDL2_InitForOpenGL(m_wnd, m_glContext);

	ImGui::StyleColorsClassic();
	//ImGui::StyleColorsDark();

	// dpi
	float dpi = 0.0f;
	int wndDisplayIndex = SDL_GetWindowDisplayIndex(m_wnd);
	SDL_GetDisplayDPI(wndDisplayIndex, &dpi, NULL, NULL);
	std::cout << std::to_string(dpi) + " dpi" << std::endl;
	return 0;
}

void GOpenGLView::SetFont()
{
	if (m_font_path.empty()) return;
	auto full_path = GWorld::Instance()->GetAssetpath() + m_font_path;

	ImFontAtlas* fonts = ImGui::GetIO().Fonts;
	fonts->Clear();

	ImFontConfig config;
	config.MergeMode = false;
	config.OversampleH = 1;
	config.GlyphOffset = ImVec2(0, -1);

	ImFont* font = fonts->AddFontFromFileTTF(full_path.c_str(), m_font_size, &config, fonts->GetGlyphRangesChineseFull());
	if (font == nullptr) {
		fonts->Clear();
		fonts->AddFontDefault();
	}
	fonts->Build();

	ImGui_ImplOpenGL3_DestroyFontsTexture();
	ImGui_ImplOpenGL3_CreateFontsTexture();
}

std::shared_ptr<GShader> GOpenGLView::GetShader()
{ 
	if (m_main_shader == nullptr) {
		m_main_shader = GWorld::Instance()->ResourceMgr()->Shader("main_shader");
	}
	return m_main_shader; 
}

bool GOpenGLView::WindowShouldClose()
{
	return m_quit;
}

int GOpenGLView::PollEvents()
{
	ImGuiIO& io = ImGui::GetIO();
	SDL_Event event;
	bool minimized = false;
	bool hasFocus = true;
	bool maximized = false;
	bool fullscreen = false;
	int rect[4] = { 0, 0, 0, 0 };
	
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT) {
			m_quit = true;
			std::cout << "Received SDL_QUIT event -> quitting" << std::endl;
		}
		else if (event.type == SDL_WINDOWEVENT) {
			if (event.window.event == SDL_WINDOWEVENT_MOVED || event.window.event == SDL_WINDOWEVENT_MAXIMIZED || event.window.event == SDL_WINDOWEVENT_RESIZED) {
				Uint32 wndFlags = SDL_GetWindowFlags(m_wnd);

				maximized = wndFlags & SDL_WINDOW_MAXIMIZED;
				fullscreen = wndFlags & SDL_WINDOW_FULLSCREEN_DESKTOP;
				minimized = false;

				// cache window size and position
				if (!maximized) {
					int tempX = 0, tempY = 0;
					SDL_GetWindowPosition(m_wnd, &tempX, &tempY);
					rect[0] = tempX;
					rect[1] = tempY;
					SDL_GetWindowSize(m_wnd, &tempX, &tempY);
					rect[2] = tempX;
					rect[3] = tempY;
				}
			}
			else if (event.window.event == SDL_WINDOWEVENT_MINIMIZED)
				minimized = true;
			else if (event.window.event == SDL_WINDOWEVENT_FOCUS_LOST)
				hasFocus = false;
			else if (event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED)
				hasFocus = true;
		}
		ImGui_ImplSDL2_ProcessEvent(&event);
	}
	return 0;
}

void GOpenGLView::BeginRender()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(m_wnd);
	ImGui::NewFrame();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void GOpenGLView::EndRender()
{
	// render ImGUI
	ImGui::Render();

	ImDrawData* drawData = ImGui::GetDrawData();
	if (drawData != NULL) {
		// actually render to back buffer
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Update and Render additional Platform Windows
		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}
	}
	SDL_GL_SwapWindow(m_wnd);
}

void GOpenGLView::Exit()
{
	// sdl2
	SDL_GL_DeleteContext(m_glContext);
	SDL_DestroyWindow(m_wnd);
	SDL_Quit();

	// imgui
	ImGui_ImplSDL2_Shutdown();
	ImGui_ImplOpenGL3_Shutdown();
	ImGui::DestroyContext();
}

NS_G4Y_END