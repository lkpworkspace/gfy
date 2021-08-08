#ifndef __GPhyDebugDraw_H__
#define __GPhyDebugDraw_H__
#include <vector>
#include <btBulletDynamicsCommon.h>
#include <glm/glm.hpp>

#include "GConfig.hpp"

NS_G4Y_BEGIN

class GShader;
class GPhyDebugDraw : public btIDebugDraw
{
public:
	GPhyDebugDraw();
	~GPhyDebugDraw();

	void Init();
	
	void Draw(glm::mat4& mvp);

	// override function
	virtual void	drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override;
	virtual void	drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) override;
	virtual void	reportErrorWarning(const char* warningString) override;
	virtual void	draw3dText(const btVector3& location, const char* textString) override;
	virtual void	setDebugMode(int debugMode) override;
	virtual int		getDebugMode() const override;

private:

	struct V3F_V4F {
		glm::vec3 position;
		glm::vec4 color;
	};

	std::vector<V3F_V4F>      m_vertices;

	int m_debug_mode;

	unsigned int             VAO, VBO;

	bool                     m_dirty;

	std::shared_ptr<GShader> m_shader;
};

NS_G4Y_END

#endif