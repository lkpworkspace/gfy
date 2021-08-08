#ifndef __GRESOURCEMGR_H__
#define __GRESOURCEMGR_H__
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <boost/property_tree/ptree.hpp>

#include "GConfig.hpp"
#include "GCommon.hpp"

NS_G4Y_BEGIN

class GMesh;
class GTexture;
struct G4Y_DLL GModelNodeInfo
{
    GModelNodeInfo() :
        empty(true)
    {}
    
    typedef std::vector<std::pair<std::shared_ptr<GMesh>, std::vector<std::string>>> meshinfo_t;
    bool Empty() { return empty; }
    void SetEmpty(bool b) { empty = b; }

    bool                                         empty;
    std::string                                  name;
    meshinfo_t                                   info;
    std::vector<std::shared_ptr<GModelNodeInfo>> children;
};

class GObj;
class GShader;
using boost::property_tree::ptree;
class G4Y_DLL GResourceMgr
{
public:
    GResourceMgr();
    ~GResourceMgr();

	void Init(const ptree& cfg);

    /* shader管理 */
    bool LoadShader(std::string name, std::string vs, std::string fs, bool str = true);
    std::shared_ptr<GShader> Shader(std::string);

    /* 模型管理 */
    bool LoadModel(std::string const& name);
    std::shared_ptr<GObj> Instantiate(std::string name);
    std::shared_ptr<GObj> CloneChildNode(std::string model_name, std::string node_name);    

    /* texture管理 */
	bool LoadTexture(const std::string& name);
	std::shared_ptr<GTexture> Texture(const std::string& name);

    static void Test();

private:
    void InstantiateModel(std::shared_ptr<GObj> p, std::shared_ptr<GModelNodeInfo> node);

    void ProcessNode(std::shared_ptr<GModelNodeInfo> r, aiNode *node, const aiScene *scene);
    void ProcessMesh(std::shared_ptr<GModelNodeInfo> r, aiMesh *mesh, const aiScene *scene);
    std::vector<std::string> LoadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);

    std::string                                                       m_resource_directory;
    std::string                                                       m_cur_model_path;

    std::unordered_map<std::string, std::shared_ptr<GModelNodeInfo>>  m_models;
    std::unordered_map<std::string, std::shared_ptr<GShader>>         m_shaders;
    std::unordered_map<std::string, std::shared_ptr<GTexture>>        m_textures;
};

NS_G4Y_END

#endif