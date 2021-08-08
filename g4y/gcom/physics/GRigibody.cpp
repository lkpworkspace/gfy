#include <cassert>
#include <btBulletDynamicsCommon.h>

#include "GRigibody.hpp"
#include "GPhysics.hpp"
#include "GWorld.hpp"
#include "GPhyWorld.hpp"
#include "GTransform.hpp"

NS_G4Y_BEGIN

btVector3 convert_vec3_to_btvec3(const glm::vec3& vec3);

static void get_world_transform(std::shared_ptr<GTransform> trans_com, btTransform& worldTrans)
{
	auto p = trans_com->Position();
	auto q = trans_com->Rotation();
	worldTrans.setOrigin(btVector3(p.x, p.y, p.z));
	worldTrans.setRotation(btQuaternion(q.x, q.y, q.z, q.w));
}

static void set_world_transform(std::shared_ptr<GTransform> trans_com, const btTransform& worldTrans)
{
	btTransform trans = worldTrans;
	btScalar rx, ry, rz;
	trans.getRotation().getEulerZYX(rz, ry, rx);
	trans_com->SetPosition(glm::vec3(trans.getOrigin().x(), trans.getOrigin().y(), trans.getOrigin().z()));
	trans_com->SetRotation(glm::vec3(rx, ry, rz));
}

GRigibody::GRigibody(GRigidBodyDes& des)
	: GPhyObj(GPhyObj::PhyObjType::RIGID_BODY)
	, m_des(des)
{}

void GRigibody::Init()
{
	m_transform = GetCom<GTransform>();
	m_phy_world = GWorld::Instance()->PhyWorld();

	auto collider = m_des.shape;

	btVector3 inertia = convert_vec3_to_btvec3(m_des.local_inertia);
	collider->GetbtShape()->calculateLocalInertia(m_des.mass, inertia);

	btTransform bttrans;
	get_world_transform(m_transform.lock(), bttrans);
	btDefaultMotionState* def_motion = new btDefaultMotionState(bttrans);
	btRigidBody::btRigidBodyConstructionInfo rbinfo(m_des.mass, def_motion, collider->GetbtShape(), inertia);
	auto rigidbody = std::make_shared<btRigidBody>(rbinfo);

	m_col_obj = std::static_pointer_cast<btCollisionObject>(rigidbody);

	rigidbody->setUserPointer(this);

	if (m_des.disable_sleep) rigidbody->setActivationState(DISABLE_DEACTIVATION);
	m_phy_world.lock()->AddPhyObj(this);
}

void GRigibody::OnDestroy()
{
	auto rigi = std::static_pointer_cast<btRigidBody>(m_col_obj);

	delete rigi->getMotionState();
	m_phy_world.lock()->DelPhyObj(this);
}

void GRigibody::SyncNodeToPhysics()
{
	auto rigi = std::static_pointer_cast<btRigidBody>(m_col_obj);
    auto motionState = rigi->getMotionState();

    btTransform trans;
    auto p = m_transform.lock()->Position();
    auto q = m_transform.lock()->Rotation();
    trans.setOrigin(btVector3(p.x, p.y, p.z));
    trans.setRotation(btQuaternion(q.x, q.y, q.z, q.w));

    motionState->setWorldTransform(trans);
	rigi->setMotionState(motionState);
}

void GRigibody::SyncPhysicsToNode()
{
	if (IsKinematic()) return;

	auto rigi = std::static_pointer_cast<btRigidBody>(m_col_obj);
    btTransform trans = rigi->getWorldTransform();;
    btScalar rx, ry, rz;
    trans.getRotation().getEulerZYX(rz, ry, rx);
    m_transform.lock()->SetPosition(glm::vec3(trans.getOrigin().x(), trans.getOrigin().y(), trans.getOrigin().z()));
    m_transform.lock()->SetRotation(glm::vec3(rx, ry, rz));
}

/** Set kinematic object. */
void GRigibody::SetKinematic(bool kinematic)
{
	if (kinematic)
	{
		m_col_obj->setCollisionFlags(m_col_obj->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
		m_col_obj->setActivationState(DISABLE_DEACTIVATION);
	}
	else
	{
		m_col_obj->setCollisionFlags(m_col_obj->getCollisionFlags() & ~btCollisionObject::CF_KINEMATIC_OBJECT);
		m_col_obj->setActivationState(ACTIVE_TAG);
	}
}

/** Check rigid body is kinematic object. */
bool GRigibody::IsKinematic() const
{
	if (m_col_obj)
		return m_col_obj->isKinematicObject();
	return false;
}

NS_G4Y_END