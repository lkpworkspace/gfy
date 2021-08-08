#ifndef __GCAMERA_H__
#define __GCAMERA_H__
#include <glm/glm.hpp>

#include "GCom.hpp"

NS_G4Y_BEGIN

class GTransform;
class G4Y_DLL GCamera : public GCom
{
    G_COM
public:
    enum CAMERRA_TYPE{ ORTHO, PERSPECTIVE };

    GCamera();
    virtual ~GCamera();

    virtual void Init() override;

    bool IsOrthographic() { return orthographic; }
    void SetCameraType(CAMERRA_TYPE t);

    glm::vec4 OrthoRect() { return glm::vec4(x, y, w, h); }
    void SetOrthoRect(glm::vec4 rect) { x = rect.x;  y = rect.y;  w = rect.z;  h = rect.w; }

    glm::vec2 FovAspect() { return glm::vec2(fov, aspect); }
    void SetFovAspect(glm::vec2 fa) { fov = fa.x; aspect = fa.y; }

    glm::vec2 NearFar() { return glm::vec2(near, far); }
    void SetNearFar(glm::vec2 nf){ near = nf.x; far = nf.y; }

    glm::mat4 Projection();
    glm::mat4 View();

	static GCamera* MainCamera() { return s_main_camera; }

private:
    // 摄像机的指向向量 = glm::normalize(position - target);
    bool orthographic;
    // for perspective
    float fov;
    float aspect;
    float near;
    float far;

    // for orthographic
    float x;
    float y;
    float w;
    float h;

    std::weak_ptr<GTransform> m_transform;

	static GCamera* s_main_camera;
};

NS_G4Y_END

#endif