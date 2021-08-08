#ifndef __GMESHCOLLIDER_H__
#define __GMESHCOLLIDER_H__
#include "GCollider.h"
#include <iostream>

class GTransform;
class GPhyWorld;
class G4Y_DLL GMeshCollider : public GCollider
{
public:
    GMeshCollider(){}
    virtual ~GMeshCollider(){}

    virtual void Init() override;

    virtual void Start() override;

    virtual void OnDestroy() override;

    virtual std::string ColliderName() { return "GMeshCollider"; }

private:
    std::weak_ptr<GPhyWorld>              m_phy_world;
    std::weak_ptr<GTransform>             m_transform;
};

#endif