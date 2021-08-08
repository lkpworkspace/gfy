#include <iostream>
#include <GL/glew.h>

#include "GShader.hpp"
#include "GPhyDebugDraw.hpp"

static const char* vs_code = \
"#version 330 core\n"
"layout (location = 0) in vec3 pos;\n"
"layout (location = 1) in vec4 color;\n"
"\n"
"uniform mat4 MVP;\n"
"\n"
"out vec4 vcolor;\n"
"void main()\n"
"{\n"
"    gl_Position = MVP * vec4(pos, 1.0);\n"
"    vcolor = color;\n"
"}";

static const char* fs_code = \
"#version 330 core\n"
"in vec4 vcolor;\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vcolor;\n"
"}";

NS_G4Y_BEGIN

glm::vec3 convert_btvec3_to_vec3(const btVector3& btVec3);
btVector3 convert_vec3_to_btvec3(const glm::vec3& vec3);

GPhyDebugDraw::GPhyDebugDraw()
	: m_debug_mode((int)(DBG_DrawWireframe | DBG_DrawConstraints | DBG_DrawConstraintLimits))
	, m_dirty(true)
{
}

GPhyDebugDraw::~GPhyDebugDraw()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}

void GPhyDebugDraw::Init()
{
	m_shader = std::make_shared<NS_G4Y::GShader>(vs_code, fs_code, false);

	m_vertices.emplace_back();

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices) * sizeof(float), &m_vertices[0], GL_STREAM_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(V3F_V4F), (GLvoid *)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(V3F_V4F), (GLvoid *)offsetof(V3F_V4F, color));

	glBindVertexArray(0);
}

void GPhyDebugDraw::Draw(glm::mat4& mvp)
{
	m_shader->Use();
	m_shader->SetUniform("MVP", mvp);

	glBindVertexArray(VAO);

	if (m_dirty)
	{
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(V3F_V4F) * m_vertices.size(), &m_vertices[0], GL_STREAM_DRAW);
		m_dirty = false;
	}

	glDrawArrays(GL_LINES, 0, m_vertices.size());
	
	glBindVertexArray(0);

	m_vertices.clear();
}

void	GPhyDebugDraw::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{
	//std::cout << "draw from: " << from.x() << "," << from.y() << "," << from.z() << std::endl;
	//std::cout << "draw to: " << to.x() << "," << to.y() << "," << to.z() << std::endl;

	V3F_V4F tmp;
	tmp.position = convert_btvec3_to_vec3(from);
	tmp.color = glm::vec4(color.x(), color.y(), color.z(), 1.0f);
	m_vertices.push_back(tmp);
	tmp.position = convert_btvec3_to_vec3(to);
	tmp.color = glm::vec4(color.x(), color.y(), color.z(), 1.0f);
	m_vertices.push_back(tmp);

	m_dirty = true;
}
void	GPhyDebugDraw::drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
{
	drawLine(PointOnB, PointOnB + normalOnB * distance, color);
}
void	GPhyDebugDraw::reportErrorWarning(const char* warningString)
{
	std::cout << "[debug draw] " << warningString << std::endl;
}
void	GPhyDebugDraw::draw3dText(const btVector3& location, const char* textString)
{

}
void	GPhyDebugDraw::setDebugMode(int debugMode)
{
	m_debug_mode = debugMode;
}
int		GPhyDebugDraw::getDebugMode() const
{
	return m_debug_mode;
}

NS_G4Y_END