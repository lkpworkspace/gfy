#ifndef __GPhysics_hpp__
#define __GPhysics_hpp__
#include <set>
#include <vector>
#include <glm/glm.hpp>

#include "GCom.hpp"

class btCollisionShape;
class btCollisionObject;

NS_G4Y_BEGIN

class GPhyObj;

class G4Y_DLL GPhyShape
{
public:
	enum class ShapeType
	{
		UNKNOWN = 0,
		BOX,
		SPHERE,
		CYLINDER,
		CAPSULE,
		CONVEX,
		MESH,
		HEIGHT_FIELD,
		COMPOUND
	};
	GPhyShape();
	~GPhyShape();

	virtual ShapeType GetShapeType() const { return m_shape_type; }

	static std::shared_ptr<GPhyShape> CreateBox(const glm::vec3& extent);

	static std::shared_ptr<GPhyShape> CreateStaticPlane(const glm::vec3& normal, float constant);

	btCollisionShape* GetbtShape() const { return m_shape; }
protected:

	bool InitBox(const glm::vec3& ext);
	bool InitStaticPlane(const glm::vec3& normal, float constant);

	ShapeType             m_shape_type;
	btCollisionShape*     m_shape;
};

struct G4Y_DLL GPhyCollisionInfo
{
	struct CollisionPoint
	{
		glm::vec3 localPositionOnA;
		glm::vec3 worldPositionOnA;
		glm::vec3 localPositionOnB;
		glm::vec3 worldPositionOnB;
		glm::vec3 worldNormalOnB;
	};

	GPhyObj *objA;
	GPhyObj *objB;
	GPhyObj *target;
	std::vector<CollisionPoint> collisionPointList;
};

class G4Y_DLL GPhyObj : public GCom
{
	G_COM
public:
	enum class PhyObjType
	{
		UNKNOWN = 0,
		RIGID_BODY,
		COLLIDER,
	};

	explicit GPhyObj(PhyObjType type = PhyObjType::UNKNOWN);

	virtual PhyObjType GetObjType() const { return m_type; }

	void OnCollision(const GPhyCollisionInfo& col_info);

	void OnCollisionRemove(GPhyObj*);

	void OnCollisionEnd();

	/**
	* synchronize node transformation to physics
	*/
	virtual void SyncNodeToPhysics() = 0;

	/**
	 * synchronize physics transformation to node
	 */
	virtual void SyncPhysicsToNode() = 0;

	std::shared_ptr<btCollisionObject> GetbtCol() { return m_col_obj; }

	bool IsTrigger();

protected:

	std::shared_ptr<btCollisionObject> m_col_obj;

private:
	bool IsTrigger(GPhyObj *col);
	bool IsTrigger(std::shared_ptr<btCollisionObject> btcol);

	PhyObjType     m_type;
	std::set<GPhyObj*>           m_last_cols;
	std::set<GPhyObj*>           m_cur_cols;
};

NS_G4Y_END

#endif
