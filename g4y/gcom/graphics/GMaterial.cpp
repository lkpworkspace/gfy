#include "GMaterial.hpp"
#include "GCommon.hpp"

NS_G4Y_BEGIN

GMaterial::GMaterial(std::shared_ptr<GShader> s, std::shared_ptr<GTexture> t)
{
    shader = s;
    texture = t;
}

NS_G4Y_END