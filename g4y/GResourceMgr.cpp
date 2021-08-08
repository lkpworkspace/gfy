#include "GResourceMgr.hpp"
#include "GWorld.hpp"
#include "GShader.hpp"
#include "GTexture.hpp"
#include "GMesh.hpp"
#include "GMeshRenderer.hpp"
#include "GOpenGLView.hpp"
#include "GMaterial.hpp"
#include "GLogger.hpp"

static const char* VS_CODE = \
"#version 330 core\n"
"layout (location = 0) in vec3 position;\n"
"layout (location = 1) in vec3 normal;\n"
"layout (location = 2) in vec2 tex_coords;\n"
"\n"
"out vec3 FragPos;\n"
"out vec3 Normal;\n"
"out vec2 TexCoords;\n"
"\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"\n"
"void main()\n"
"{\n"
"    FragPos = vec3(model * vec4(position, 1.0));\n"
"    Normal = mat3(transpose(inverse(model))) * normal;  \n"
"    TexCoords = tex_coords;\n"
"    \n"
"    gl_Position = projection * view * vec4(FragPos, 1.0);\n"
"}";

static const char* FS_CODE = \
"#version 330 core\n"
"out vec4 FragColor;\n"
"\n"
"struct Material {\n"
"    sampler2D diffuse;\n"
"    sampler2D specular;\n"
"    float shininess;\n"
"}; \n"
"\n"
"struct DirLight {\n"
"    vec3 direction;\n"
"	\n"
"    vec3 ambient;\n"
"    vec3 diffuse;\n"
"    vec3 specular;\n"
"};\n"
"\n"
"struct PointLight {\n"
"    vec3 position;\n"
"    \n"
"    float constant;\n"
"    float linear;\n"
"    float quadratic;\n"
"	\n"
"    vec3 ambient;\n"
"    vec3 diffuse;\n"
"    vec3 specular;\n"
"};\n"
"\n"
"struct SpotLight {\n"
"    vec3 position;\n"
"    vec3 direction;\n"
"    float cut_off;\n"
"    float outer_cut_off;\n"
"  \n"
"    float constant;\n"
"    float linear;\n"
"    float quadratic;\n"
"  \n"
"    vec3 ambient;\n"
"    vec3 diffuse;\n"
"    vec3 specular;       \n"
"};\n"
"\n"
"#define NR_POINT_LIGHTS 4\n"
"\n"
"in vec3 FragPos;\n"
"in vec3 Normal;\n"
"in vec2 TexCoords;\n"
"\n"
"uniform vec3         view_pos;\n"
"uniform DirLight     dir_light;\n"
"uniform PointLight   point_lights[NR_POINT_LIGHTS];\n"
"uniform SpotLight    spot_light;\n"
"uniform Material     material;\n"
"\n"
"// function prototypes\n"
"vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);\n"
"vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);\n"
"vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);\n"
"\n"
"void main()\n"
"{    \n"
"    // properties\n"
"    vec3 norm = normalize(Normal);\n"
"    vec3 viewDir = normalize(view_pos - FragPos);\n"
"\n"
"    // phase 1: directional lighting\n"
"    // vec3 result = CalcDirLight(dir_light, norm, viewDir);\n"
"    // // phase 2: point lights\n"
"    // for(int i = 0; i < NR_POINT_LIGHTS; i++)\n"
"    //     result += CalcPointLight(point_lights[i], norm, FragPos, viewDir);    \n"
"    // // phase 3: spot light\n"
"    // result += CalcSpotLight(spot_light, norm, FragPos, viewDir);    \n"
"    \n"
"    // FragColor = vec4(result, 1.0);\n"
"    FragColor = texture(material.diffuse, TexCoords);\n"
"}\n"
"\n"
"// calculates the color when using a directional light.\n"
"vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)\n"
"{\n"
"    vec3 lightDir = normalize(-light.direction);\n"
"    // diffuse shading\n"
"    float diff = max(dot(normal, lightDir), 0.0);\n"
"    // specular shading\n"
"    vec3 reflectDir = reflect(-lightDir, normal);\n"
"    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);\n"
"    // combine results\n"
"    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));\n"
"    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));\n"
"    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));\n"
"    return (ambient + diffuse + specular);\n"
"}\n"
"\n"
"// calculates the color when using a point light.\n"
"vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)\n"
"{\n"
"    vec3 lightDir = normalize(light.position - fragPos);\n"
"    // diffuse shading\n"
"    float diff = max(dot(normal, lightDir), 0.0);\n"
"    // specular shading\n"
"    vec3 reflectDir = reflect(-lightDir, normal);\n"
"    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);\n"
"    // attenuation\n"
"    float distance = length(light.position - fragPos);\n"
"    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    \n"
"    // combine results\n"
"    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));\n"
"    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));\n"
"    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));\n"
"    ambient *= attenuation;\n"
"    diffuse *= attenuation;\n"
"    specular *= attenuation;\n"
"    return (ambient + diffuse + specular);\n"
"}\n"
"\n"
"// calculates the color when using a spot light.\n"
"vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)\n"
"{\n"
"    vec3 lightDir = normalize(light.position - fragPos);\n"
"    // diffuse shading\n"
"    float diff = max(dot(normal, lightDir), 0.0);\n"
"    // specular shading\n"
"    vec3 reflectDir = reflect(-lightDir, normal);\n"
"    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);\n"
"    // attenuation\n"
"    float distance = length(light.position - fragPos);\n"
"    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    \n"
"    // spotlight intensity\n"
"    float theta = dot(lightDir, normalize(-light.direction)); \n"
"    float epsilon = light.cut_off - light.outer_cut_off;\n"
"    float intensity = clamp((theta - light.outer_cut_off) / epsilon, 0.0, 1.0);\n"
"    // combine results\n"
"    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));\n"
"    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));\n"
"    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));\n"
"    ambient *= attenuation * intensity;\n"
"    diffuse *= attenuation * intensity;\n"
"    specular *= attenuation * intensity;\n"
"    return (ambient + diffuse + specular);\n"
"}";

