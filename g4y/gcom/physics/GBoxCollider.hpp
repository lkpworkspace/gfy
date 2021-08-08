#ifndef __GBOXCOLLIDER_H__
#define __GBOXCOLLIDER_H__
#include "GCollider.hpp"

class GTransform;
class GPhyWorld;
class GCamera;
class GShader;
class G4Y_DLL GBoxCollider : public GCollider
{
public:
    GBoxCollider();
    GBoxCollider(glm::vec3);
	virtual ~GBoxCollider();

    virtual void Init() override;

	virtual void OnRender() override;

    virtual void OnDestroy() override;

    void SetBoxHalfExtents(glm::vec3);
    glm::vec3 GetBoxHalfExtents();

    virtual std::string ColliderName() { return "GBoxCollider"; }

// private:
	bool                                        m_has_rigibody;
    std::weak_ptr<GPhyWorld>                    m_phy_world;
    std::weak_ptr<GTransform>                   m_transform;
    glm::vec3                                   m_box_half_extents;

private:
	void InitShader();
	void DrawShape();
	void DelShader();
	void UpdateVertices();

	unsigned int             VAO, VBO, EBO;
	bool                     m_shape_changed;
	std::shared_ptr<GShader> m_shader;
	std::weak_ptr<GCamera>   m_camera;
};

#endif