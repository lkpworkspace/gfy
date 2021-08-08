#include <btBulletDynamicsCommon.h>
#include "GPhysics.hpp"

NS_G4Y_BEGIN

	glm::vec3 convert_btvec3_to_vec3(const btVector3& btVec3)
	{
		return glm::vec3(btVec3.x(), btVec3.y(), btVec3.z());
	}
	btVector3 convert_vec3_to_btvec3(const glm::vec3& vec3)
	{
		return btVector3(vec3.x, vec3.y, vec3.z);
	}

	GPhyShape::GPhyShape()
		: m_shape_type(ShapeType::UNKNOWN)
		, m_shape(nullptr)
	{}

	GPhyShape::~GPhyShape()
	{
		delete m_shape;
	}

	std::shared_ptr<GPhyShape> GPhyShape::CreateBox(const glm::vec3& extent)
	{
		auto shape = std::make_shared<GPhyShape>();
		shape->InitBox(extent);
		return shape;
	}
	bool GPhyShape::InitBox(const glm::vec3& ext)
	{
		m_shape_type = ShapeType::BOX;
		m_shape = new btBoxShape(g4y::convert_vec3_to_btvec3(ext * 0.5f));
		return true;
	}

	std::shared_ptr<GPhyShape> GPhyShape::CreateStaticPlane(const glm::vec3& normal, float constant)
	{
		auto shape = std::make_shared<GPhyShape>();
		shape->InitStaticPlane(normal, constant);
		return shape;
	}
	bool GPhyShape::InitStaticPlane(const glm::vec3& normal, float constant)
	{
		m_shape_type = ShapeType::MESH;
		m_shape = new btStaticPlaneShape(g4y::convert_vec3_to_btvec3(normal), constant);
		return true;
	}



	GPhyObj::GPhyObj(PhyObjType type)
		: m_type(type)
	{}

	bool GPhyObj::IsTrigger(std::shared_ptr<btCollisionObject> btcol)
	{
		int flag = btcol->getCollisionFlags();
		return ((flag & btCollisionObject::CF_NO_CONTACT_RESPONSE) != 0);
	}

	bool GPhyObj::IsTrigger(GPhyObj *col)
	{
		auto btcol = col->GetbtCol();
		return IsTrigger(btcol);
	}

	bool GPhyObj::IsTrigger()
	{
		return IsTrigger(m_col_obj);
	}

	void GPhyObj::OnCollision(const GPhyCollisionInfo& col_info)
	{
		// 判断该对象是否存在与上一次的碰撞，
		//  如果存在就调用OnCollisionStay函数
		//  如果不存在就调用OnCollisionEnter函数
		// 将碰撞对象加入到当前hash表中

		bool is_trigger = IsTrigger(col_info.objA) || IsTrigger(col_info.objB);
		bool col_obj_exist = (m_last_cols.find(col_info.target) != m_last_cols.end());
		// 获得所有组件并调用碰撞消息
		auto coms = Obj()->GetComs();
		for (const auto& c : coms) {
			if (col_obj_exist) {
				if (is_trigger)
					c->OnTriggerStay(&col_info);
				else
					c->OnCollisionStay(&col_info);
			}
			else {
				if (is_trigger)
					c->OnTriggerEnter(&col_info);
				else
					c->OnCollisionEnter(&col_info);
			}
		}
		m_cur_cols.insert(col_info.target);
	}

	void GPhyObj::OnCollisionRemove(GPhyObj* col)
	{
		for (auto c : m_last_cols) {
			if (c == col) {
				m_last_cols.erase(c);
				break;
			}
		}
	}

	void GPhyObj::OnCollisionEnd()
	{
		// 遍历上一次碰撞对象中 在这一次中是否存在
		// 如果存在，说明碰撞没有离开
		// 如果不存在， 则说明碰撞对象离开，调用OnCollisionExit函数， 并删除该对象
		// 将这一次的对象加入到上一次的hash表中，并去重复
		// 清空当前hash表
		auto coms = Obj()->GetComs();
		for (auto begin = m_last_cols.begin(); begin != m_last_cols.end(); ) {
			if (m_cur_cols.find(*begin) == m_cur_cols.end()) {
				for (const auto& c : coms) {
					if (IsTrigger() || (*begin)->IsTrigger())
						c->OnTriggerExit(*begin);
					else
						c->OnCollisionExit(*begin);
				}
				begin = m_last_cols.erase(begin);
			}
			else {
				++begin;
			}
		}
		m_last_cols.insert(m_cur_cols.begin(), m_cur_cols.end());
		m_cur_cols.clear();
	}

NS_G4Y_END

