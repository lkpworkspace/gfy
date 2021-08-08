#ifndef __GTEXTURE_HPP__
#define __GTEXTURE_HPP__
#include <string>
#include <GL/glew.h>

#include "GConfig.hpp"

NS_G4Y_BEGIN

class GTexture
{
public:
    GTexture();

    bool LoadTextureFromFile(std::string);

    bool              m_vaild;
    unsigned int      m_id;
    int               m_width;
    int               m_height;
	GLenum            m_format;
    std::string       m_type;
    std::string       m_path;
};

NS_G4Y_END

#endif