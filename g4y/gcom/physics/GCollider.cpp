#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>

#include "GCollider.hpp"
#include "GCommon.hpp"
#include "GTransform.hpp"
#include "GWorld.hpp"
#include "GPhyWorld.hpp"

NS_G4Y_BEGIN

GCollider::GCollider(GColliderDes& des)
	: GPhyObj(GPhyObj::PhyObjType::COLLIDER)
	, m_des(des)
{}

void GCollider::Init()
{
	m_phy_world = GWorld::Instance()->PhyWorld();

	auto ghost_obj = std::make_shared<btGhostObject>();
	m_col_obj = std::static_pointer_cast<btCollisionObject>(ghost_obj);

	ghost_obj->setCollisionShape(m_des.shape->GetbtShape());
	ghost_obj->setUserPointer(this);
	SetTrigger(m_des.is_trigger);

	m_phy_world.lock()->AddPhyObj(this);

	auto trans = GetCom<GTransform>();
	m_transform = trans;
}

void GCollider::OnDestroy()
{
	m_phy_world.lock()->DelPhyObj(this);
}

void GCollider::SetTrigger(bool is_trigger)
{
	m_col_obj->setCollisionFlags(is_trigger == true ?
		m_col_obj->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE :
		m_col_obj->getCollisionFlags() & ~btCollisionObject::CF_NO_CONTACT_RESPONSE);
}

void GCollider::SyncNodeToPhysics()
{
	btTransform trans;
	auto p = m_transform.lock()->Position();
	auto q = m_transform.lock()->Rotation();
	trans.setOrigin(btVector3(p.x, p.y, p.z));
	trans.setRotation(btQuaternion(q.x, q.y, q.z, q.w));

	m_col_obj->setWorldTransform(trans);
}


void GCollider::SyncPhysicsToNode()
{
	btTransform trans = m_col_obj->getWorldTransform();;
	btScalar rx, ry, rz;
	trans.getRotation().getEulerZYX(rz, ry, rx);
	m_transform.lock()->SetPosition(glm::vec3(trans.getOrigin().x(), trans.getOrigin().y(), trans.getOrigin().z()));
	m_transform.lock()->SetRotation(glm::vec3(rx, ry, rz));
}

NS_G4Y_END