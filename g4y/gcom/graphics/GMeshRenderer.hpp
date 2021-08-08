#ifndef __GMESHRENDERER_HPP__
#define __GMESHRENDERER_HPP__
#include <string>
#include <vector>

#include "GCom.hpp"

NS_G4Y_BEGIN

class GMesh;
class GMaterial;
class GTransform;
class GCamera;
class G4Y_DLL GMeshRenderer : public GCom
{
    G_COM
public:
    virtual void Start() override;

    virtual void OnRender() override;

    virtual void OnDestroy() override;

    std::vector<GMaterial> m_materials;
    std::weak_ptr<GMesh>   m_mesh;
    std::weak_ptr<GTransform> m_transform;
    std::weak_ptr<GCamera> m_camera;
};

NS_G4Y_END

#endif
