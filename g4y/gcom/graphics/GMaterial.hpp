#ifndef __GMATERIAL_HPP__
#define __GMATERIAL_HPP__
#include <memory>

#include "GConfig.hpp"

NS_G4Y_BEGIN

class GShader;
class GTexture;
class GMaterial
{
public:
    GMaterial(){}
    GMaterial(std::shared_ptr<GShader>, std::shared_ptr<GTexture>);

    GMaterial(const GMaterial& o){
        operator=(o);
    }

    GMaterial& operator=(const GMaterial& o){
        shader = o.shader.lock();
        texture = o.texture.lock();
        return *this;
    }

    std::weak_ptr<GShader>    shader;
    std::weak_ptr<GTexture>   texture;
};

NS_G4Y_END

#endif