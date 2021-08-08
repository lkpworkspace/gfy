#include <iostream>
#include "GObj.hpp"
#include "GCom.hpp"
#include "GScene.hpp"
#include "GTransform.hpp"

#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/random_generator.hpp>

NS_G4Y_BEGIN

std::unordered_multimap<std::string, std::weak_ptr<GObj>> GObj::s_tagged_objs;
std::vector<std::weak_ptr<GCom>>                          GObj::s_destroy_coms;
std::vector<std::weak_ptr<GObj>>                          GObj::s_destroy_objs;

GObj::GObj() :
    std::enable_shared_from_this<GObj>(),
    m_destroy(false),
    m_destroy_flag(false),
    m_tag("Untagged")
{
    Init();
}

GObj::~GObj()
{}

void GObj::Init()
{
    m_active = true;
#ifndef _WIN32
    boost::uuids::uuid a_uuid = boost::uuids::random_generator()(); 
    m_uuid = boost::uuids::to_string(a_uuid);
#endif
}

void GObj::SetTag(std::string tag)
{
    if(tag == "Untagged") return;
    if(m_tag != "Untagged"){
        std::cout << "[WARNING] SetTag Again" << std::endl;
        return;
    }
    m_tag = tag;
    s_tagged_objs.insert({tag, shared_from_this()});
}

// FIXME: 引用不存在的元素需要被删除
std::shared_ptr<GObj> GObj::FindWithTag(std::string tag)
{
    auto search = s_tagged_objs.find(tag);
    if(search != s_tagged_objs.end()){
        if(!search->second.expired())
            return search->second.lock();
    }
    return nullptr;
}

// FIXME: 引用不存在的元素需要被删除
std::vector<std::shared_ptr<GObj>> GObj::FindObjsWithTag(std::string tag)
{
    std::vector<std::shared_ptr<GObj>> ret;
    auto pr = s_tagged_objs.equal_range (tag); // pair of begin & end iterators returned
    while(pr.first != pr.second)
    {
        if(!pr.first->second.expired()){
            ret.push_back(pr.first->second.lock());
            ++pr.first; // Increment begin iterator
        }else{
            pr.first = s_tagged_objs.erase(pr.first);
        }
    }
    return ret;
}

void GObj::Destroy(std::shared_ptr<GObj> o)
{
    if(o->m_destroy_flag) return;
    o->m_destroy_flag = true;
    s_destroy_objs.push_back(o);
}

void GObj::Destroy(std::shared_ptr<GCom> c)
{
    if(c->m_destroy) return;
    c->m_destroy = true;
    s_destroy_coms.push_back(c);
}

bool GObj::AddChild(std::shared_ptr<GObj> obj)
{
	if (m_children.find(obj) != m_children.end()) return false;
    m_children.insert(obj);
    obj->m_parent = shared_from_this();

	return true;
}

void GObj::DelChild(std::shared_ptr<GObj> obj)
{
    m_children.erase(obj);
}
bool GObj::AddCom(boostpy::object c) {
	std::string type = boostpy::extract<std::string>(c.attr("objType")());
	if (type == "pyObj") {
		// TODO ...
		return false;
	}
	else if (type == "cppObj") {
		GComWarp& com = boostpy::extract<GComWarp&>(c);
		bool ret = AddCom(com.m_com);
		if (ret) com.m_com->SetPyComRef(c);
		return ret;
	}
	return false;
}
bool GObj::AddCom(std::shared_ptr<GCom> com)
{   
	if (m_coms.find(com) != m_coms.end()) return false;
	if (m_named_coms.find(com->TypeName()) != m_named_coms.end()) return false;

    com->m_obj = shared_from_this();
    com->Init();

    m_coms.insert(com);
    m_named_coms[com->TypeName()] = com;
	return true;
}

void GObj::DelCom(std::shared_ptr<GCom> com)
{
	assert(m_coms.find(com) != m_coms.end());
	assert(m_named_coms.find(com->TypeName()) != m_named_coms.end());

    com->OnDestroy();
    m_named_coms.erase(com->TypeName());
    m_coms.erase(com);
}

std::shared_ptr<GCom> GObj::GetCom(std::string com_name)
{
    if(m_named_coms.find(com_name) == m_named_coms.end())
        return nullptr;
	else {
		auto wp = m_named_coms[com_name];
		if (wp.expired()) {
			return nullptr;
		}
		return wp.lock();
	}
}

std::vector<std::shared_ptr<GCom>> GObj::GetComs()
{
    std::vector<std::shared_ptr<GCom>> coms;
    for(const auto& c : m_coms){
        coms.push_back(c);
    }
    return coms;
}

std::vector<std::shared_ptr<GObj>> GObj::Children() 
{ 
    return std::vector<std::shared_ptr<GObj>>{m_children.begin(), m_children.end()}; 
}

void GObj::Start()
{
    if(!m_active) return;
    if(m_destroy_flag || m_destroy) return;

    for(const auto& c : m_coms){
        if(c->m_destroy) continue;
        c->OnStart();
    }
    for(const auto& o : m_children){
        o->Start();
    }
}

void GObj::UpdateComAndChildren()
{
    if(!m_active) return;
    if(m_destroy_flag || m_destroy) return;

    for(const auto& c : m_coms){
        if(!c->m_start || c->m_destroy) continue;
        c->Update();
    }
    for(const auto& o : m_children){
        o->UpdateComAndChildren();
    }
}

void GObj::UpdateLate()
{
    if(!m_active) return;
    if(m_destroy_flag || m_destroy) return;

    for(const auto& c : m_coms){
        if(!c->m_start || c->m_destroy) continue;
        c->LateUpdate();
    }
    for(const auto& o : m_children){
        o->UpdateLate();
    }
}

void GObj::UpdateRender()
{
    if(!m_active) return;
    if(m_destroy_flag || m_destroy) return;

    for(const auto& c : m_coms){
        if(!c->m_start || c->m_destroy) continue;
        c->OnRender();
    }
    for(const auto& o : m_children){
        o->UpdateRender();
    }
}

void GObj::AddDefaultComs()
{
    AddCom(std::static_pointer_cast<GCom>(std::make_shared<GTransform>()));
}

NS_G4Y_END