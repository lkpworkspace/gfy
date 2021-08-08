#ifndef __GOBJ_H__
#define __GOBJ_H__
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <memory>
#include <typeinfo>

#include <boost/python.hpp>
namespace boostpy = boost::python;

#include "GConfig.hpp"

NS_G4Y_BEGIN

class GCom;
class G4Y_DLL GObj : public std::enable_shared_from_this<GObj>
{
    friend class GScene;
public:
    GObj();
    virtual ~GObj();

    void SetActive(bool b = true){ m_active = b; }
    bool IsActive(){ return m_active; }

    bool AddChild(std::shared_ptr<GObj> obj);

    template<typename T>
    bool AddCom(std::shared_ptr<T> com){
        auto c = std::dynamic_pointer_cast<GCom>(com);
        if(c) return AddCom(c);
        return false;
    }
	bool AddCom(boostpy::object c);
    bool AddCom(std::shared_ptr<GCom> com);

	template<typename T>
	std::shared_ptr<T> GetCom() {
		auto com = GetCom(typeid(T).name());
		if (com == nullptr) return nullptr;
		return std::dynamic_pointer_cast<T>(com);
	}
    
    std::vector<std::shared_ptr<GCom>> GetComs();
    
    void SetTag(std::string tag);
    std::string Tag() { return m_tag; }

    void SetObjName(std::string objname){ m_objname = objname; }
    std::string ObjName() { return m_objname; }

    std::shared_ptr<GObj> Parent() { return m_parent.expired() ? nullptr : m_parent.lock(); }
    std::vector<std::shared_ptr<GObj>> Children();

    void AddDefaultComs();

    std::string& UUID() { return m_uuid; }

    static std::shared_ptr<GObj> FindWithTag(std::string tag);
    static std::vector<std::shared_ptr<GObj>> FindObjsWithTag(std::string tag);

    static void Destroy(std::shared_ptr<GObj>);
    static void Destroy(std::shared_ptr<GCom>);
    template<typename T>
    static void Destroy(std::shared_ptr<T> com){
        auto c = std::dynamic_pointer_cast<GCom>(com);
        if(c) Destroy(c);
    }

protected:

    void Init();

    void Start();

    void UpdateComAndChildren();

    void UpdateLate();

    void UpdateRender();

private:
	std::shared_ptr<GCom> GetCom(std::string com_name);
    void DelCom(std::shared_ptr<GCom>);
    void DelChild(std::shared_ptr<GObj> obj);

    bool         m_active;

    /* 被标记将要销毁 */
    bool         m_destroy_flag;

    /* 是否已经被销毁,防止被重复删除 */
    bool         m_destroy;

    std::string m_uuid;

    /* for search */
    std::string  m_tag;

    std::string  m_objname;

    std::weak_ptr<GObj> m_parent;
    
    std::unordered_set<std::shared_ptr<GCom>> m_coms;

    std::unordered_map<std::string, std::weak_ptr<GCom>> m_named_coms;

    std::unordered_set<std::shared_ptr<GObj>> m_children;

    static std::unordered_multimap<std::string, std::weak_ptr<GObj>> s_tagged_objs;
    static std::vector<std::weak_ptr<GCom>>                          s_destroy_coms;
    static std::vector<std::weak_ptr<GObj>>                          s_destroy_objs;
};

NS_G4Y_END

#endif