#ifndef __ROAMSCRIPT_H__
#define __ROAMSCRIPT_H__
#include "G4Y.hpp"
#include "imgui.h"

USING_NS_G4Y;

class RoamScript : public GCom
{
    G_COM
public:
    RoamScript(std::string title = "") :
        title(title)
    {}

    virtual void Start() override
    {
        m_transform = GetCom<GTransform>();
        auto eulers = m_transform.lock()->EulerAngles();
        pitch = eulers.x;
        yaw = eulers.y;
    }

    virtual void Update() override
    {
        // ImGui::Begin(title.c_str());
        // {
        //     auto p = m_transform.lock()->LocalPosition();
        //     auto r = m_transform.lock()->LocalEulerAngles();
        //     ImGui::Text("local pos     (%f, %f, %f)", p.x, p.y, p.z);
        //     ImGui::Text("local rotate  (%f, %f, %f)", r.x, r.y, r.z);
        // }
        // {
        //     auto p = m_transform.lock()->Position();
        //     auto r = m_transform.lock()->EulerAngles();
        //     auto s = m_transform.lock()->Scale();
        //     ImGui::Text("global pos    (%f, %f, %f)", p.x, p.y, p.z);
        //     ImGui::Text("global rotate (%f, %f, %f)", r.x, r.y, r.z);
        //     ImGui::Text("       scale  (%f, %f, %f)", s.x, s.y, s.z);
        // }
        // ImGui::End();
        auto& io = ImGui::GetIO();
        float delta_time = (float)GWorld::GetDeltaTime();
        float speed = 12.0f;
        if(ImGui::IsKeyDown(0x1A) || io.MouseWheel > 0.0f){
            m_transform.lock()->Translate(m_transform.lock()->Forward() * delta_time * speed * -1.0f);
        }
        if(ImGui::IsKeyDown(0x16) || io.MouseWheel < 0.0f){
            auto back = m_transform.lock()->Forward();
            m_transform.lock()->Translate(back * delta_time * speed);
        }
        if(ImGui::IsKeyDown(0x4)){
            auto left = m_transform.lock()->Position() + (m_transform.lock()->Right() * delta_time * speed * -1.0f);
            m_transform.lock()->SetPosition(left);
        }
        if(ImGui::IsKeyDown(0x7)){
            auto right = m_transform.lock()->Position() + (m_transform.lock()->Right() * delta_time * speed);
            m_transform.lock()->SetPosition(right);
        }
        if(ImGui::IsKeyDown(0x2C)){ // key space
            m_transform.lock()->Translate(glm::vec3(0, 1, 0) * delta_time * speed);
        }
        if(ImGui::IsKeyDown(0xE0)){ // key left ctrl
            m_transform.lock()->Translate(glm::vec3(0, -1, 0) * delta_time * speed);
        }

        if(ImGui::IsMouseDown(1) && io.MouseDelta.y != 0.0f){
            pitch += io.MouseDelta.y * 10 * delta_time;
        }
        if(ImGui::IsMouseDown(1) && io.MouseDelta.x != 0.0f){
            yaw += io.MouseDelta.x * 10 * delta_time;
        }
        if(ImGui::IsMouseDown(1) && (io.MouseDelta.x != 0.0f || io.MouseDelta.y != 0.0f)){
            m_transform.lock()->SetRotation(glm::vec3(-pitch, -yaw, 0));
        }
    }

    float pitch = 0.0f;
    float yaw = 0.0f;

    std::string title;
    std::weak_ptr<GTransform> m_transform;
};

#endif