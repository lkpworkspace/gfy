#ifndef __MoveScript_H__
#define __MoveScript_H__
#include "G4Y.hpp"
#include "imgui.h"

USING_NS_G4Y;

class MoveScript : public GCom
{
public:
    MoveScript(std::string title, glm::vec3 pos = glm::vec3(0,0,0), glm::vec3 rot = glm::vec3(0,0,0)) :
        title(title),
        x(pos.x),
        y(pos.y),
        z(pos.z),
        rx(rot.x),
        ry(rot.y),
        rz(rot.z)
    {
    }

    virtual void Update() override
    {
        ImGui::Begin(title.c_str());

        bool slider_pos[3];
        slider_pos[0] = ImGui::SliderFloat("Move x", &x, -150.0f, 150.0f);
        slider_pos[1] = ImGui::SliderFloat("Move y", &y, -150.0f, 150.0f);
        slider_pos[2] = ImGui::SliderFloat("Move z", &z, -150.0f, 150.0f);
        if(slider_pos[0] || slider_pos[1] || slider_pos[2])
			GetCom<GTransform>()->SetPosition(glm::vec3(x,y,z));

        bool slider_rotate[3];
        slider_rotate[0] = ImGui::SliderFloat("Rotate x", &rx, -180.0f, 180.0f);
        slider_rotate[1] = ImGui::SliderFloat("Rotate y", &ry, -180.0f, 180.0f);
        slider_rotate[2] = ImGui::SliderFloat("Rotate z", &rz, -180.0f, 180.0f);
        if(slider_rotate[0] || slider_rotate[1] || slider_rotate[2])
			GetCom<GTransform>()->SetRotation(glm::vec3(rx,ry,rz));

        ImGui::End();
    }

    float x;
    float y;
    float z;
    float rx = -20.0f;
    float ry = 0.0f;
    float rz = 0.0f;

    std::string title;
};

#endif