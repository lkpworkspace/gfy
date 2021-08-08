#ifndef __GMESH_HPP__
#define __GMESH_HPP__
#include <vector>
#include <glm/glm.hpp>

#include "GConfig.hpp"

NS_G4Y_BEGIN

struct GVertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 tex_coords;
    glm::vec3 tangent;
    glm::vec3 bitangent;
};

class GMesh
{
public:
    GMesh(){}
    ~GMesh();
    GMesh(std::vector<GVertex> vertices, std::vector<unsigned int> indices);

    GMesh(const GMesh& o){
        operator=(o);
    }

    GMesh& operator=(const GMesh& o){
        VAO = o.VAO;
        VBO = o.VBO;
        EBO = o.EBO;
        m_indices = o.m_indices;
        m_vertices = o.m_vertices;
        return *this;
    }
    
    unsigned int              VAO;
    std::vector<unsigned int> m_indices;
    std::vector<GVertex>      m_vertices;
    void SetupMesh();
private:

    unsigned int              VBO, EBO;
};

NS_G4Y_END

#endif