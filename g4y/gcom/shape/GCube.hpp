#ifndef __GCUBE_H__
#define __GCUBE_H__
#include <glm/glm.hpp>

#include "GCom.hpp"

NS_G4Y_BEGIN

class GShader;
class GCamera;
class GTransform;
class G4Y_DLL GCube : public GCom
{
    G_COM
public:
    GCube();
    virtual ~GCube();

    virtual void Start() override;

    virtual void OnRender() override;

    virtual void OnDestroy() override;

    void SetColor(glm::vec4 col);
    glm::vec4 Color() { return color; }

private:
    glm::vec4 color;
    unsigned int VAO, VBO;
    std::shared_ptr<GShader> m_shader;
    std::weak_ptr<GCamera>   m_camera;
    std::weak_ptr<GTransform>m_transform;
};

NS_G4Y_END

#endif