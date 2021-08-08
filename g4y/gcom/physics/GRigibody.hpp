#ifndef __GRIGIBODY_H__
#define __GRIGIBODY_H__
#include <set>

#include "GPhysics.hpp"

class btRigidBody;
class btCollisionObject;

NS_G4Y_BEGIN

class GTransform;
class GPhyWorld;
struct G4Y_DLL GRigidBodyDes final
{
	/* Note: mass equals zero means static, default 1 */
	float                           mass;
	/* default (0, 0, 0) */
	glm::vec3                       local_inertia; 
	/* it is always active if disabled */
	bool                            disable_sleep;

	std::shared_ptr<GPhyShape>      shape;

	explicit GRigidBodyDes(std::shared_ptr<GPhyShape> c)
		: mass(1.f)
		, local_inertia(0.f, 0.f, 0.f)
		, disable_sleep(true)
		, shape(c)
	{}

	explicit GRigidBodyDes(const GRigidBodyDes& o)
	{
		operator=(o);
	}

	GRigidBodyDes& operator=(const GRigidBodyDes &o)
	{
		mass = o.mass;
		local_inertia = o.local_inertia;
		disable_sleep = o.disable_sleep;
		shape = o.shape;
		return *this;
	}
};

class G4Y_DLL GRigibody final : public g4y::GPhyObj
{
	friend class GPhyWorld;
    G_COM
public:
	explicit GRigibody(GRigidBodyDes& des);

	virtual void Init() override;

	virtual void OnDestroy() override;

	/** Set kinematic object. */
	void SetKinematic(bool kinematic);

	/** Check rigid body is kinematic object. */
	bool IsKinematic() const;

    std::weak_ptr<GPhyWorld>              m_phy_world;
    std::weak_ptr<GTransform>             m_transform;

   /**
     * synchronize node transformation to physics
     */
    virtual void SyncNodeToPhysics() override;
    
    /**
     * synchronize physics transformation to node
     */
    virtual void SyncPhysicsToNode() override;
private:

	GRigidBodyDes m_des;
};

NS_G4Y_END

#endif