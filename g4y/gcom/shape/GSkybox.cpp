#include "GSkybox.hpp"
#include "GShader.hpp"
#include "GObj.hpp"
#include "GCamera.hpp"
#include "GTransform.hpp"

#include <iostream>
#include <GL/glew.h>

#include "stb_image.h"

static const char* vs_code = \
"#version 330 core\n"
"layout (location = 0) in vec3 pos;\n"
"\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"\n"
"out vec3 TexCoords;\n"
"void main()\n"
"{\n"
"    TexCoords = pos;\n"
"    gl_Position = projection * view * vec4(pos, 1.0);\n"
"}";

static const char* fs_code = \
"#version 330 core\n"
"in vec3 TexCoords;\n"
"\n"
"uniform samplerCube skybox;\n"
"\n"
"out vec4 FragColor;\n"
"\n"
"void main()\n"
"{\n"
"   FragColor = texture(skybox, TexCoords);\n"
"}";

static float skybox_vertices[] = {       
    -500.0f,  500.0f, -500.0f,
    -500.0f, -500.0f, -500.0f,
    500.0f, -500.0f, -500.0f,
    500.0f, -500.0f, -500.0f,
    500.0f,  500.0f, -500.0f,
    -500.0f,  500.0f, -500.0f,

    -500.0f, -500.0f,  500.0f,
    -500.0f, -500.0f, -500.0f,
    -500.0f,  500.0f, -500.0f,
    -500.0f,  500.0f, -500.0f,
    -500.0f,  500.0f,  500.0f,
    -500.0f, -500.0f,  500.0f,

    500.0f, -500.0f, -500.0f,
    500.0f, -500.0f,  500.0f,
    500.0f,  500.0f,  500.0f,
    500.0f,  500.0f,  500.0f,
    500.0f,  500.0f, -500.0f,
    500.0f, -500.0f, -500.0f,

    -500.0f, -500.0f,  500.0f,
    -500.0f,  500.0f,  500.0f,
    500.0f,  500.0f,  500.0f,
    500.0f,  500.0f,  500.0f,
    500.0f, -500.0f,  500.0f,
    -500.0f, -500.0f,  500.0f,

    -500.0f,  500.0f, -500.0f,
    500.0f,  500.0f, -500.0f,
    500.0f,  500.0f,  500.0f,
    500.0f,  500.0f,  500.0f,
    -500.0f,  500.0f,  500.0f,
    -500.0f,  500.0f, -500.0f,

    -500.0f, -500.0f, -500.0f,
    -500.0f, -500.0f,  500.0f,
    500.0f, -500.0f, -500.0f,
    500.0f, -500.0f, -500.0f,
    -500.0f, -500.0f,  500.0f,
    500.0f, -500.0f,  500.0f
};

NS_G4Y_BEGIN

void GSkybox::LoadCubemap()
{
    unsigned int texture_id;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);

    int width, height, nr_channels;
    for (unsigned int i = 0; i < m_tex_path.size(); i++)
    {
        unsigned char *data = stbi_load(m_tex_path[i].c_str(), &width, &height, &nr_channels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
                         0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << m_tex_path[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void GSkybox::Start()
{
    m_transform = GetCom<GTransform>();
    m_camera = Obj()->FindWithTag("GCamera")->GetCom<GCamera>();
    m_shader = std::make_shared<GShader>(vs_code, fs_code, false);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_vertices), &skybox_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    LoadCubemap();
}

void GSkybox::OnRender() 
{
    glm::mat4 P = m_camera.lock()->Projection();
    glm::mat4 V = glm::mat4(glm::mat3(m_camera.lock()->View()));
    //glm::mat4 V = m_camera.lock()->View();
   
    glDepthFunc(GL_LEQUAL);
    m_shader->Use();
    m_shader->SetUniform("skybox", 0);
    m_shader->SetUniform("projection", P);
    m_shader->SetUniform("view", V);

    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);
}

void GSkybox::OnDestroy() 
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

NS_G4Y_END