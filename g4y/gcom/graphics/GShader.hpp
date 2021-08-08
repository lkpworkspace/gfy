#ifndef __GSHADER_HPP__
#define __GSHADER_HPP__
#include <string>
#include <glm/glm.hpp>

#include "GConfig.hpp"

NS_G4Y_BEGIN

class GShader
{
public:
    GShader(const std::string &vs, const std::string &fs, bool filepath = true);
    virtual ~GShader();

    bool IsValid() { return m_valid; }
    
    void Use();
    unsigned int ID() { return m_id; }
    void SetUniform(const std::string &name, bool value) const;
    void SetUniform(const std::string &name, int value) const;
    void SetUniform(const std::string &name, float value) const;

    void SetUniform(const std::string &name, glm::vec2& value) const;
    void SetUniform(const std::string &name, float x, float y) const;
    void SetUniform(const std::string &name, glm::vec3& value) const;
    void SetUniform(const std::string &name, float x, float y, float z) const;
    void SetUniform(const std::string &name, glm::vec4& value) const;
    void SetUniform(const std::string &name, float x, float y, float z, float w) const;

    void SetUniform(const std::string &name, glm::mat2& value) const;
    void SetUniform(const std::string &name, glm::mat3& value) const;
    void SetUniform(const std::string &name, glm::mat4& value) const;
private:
    void CompileShader(const char* vs_code, const char* fs_code);
    void CheckCompileErrors(unsigned int shader, std::string type);

    unsigned int m_id;
    bool         m_valid;
};

NS_G4Y_END

#endif