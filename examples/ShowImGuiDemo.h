#ifndef __SHOWIMGUIDEMO_H__
#define __SHOWIMGUIDEMO_H__
#include "G4Y.hpp"
#include "imgui.h"

USING_NS_G4Y;

class ShowImGuiDemoScript : public GCom
{
	G_COM
public:
	virtual void Update() override
	{
		bool show_demo = true;
		ImGui::ShowDemoWindow(&show_demo);
		//ImGui::Begin("ImGui Demo");	
		//ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		//ImGui::End();
	}
};

#endif