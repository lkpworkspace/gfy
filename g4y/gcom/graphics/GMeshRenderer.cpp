#include <iostream>
#include <glm/glm.hpp>
#include <GL/glew.h>

#include "GMeshRenderer.hpp"
#include "GMaterial.hpp"
#include "GCamera.hpp"
#include "GTransform.hpp"
#include "GTexture.hpp"
#include "GMesh.hpp"
#include "GShader.hpp"

NS_G4Y_BEGIN

void GMeshRenderer::Start()
{
    m_camera = Obj()->FindWithTag("GCamera")->GetCom<GCamera>();
    m_transform = GetCom<GTransform>();
}

void GMeshRenderer::OnRender()
{
    // bind appropriate textures
    unsigned int diffuseNr  = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr   = 1;
    unsigned int heightNr   = 1;
    for(int i = 0; i < m_materials.size(); i++)
    {
        if(m_materials[i].shader.expired() || m_materials[i].texture.expired()){
            std::cout << "no shader" << std::endl;
            continue;
        }
        auto shader = m_materials[i].shader.lock();
        auto texture = m_materials[i].texture.lock();

        shader->Use();

        glm::mat4 P = m_camera.lock()->Projection();
        glm::mat4 V = m_camera.lock()->View();
        glm::mat4 M = m_transform.lock()->ToMat4();
        shader->SetUniform("projection", P);
        shader->SetUniform("view", V);
        shader->SetUniform("model", M);

        glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
        // retrieve texture number (the N in diffuse_textureN)
        std::string number;
        std::string name = texture->m_type;
        if(name == "material.diffuse")
            number = std::to_string(diffuseNr++);
        else if(name == "material.specular")
            number = std::to_string(specularNr++); // transfer unsigned int to stream
        else if(name == "material.normal")
            number = std::to_string(normalNr++); // transfer unsigned int to stream
        else if(name == "material.height")
            number = std::to_string(heightNr++); // transfer unsigned int to stream

        // now set the sampler to the correct texture unit
        if(i == 0){
            shader->SetUniform(name.c_str(), i);
        }else{
            shader->SetUniform((name + number).c_str(), i);
        }
        //glUniform1i(glGetUniformLocation(shader->ID(), (name + number).c_str()), i);
        // and finally bind the texture
        glBindTexture(GL_TEXTURE_2D, texture->m_id);
    }
    
    glBindVertexArray(m_mesh.lock()->VAO);
    glDrawElements(GL_TRIANGLES, (GLsizei)m_mesh.lock()->m_indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // always good practice to set everything back to defaults once configured.
    glActiveTexture(GL_TEXTURE0);
}

void GMeshRenderer::OnDestroy()
{

}

NS_G4Y_END