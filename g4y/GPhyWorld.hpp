#ifndef __GPHYWORLD_H__
#define __GPHYWORLD_H__
#include <string>
#include <memory>
#include <vector>
#include <glm/glm.hpp>

#include "GConfig.hpp"

class btCollisionObject;
class btRigidBody;
class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btBroadphaseInterface;
class btSequentialImpulseConstraintSolver;
class btDiscreteDynamicsWorld;

NS_G4Y_BEGIN

class GObj;
class GPhyObj;
class GPhyDebugDraw;
class G4Y_DLL GRayHit
{
public:
    glm::vec3           pick_pos;
    std::weak_ptr<GObj> obj;
};

class G4Y_DLL GPhyWorld final
{
public:
	~GPhyWorld();

    void Init();

	void DebugDraw(glm::mat4& mvp);

	void AddPhyObj(GPhyObj* col_obj);
	void DelPhyObj(GPhyObj* col_obj);

    bool RayTest(glm::vec3 from, glm::vec3 to, GRayHit& hit);

    void UpdateDynamicsWorld();

	void PreSimulate();

	void PostSimulate();

	void RemoveCol(GPhyObj* col);
// private:
    std::shared_ptr<btDefaultCollisionConfiguration>        m_collision_cfg;
    std::shared_ptr<btCollisionDispatcher>                  m_collision_dispatcher;
    std::shared_ptr<btBroadphaseInterface>                  m_broadphase;
    std::shared_ptr<btSequentialImpulseConstraintSolver>    m_solver;
    std::shared_ptr<btDiscreteDynamicsWorld>                m_dynamics_world;

    std::vector<GPhyObj*>                         m_all_col_objs;
	std::vector<GPhyObj*>                         m_remove_col_objs;

	std::shared_ptr<GPhyDebugDraw>                          m_debug_draw;
};

NS_G4Y_END

#endif