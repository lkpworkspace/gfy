#ifndef __GCOLLIDER_H__
#define __GCOLLIDER_H__
#include <iostream>
#include <set>
#include <boost/signals2.hpp>
#include <glm/glm.hpp>
#include "glm/gtx/quaternion.hpp"

#include "GPhysics.hpp"

class btCollisionObject;
class btCollisionShape;
class btGhostObject;

NS_G4Y_BEGIN

class GCollider;
class GTransform;
class GPhyWorld;
class GCollision
{
public:
    std::shared_ptr<GCollider> collider;
};

struct G4Y_DLL GColliderDes final
{
	bool          is_trigger;

	std::shared_ptr<GPhyShape> shape;

	explicit GColliderDes(std::shared_ptr<GPhyShape> c, bool trigger = false)
		: shape(c)
		, is_trigger(trigger)
	{}

	explicit GColliderDes(const GColliderDes& o)
	{
		operator=(o);
	}

	GColliderDes& operator=(const GColliderDes &o)
	{
		is_trigger = o.is_trigger;
		shape = o.shape;
		return *this;
	}
};

class G4Y_DLL GCollider final : public GPhyObj
{
public:
	virtual std::string TypeName() final { return typeid(GCollider).name(); }

public:
	explicit GCollider(GColliderDes& des);

	virtual void Init() override;

	virtual void OnDestroy() override;

	/**
	* synchronize node transformation to physics
	*/
	virtual void SyncNodeToPhysics() override;

	/**
	 * synchronize physics transformation to node
	 */
	virtual void SyncPhysicsToNode() override;

    virtual std::string ColliderName() { return "GCollider"; }

	void SetTrigger(bool is_trigger);

protected:
	std::weak_ptr<GTransform>    m_transform;
	std::weak_ptr<GPhyWorld>     m_phy_world;

	GColliderDes                 m_des;
};

NS_G4Y_END

#endif