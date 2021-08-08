#include "GBoxCollider.h"
#include <btBulletDynamicsCommon.h>
#include "GWorld.h"
#include "GPhyWorld.h"
#include "GTransform.h"
#include "GRigibody.h"
#include "GShader.hpp"
#include "GObj.h"
#include "GCamera.h"
#include <GL/glew.h>

static const char* vs_code = \
"#version 330 core\n"
"layout (location = 0) in vec3 pos;\n"
"uniform vec4 color;\n"
"\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"\n"
"out vec4 vcolor;\n"
"void main()\n"
"{\n"
"    gl_Position = projection * view * model * vec4(pos, 1.0);\n"
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

static unsigned int indeies[] = {
	// bottom
	0, 1, 1, 2,
	2, 3, 3, 0,
	// up
	4, 5, 5, 6,
	6, 7, 7, 4,
	
	0, 4, 1, 5,
	2, 6, 3, 7,
};

/*
         7*******4
		**      **
       * *     * *
      *  *    *  *
     6*******5   *
     *   *   *   *
	 *   3*******0
	 *	*	 *  *
	 * *	 * *
	 *       *
     2*******1	 		 
*/
static float vertices[] = {
	 0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f,  0.5f,
	-0.5f, -0.5f,  0.5f,
	-0.5f, -0.5f, -0.5f,
	 0.5f,  0.5f, -0.5f,
	 0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f, -0.5f,
};

GBoxCollider::GBoxCollider() 
	: m_box_half_extents(.5, .5, .5)
	, m_shape_changed(false)

{
	UpdateVertices();
}

GBoxCollider::GBoxCollider(glm::vec3 box_half_extents) 
	: m_box_half_extents(box_half_extents.x, box_half_extents.y, box_half_extents.z)
	, m_shape_changed(false)
{
	UpdateVertices();
}

GBoxCollider::~GBoxCollider()
{
	DelShader();
}

void GBoxCollider::UpdateVertices()
{
	int flag[] = {
			 1, -1, -1,
			 1, -1,  1,
			-1, -1,  1,
			-1, -1, -1,
			 1,  1, -1,
			 1,  1,  1,
			-1,  1,  1,
			-1,  1, -1
	};
	for (int i = 0; i < 24; i += 3) {
		vertices[i] = m_box_half_extents.x * flag[i];
		vertices[i + 1] = m_box_half_extents.y * flag[i + 1];
		vertices[i + 2] = m_box_half_extents.z * flag[i + 2];
	}
}

void GBoxCollider::SetBoxHalfExtents(glm::vec3 v)
{
	m_shape_changed = true;
	m_box_half_extents = v;
	auto shape = std::static_pointer_cast<btCollisionShape>(std::make_shared<btBoxShape>(btVector3(m_box_half_extents.x, m_box_half_extents.y, m_box_half_extents.z)));
	m_shape = shape;
	if(m_col_obj != nullptr) m_col_obj->setCollisionShape(m_shape.get());
}

glm::vec3 GBoxCollider::GetBoxHalfExtents()
{
   return m_box_half_extents;
}

void GBoxCollider::Init()
{
   m_transform = GetCom<GTransform>();
   m_pos_connection = m_transform.lock()->connect((GTransform::position_changed_sig_t::slot_type)boost::bind(&GBoxCollider::SetPositionXYZ, this, _1, _2, _3));
   m_rot_connection = m_transform.lock()->connect((GTransform::rotation_changed_sig_t::slot_type)boost::bind(&GBoxCollider::SetRotationXYZW, this, _1, _2, _3, _4));

   m_phy_world = GWorld::Instance()->PhyWorld();
   m_shape     = std::static_pointer_cast<btCollisionShape>(std::make_shared<btBoxShape>(btVector3(m_box_half_extents.x, m_box_half_extents.y, m_box_half_extents.z)));

   InitShader();

   auto com = GetCom<GRigibody>();
   if (com == nullptr) {
	   m_has_rigibody = false;
	   
	   m_col_obj = std::make_shared<btCollisionObject>();
	   m_col_obj->setUserPointer(this);
	   m_col_obj->setCollisionShape(m_shape.get());
	   SetPostion(m_transform.lock()->Position());
	   m_phy_world.lock()->AddCollisionObj(m_col_obj);
   }
   else {
	   m_has_rigibody = true;
   }
}

void GBoxCollider::OnRender()
{
	// »æÖÆÅö×²¿ò
	DrawShape();
}

void GBoxCollider::OnDestroy()
{
	auto com = GetCom<GRigibody>();
	if (com == nullptr) {
		m_phy_world.lock()->DelCollisionObj(m_col_obj);
	}
}

void GBoxCollider::InitShader()
{
	m_transform = GetCom<GTransform>();
	m_camera = Obj()->FindWithTag("GCamera")->GetCom<GCamera>();
	m_shader = std::make_shared<GShader>(vs_code, fs_code, false);

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) * sizeof(float), &vertices[0], GL_STREAM_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indeies) * sizeof(unsigned int), &indeies[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	glBindVertexArray(0);
}

void GBoxCollider::DrawShape()
{
	glm::mat4 P = m_camera.lock()->Projection();
	glm::mat4 V = m_camera.lock()->View();
	glm::mat4 M = m_transform.lock()->ToMat4();

	m_shader->Use();
	m_shader->SetUniform("color", glm::vec4(0.0, 1.0, 0.0, 1.0));
	m_shader->SetUniform("projection", P);
	m_shader->SetUniform("view", V);
	m_shader->SetUniform("model", M);
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBindVertexArray(VAO);
	if (m_shape_changed) {
		m_shape_changed = false;
		// calc shape
		UpdateVertices();
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices) * sizeof(float), &vertices[0]);
	}
	glDrawElements(GL_LINES, (GLsizei)sizeof(indeies), GL_UNSIGNED_INT, 0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glBindVertexArray(0);
}

void GBoxCollider::DelShader()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}