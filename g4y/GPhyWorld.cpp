#include <btBulletDynamicsCommon.h>
#include "BulletCollision/NarrowPhaseCollision/btRaycastCallback.h"

#include "GPhyWorld.hpp"
#include "GCommon.hpp"
#include "GCollider.hpp"
#include "GRigibody.hpp"
#include "GPhyDebugDraw.hpp"
#include "GPhysics.hpp"

NS_G4Y_BEGIN

glm::vec3 convert_btvec3_to_vec3(const btVector3& btVec3);
btVector3 convert_vec3_to_btvec3(const glm::vec3& vec3);

GPhyWorld::~GPhyWorld()
{

}

void GPhyWorld::Init()
{
    m_collision_cfg             = std::make_shared<btDefaultCollisionConfiguration>();
    m_collision_dispatcher      = std::make_shared<btCollisionDispatcher>(m_collision_cfg.get());
    m_broadphase                = std::static_pointer_cast<btBroadphaseInterface>(std::make_shared<btDbvtBroadphase>());
    m_solver                    = std::make_shared<btSequentialImpulseConstraintSolver>();
    m_dynamics_world            = std::make_shared<btDiscreteDynamicsWorld>(
        m_collision_dispatcher.get(),
        m_broadphase.get(),
        m_solver.get(),
        m_collision_cfg.get()
    );

    m_dynamics_world->setGravity(btVector3(0, -10, 0));

	m_debug_draw = std::make_shared<GPhyDebugDraw>();
	m_debug_draw->Init();

	m_dynamics_world->setDebugDrawer(m_debug_draw.get());
}

void GPhyWorld::DebugDraw(glm::mat4& mvp)
{
	m_debug_draw->Draw(mvp);
}

bool GPhyWorld::RayTest(glm::vec3 from, glm::vec3 to, GRayHit& hit)
{
    btVector3 f(from.x, from.y, from.z);
    btVector3 t(to.x, to.y, to.z);
    btCollisionWorld::ClosestRayResultCallback ray_cb(f, t);
    ray_cb.m_flags |= btTriangleRaycastCallback::kF_UseGjkConvexCastRaytest;
    m_dynamics_world->rayTest(f, t, ray_cb);
    if(ray_cb.hasHit()){
        btVector3 pick_pos = ray_cb.m_hitPointWorld;
        hit.pick_pos = glm::vec3(pick_pos[0], pick_pos[1], pick_pos[2]);
        void* p = ray_cb.m_collisionObject->getUserPointer();
        auto com = static_cast<GCom*>(p);
        hit.obj = com->Obj();
        return true;
    }
    return false;
}

void GPhyWorld::AddPhyObj(GPhyObj* col_obj)
{
	if (col_obj->GetObjType() == GPhyObj::PhyObjType::COLLIDER) {
		m_dynamics_world->addCollisionObject(col_obj->GetbtCol().get());
	}
	else if (col_obj->GetObjType() == GPhyObj::PhyObjType::RIGID_BODY) {
		auto btrigi = col_obj->GetbtCol().get();
		auto rb = static_cast<btRigidBody*>(btrigi);
		m_dynamics_world->addRigidBody(rb);
	}
}
void GPhyWorld::DelPhyObj(GPhyObj* col_obj)
{
	if (col_obj->GetObjType() == GPhyObj::PhyObjType::COLLIDER) {
		m_dynamics_world->removeCollisionObject(col_obj->GetbtCol().get());
	}
	else if (col_obj->GetObjType() == GPhyObj::PhyObjType::RIGID_BODY) {
		auto btrigi = col_obj->GetbtCol().get();
		auto rb = static_cast<btRigidBody*>(btrigi);
		m_dynamics_world->removeRigidBody(rb);
	}
	else {
		assert(false);
	}
	RemoveCol(col_obj);
}

void GPhyWorld::RemoveCol(GPhyObj* col)
{
	for (auto it = m_all_col_objs.begin(); it != m_all_col_objs.end();) {
		(*it)->OnCollisionRemove(col);
		if (col == *it) {
			it = m_all_col_objs.erase(it);
		}
		else {
			++it;
		}
	}
}

