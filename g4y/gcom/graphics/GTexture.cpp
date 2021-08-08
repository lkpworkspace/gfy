#include <iostream>
#include <GL/glew.h>

#include "GTexture.hpp"
#include "GResourceMgr.hpp"
#include "GObj.hpp"
#include "GScene.hpp"
#include "GCamera.hpp"
#include "GTransform.hpp"
#include "GOpenGLView.hpp"
#include "GWorld.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

NS_G4Y_BEGIN

GTexture::GTexture() :
    m_vaild(false)
{

}

bool GTexture::LoadTextureFromFile(std::string filepath)
{
	if (m_vaild) return false;

    int nrComponents;
    unsigned char *data = stbi_load(filepath.c_str(), &m_width, &m_height, &nrComponents, 0);
    if (data)
    {
		if (nrComponents == 1)
			m_format = GL_RED;
		else if (nrComponents == 3)
			m_format = GL_RGB;
		else if (nrComponents == 4)
			m_format = GL_RGBA;

		GLint last_texture = 0;
		glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
		glGenTextures(1, &m_id);
		glBindTexture(GL_TEXTURE_2D, m_id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, m_format, GL_UNSIGNED_BYTE, data);
		glBindTexture(GL_TEXTURE_2D, last_texture);

		m_vaild = true;
		m_path = filepath;

		stbi_image_free(data);
    }
    else
    {
        std::cout << "[ERROR] Texture failed to load at path: " << filepath << std::endl;
        stbi_image_free(data);
		m_vaild = false;
    }

    return m_vaild;
}

NS_G4Y_END