NS_G4Y_BEGIN

GResourceMgr::GResourceMgr()
{}

GResourceMgr::~GResourceMgr()
{}

void GResourceMgr::Init(const boost::property_tree::ptree& cfg)
{
	auto w = GWorld::Instance();
	m_resource_directory = w->GetAssetpath();

	std::string main_shader_fs;
	std::string main_shader_vs;
	namespace pt = boost::property_tree;
	pt::ptree item;

	if (cfg.empty()) {
		if (LoadShader("main_shader", VS_CODE, FS_CODE, false)) {
			GLOG(GlogLevel::GLOG_INFO, "Load main shader success");
		}
	}
	else {
		try
		{
			main_shader_fs = cfg.get<std::string>("fs");
			main_shader_vs = cfg.get<std::string>("vs");
			if (LoadShader("main_shader", w->GetAssetpath() + main_shader_vs, w->GetAssetpath() + main_shader_fs, true)) {
				std::cout << "Load main shader success" << std::endl;
			}
		}
		catch (const std::exception&)
		{
			LoadShader("main_shader", VS_CODE, FS_CODE, false);
		}
	}
}

bool GResourceMgr::LoadShader(std::string name, std::string vs, std::string fs, bool str)
{
    auto shader = std::make_shared<GShader>(vs, fs, str);
    if(shader->IsValid()){
        m_shaders[name] = shader;
        return true;
    }
    return false;
}

std::shared_ptr<GShader> GResourceMgr::Shader(std::string name)
{
    if(m_shaders.find(name) == m_shaders.end()) return nullptr;
    return m_shaders[name];
}

bool GResourceMgr::LoadTexture(const std::string& name)
{
	if (m_textures.find(name) != m_textures.end()) return false;
	auto w = GWorld::Instance();
	m_resource_directory = w->GetAssetpath();

	auto tex = std::make_shared<GTexture>();
	std::string fullpath = m_resource_directory + name;
	if (tex->LoadTextureFromFile(fullpath)) {
		m_textures[name] = tex;
		return true;
	}
	return false;
}

std::shared_ptr<GTexture> GResourceMgr::Texture(const std::string& name)
{
	if (m_textures.find(name) == m_textures.end()) return nullptr;
	return m_textures[name];
}

