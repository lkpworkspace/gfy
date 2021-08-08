#ifndef __GGRID_H__
#define __GGRID_H__
#include "GCom.hpp"

NS_G4Y_BEGIN

class GShader;
class GCamera;
class GTransform;
class G4Y_DLL GGrid : public GCom
{
    G_COM
public:
    // [begin, end]
    GGrid(int begin, int end, int step);
    virtual ~GGrid();

    virtual void Init() override;

    virtual void Update() override;

    virtual void OnRender() override;

    virtual void OnDestroy() override;

private:
    void CreateGridArr();

    // VAO: 顶点数组对象
    // VBO: 顶点缓冲对象
    //  - 它会在GPU内存（通常被称为显存）中储存大量顶点
    //  - GL_ARRAY_BUFFER
    // EBO: 索引缓冲对象
    unsigned int VAO, VBO;
    std::shared_ptr<GShader> m_shader;
    std::weak_ptr<GCamera>   m_camera;
	std::weak_ptr<GTransform> m_camera_trans;
    int m_step;
    int m_begin;
    int m_end;
    std::shared_ptr<float[]> m_arr;
    int m_cnt;
};

NS_G4Y_END

#endif