void GPhyWorld::PreSimulate()
{
	for (const auto& o : m_all_col_objs) {
		o->SyncNodeToPhysics();
	}
}

void GPhyWorld::PostSimulate()
{
	for (const auto& o : m_all_col_objs) {
		o->SyncPhysicsToNode();
	}
}

void GPhyWorld::UpdateDynamicsWorld()
{
	PreSimulate();
    m_dynamics_world->stepSimulation (1.f /60.f, 10);
	PostSimulate();

	m_dynamics_world->debugDrawWorld();

    m_all_col_objs.clear();
    for(int i =  m_dynamics_world->getNumCollisionObjects() - 1; i >= 0; --i){
        btCollisionObject* obj = m_dynamics_world->getCollisionObjectArray()[i];
		void* p = obj->getUserPointer();
		auto col_obj = static_cast<GPhyObj*>(p);
		m_all_col_objs.push_back(col_obj);

		/* test begin */
		//btRigidBody* body = btRigidBody::upcast(obj);
		//btTransform trans;
		//if (body && body->getMotionState()) {
		//	body->getMotionState()->getWorldTransform(trans);
		//}
		//else {
		//	trans = obj->getWorldTransform();
		//}
		//btScalar x, y, z;
		//trans.getRotation().getEulerZYX(x, y, z);
		//printf("world pos object %d = %f, %f, %f; %f %f %f\n",
		//	i,
		//	float(trans.getOrigin().getX()),
		//	float(trans.getOrigin().getY()),
		//	float(trans.getOrigin().getZ()),
		//	x, y, z
		//);
		/* test end */
    }

    int num_manifolds = m_dynamics_world->getDispatcher()->getNumManifolds();
    for(int i = 0; i < num_manifolds; ++i){
        btPersistentManifold* contact_manifold = m_dynamics_world->getDispatcher()->getManifoldByIndexInternal(i);
        int num_contacts = contact_manifold->getNumContacts();

		if (0 < num_contacts) {
			const btCollisionObject* obA = static_cast<const btCollisionObject*>(contact_manifold->getBody0());
			const btCollisionObject* obB = static_cast<const btCollisionObject*>(contact_manifold->getBody1());
			void* pA = obA->getUserPointer();
			void* pB = obB->getUserPointer();
			auto phyA = static_cast<GPhyObj*>(pA);
			auto phyB = static_cast<GPhyObj*>(pB);

			GPhyCollisionInfo ci;
			ci.objA = phyA;
			ci.objB = phyB;
			// printf("collision A: %s, B: %s, i: %d, num_point: %d\n", comA->TypeName().c_str(), comB->TypeName().c_str(), i, num_contacts);
			for(int j = 0; j < num_contacts; ++j){
				btManifoldPoint& pt = contact_manifold->getContactPoint(j);
				if(pt.getDistance() <= 0.f){

					GPhyCollisionInfo::CollisionPoint cp = {
							  convert_btvec3_to_vec3(pt.m_localPointA), convert_btvec3_to_vec3(pt.m_positionWorldOnA)
							, convert_btvec3_to_vec3(pt.m_localPointB), convert_btvec3_to_vec3(pt.m_positionWorldOnB)
							, convert_btvec3_to_vec3(pt.m_normalWorldOnB)
					};
					ci.collisionPointList.push_back(cp);

					// const btVector3& ptA = pt.getPositionWorldOnA();
					// const btVector3& ptB = pt.getPositionWorldOnB();
					// printf("world col objectA %d = %f, %f, %f \nworld col objectB %d = %f, %f, %f \n", 
					//     i, 
					//     float(ptA.getX()), 
					//     float(ptA.getY()), 
					//     float(ptA.getZ()),
					//     i, 
					//     float(ptB.getX()), 
					//     float(ptB.getY()), 
					//     float(ptB.getZ())
					// );
				}
			}

			ci.target = phyB;
			phyA->OnCollision(ci);

			ci.target = phyA;
			phyB->OnCollision(ci);
		}
    }

    // 调用所有碰撞对象的OnCollisionEnd函數
    for(const auto& o : m_all_col_objs){
		o->OnCollisionEnd();
    }
}

NS_G4Y_END