void GResourceMgr::ProcessNode(std::shared_ptr<GModelNodeInfo> r, aiNode *node, const aiScene *scene)
{
    // process each mesh located at the current node
    for(unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        // the node object only contains indices to index the actual objects in the scene. 
        // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        r->name = node->mName.C_Str();
        std::cout << "load node: " << node->mName.C_Str() << std::endl;
        r->SetEmpty(false);
        ProcessMesh(r, mesh, scene);
    }
    // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
    for(unsigned int i = 0; i < node->mNumChildren; i++)
    {
        auto c = std::make_shared<GModelNodeInfo>();
        r->children.push_back(c);
        ProcessNode(c, node->mChildren[i], scene);
    }
}
void GResourceMgr::ProcessMesh(std::shared_ptr<GModelNodeInfo> r,aiMesh *mesh, const aiScene *scene)
{
    auto gmesh = std::make_shared<GMesh>();

    auto& vertices = gmesh->m_vertices;
    auto& indices = gmesh->m_indices;
    std::vector<std::string> textures_name;

    std::cout << "load mesh: " << mesh->mName.C_Str() << std::endl;

    // Walk through each of the mesh's vertices
    for(unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        GVertex vertex;
        glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
        // positions
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.position = vector;
        // normals
        vector.x = mesh->mNormals[i].x;
        vector.y = mesh->mNormals[i].y;
        vector.z = mesh->mNormals[i].z;
        vertex.normal = vector;
        // texture coordinates
        if(mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
        {
            glm::vec2 vec;
            // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
            // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
            vec.x = mesh->mTextureCoords[0][i].x; 
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.tex_coords = vec;
        }
        else
            vertex.tex_coords = glm::vec2(0.0f, 0.0f);
        // tangent
        vector.x = mesh->mTangents[i].x;
        vector.y = mesh->mTangents[i].y;
        vector.z = mesh->mTangents[i].z;
        vertex.tangent = vector;
        // bitangent
        vector.x = mesh->mBitangents[i].x;
        vector.y = mesh->mBitangents[i].y;
        vector.z = mesh->mBitangents[i].z;
        vertex.bitangent = vector;
        vertices.push_back(vertex);
    }
    // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
    for(unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        // retrieve all indices of the face and store them in the indices vector
        for(unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }
    // process materials
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];    
    // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
    // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
    // Same applies to other texture as the following list summarizes:
    // diffuse: texture_diffuseN
    // specular: texture_specularN
    // normal: texture_normalN

    // 1. diffuse maps
    auto diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "material.diffuse");
    textures_name.insert(textures_name.end(), diffuseMaps.begin(), diffuseMaps.end());
    // 2. specular maps
    auto specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, "material.specular");
    textures_name.insert(textures_name.end(), specularMaps.begin(), specularMaps.end());
    // 3. normal maps
    auto normalMaps = LoadMaterialTextures(material, aiTextureType_HEIGHT, "material.normal");
    textures_name.insert(textures_name.end(), normalMaps.begin(), normalMaps.end());
    // 4. height maps
    auto heightMaps = LoadMaterialTextures(material, aiTextureType_AMBIENT, "material.height");
    textures_name.insert(textures_name.end(), heightMaps.begin(), heightMaps.end());

    gmesh->SetupMesh();
    r->info.push_back(std::make_pair(gmesh, textures_name));
}

std::vector<std::string> GResourceMgr::LoadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName)
{
    std::vector<std::string> textures_name;

    for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);

        if(m_textures.find(str.C_Str()) == m_textures.end()){
            auto texture = std::make_shared<GTexture>();

            texture->LoadTextureFromFile(m_cur_model_path + '/' + str.C_Str());
            texture->m_type = typeName;
            texture->m_path = str.C_Str();
            m_textures[str.C_Str()] = texture;
            textures_name.push_back(str.C_Str());
        }else{
            textures_name.push_back(str.C_Str());
        }
    }
    return textures_name;
}

