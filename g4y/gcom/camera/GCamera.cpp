#include "GCamera.hpp"
#include "GObj.hpp"
#include "GTransform.hpp"

NS_G4Y_BEGIN

GCamera* GCamera::s_main_camera = nullptr;

GCamera::GCamera() :
    orthographic(false),
    fov(60.0f),
    aspect(1.0f),
    near(0.1f),
    far(1000.0f),
    x(0.0f),
    y(0.0f),
    w(800.0f),
    h(600.0f)
{
	s_main_camera = this;
}

GCamera::~GCamera()
{}

void GCamera::Init()
{
    m_transform = GetCom<GTransform>();
	Obj()->SetTag("GCamera");
}

glm::mat4 GCamera::Projection()
{
    if(!orthographic)
        return glm::perspective(glm::radians(fov), aspect, near, far);
    else
        return glm::ortho(x, w, y, h, near, far);
}

void GCamera::SetCameraType(CAMERRA_TYPE t)
{
    orthographic = (t == ORTHO) ? true : false;
}

glm::mat4 GCamera::View()
{
    glm::mat4 translate(1.0f);
    
    translate = glm::translate(translate, m_transform.lock()->Position() * -1.0f);
    
    auto q = m_transform.lock()->Rotation();

    return glm::mat4_cast(glm::inverse(q)) * translate;
}

NS_G4Y_END