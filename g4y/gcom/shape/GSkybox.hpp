#ifndef __GSKYBOX_H__
#define __GSKYBOX_H__
#include <string>
#include <vector>

#include "GCom.hpp"

NS_G4Y_BEGIN

class GShader;
class GCamera;
class GTransform;
class G4Y_DLL GSkybox : public GCom
{
    G_COM
public:
    /* 右左 上下 后前 */
    GSkybox(std::vector<std::string> tex_path) :
        m_tex_path(tex_path.begin(), tex_path.end())
    {}
    virtual ~GSkybox(){}

    virtual void Start() override;

    virtual void OnRender() override;

    virtual void OnDestroy() override;

private:
    void LoadCubemap();
    
    unsigned int VAO, VBO;
    unsigned int texture_id;
    std::shared_ptr<GShader> m_shader;
    std::weak_ptr<GCamera>   m_camera;
    std::weak_ptr<GTransform>m_transform;
    std::vector<std::string> m_tex_path;
};

NS_G4Y_END

#endif