bool GResourceMgr::LoadModel(std::string const& filepath)
{
    if(m_models.find(filepath) != m_models.end()) return false;

    std::string abspath = m_resource_directory + filepath;

    // read file via ASSIMP
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(abspath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    // check for errors
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
    {
        std::cout << "[ERROR] ASSIMP:: " << importer.GetErrorString() << std::endl;
        return false;
    }
    
    auto root = std::make_shared<GModelNodeInfo>();
    m_models[filepath] = root;
    m_cur_model_path = abspath.substr(0, abspath.find_last_of('/'));

    // process ASSIMP's root node recursively
    ProcessNode(root, scene->mRootNode, scene);
    return true;
}

void GResourceMgr::InstantiateModel(std::shared_ptr<GObj> p, std::shared_ptr<GModelNodeInfo> node)
{
    std::shared_ptr<GObj> obj = nullptr;
    if(node->Empty()){
        obj = std::make_shared<GObj>();
        obj->SetObjName(node->name);
        obj->AddDefaultComs();
        p->AddChild(obj);
    }else{
        for(int i = 0; i < node->info.size(); ++i){
            obj = std::make_shared<GObj>();
            obj->SetObjName(node->name);
            obj->AddDefaultComs();

            auto render = std::make_shared<GMeshRenderer>();
            obj->AddCom(render);

            p->AddChild(obj);

            render->m_mesh = node->info[i].first;
            for(int j = 0; j < node->info[i].second.size(); ++j){
                render->m_materials.emplace_back(GWorld::Instance()->GLView()->GetShader(), m_textures[node->info[i].second[j]]);
            }
        }
    }

    for(int i = 0; i < node->children.size(); ++i){
        InstantiateModel(obj, node->children[i]);
    }
}

std::shared_ptr<GObj> GResourceMgr::Instantiate(std::string name)
{
    if(m_models.find(name) == m_models.end()) return nullptr;
    auto info = m_models[name];

    auto obj = std::make_shared<GObj>();
    obj->AddDefaultComs();

    InstantiateModel(obj, info);
    return obj;
}

std::shared_ptr<GModelNodeInfo> FindChildNodeInfo(std::shared_ptr<GModelNodeInfo> info, std::string node_name)
{
    if(info == nullptr) return nullptr;

    if(info->name == node_name){
        return info;
    }
    for(int i = 0; i < info->children.size(); ++i){
        auto ret = FindChildNodeInfo(info->children[i], node_name);
        if(ret != nullptr) return ret;
    }
    return nullptr;
}

std::shared_ptr<GObj> GResourceMgr::CloneChildNode(std::string model_name, std::string node_name)
{
    if(m_models.find(model_name) == m_models.end()) return nullptr;
    auto root_info = m_models[model_name];
    auto node_info = FindChildNodeInfo(root_info, node_name);
    std::cout << "get " << node_info->name << std::endl;
    if(node_info == nullptr) return nullptr;
    // clone node obj
    auto obj = std::make_shared<GObj>();
    obj->SetObjName(node_info->name);
    obj->AddDefaultComs();

    auto render = std::make_shared<GMeshRenderer>();
    obj->AddCom(render);

    render->m_mesh = node_info->info[0].first;
    for(int j = 0; j < node_info->info[0].second.size(); ++j){
        render->m_materials.emplace_back(GWorld::Instance()->GLView()->GetShader(), m_textures[node_info->info[0].second[j]]);
    }

    return obj;
}



void GResourceMgr::Test()
{
}


NS_G4Y_END



#if 0
//"#version 330 core\n"
//"layout (location = 0) in vec3 aPos;\n"
//"layout (location = 1) in vec3 aNormal;\n"
//"layout (location = 2) in vec2 aTexCoords;\n"
//"out vec2 TexCoords;\n"
//"\n"
//"uniform mat4 model;\n"
//"uniform mat4 view;\n"
//"uniform mat4 projection;\n"
//"\n"
//"void main()\n"
//"{\n"
//"    TexCoords = aTexCoords;\n"
//"    gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
//"}";

//"#version 330 core\n"
//"out vec4 FragColor;\n"
//"in vec2 TexCoords;\n"
//"uniform sampler2D texture_diffuse1;\n"
//"void main()\n"
//"{\n"
//"   FragColor = texture(texture_diffuse1, TexCoords);\n"
//"}";




#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 tex_coords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	FragPos = vec3(model * vec4(position, 1.0));
	Normal = mat3(transpose(inverse(model))) * normal;
	TexCoords = tex_coords;

	gl_Position = projection * view * vec4(FragPos, 1.0);
}




#version 330 core
out vec4 FragColor;

struct Material {
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};

struct DirLight {
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct PointLight {
	vec3 position;

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct SpotLight {
	vec3 position;
	vec3 direction;
	float cut_off;
	float outer_cut_off;

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

#define NR_POINT_LIGHTS 4

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3         view_pos;
uniform DirLight     dir_light;
uniform PointLight   point_lights[NR_POINT_LIGHTS];
uniform SpotLight    spot_light;
uniform Material     material;

// function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
	// properties
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(view_pos - FragPos);

	// phase 1: directional lighting
	// vec3 result = CalcDirLight(dir_light, norm, viewDir);
	// // phase 2: point lights
	// for(int i = 0; i < NR_POINT_LIGHTS; i++)
	//     result += CalcPointLight(point_lights[i], norm, FragPos, viewDir);    
	// // phase 3: spot light
	// result += CalcSpotLight(spot_light, norm, FragPos, viewDir);    

	// FragColor = vec4(result, 1.0);
	FragColor = texture(material.diffuse, TexCoords);
}

// calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(-light.direction);
	// diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);
	// specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	// combine results
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
	vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
	return (ambient + diffuse + specular);
}

// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - fragPos);
	// diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);
	// specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	// attenuation
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	// combine results
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
	vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
	return (ambient + diffuse + specular);
}

// calculates the color when using a spot light.
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - fragPos);
	// diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);
	// specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	// attenuation
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	// spotlight intensity
	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.cut_off - light.outer_cut_off;
	float intensity = clamp((theta - light.outer_cut_off) / epsilon, 0.0, 1.0);
	// combine results
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
	vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
	ambient *= attenuation * intensity;
	diffuse *= attenuation * intensity;
	specular *= attenuation * intensity;
	return (ambient + diffuse + specular);
}

#endif