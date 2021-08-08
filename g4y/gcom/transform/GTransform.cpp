#include "GTransform.hpp"
#include "GObj.hpp"

NS_G4Y_BEGIN

GTransform::Transform::Transform()
: pos(glm::vec3(0,0,0))
, rot(glm::vec3(0, 0, 0))
, scale(glm::vec3(1,1,1))
{}

GTransform::Transform::Transform(const glm::vec3& _pos, const glm::quat& _rot, glm::vec3 _scale)
: pos(_pos)
, rot(_rot)
, scale(_scale)
{}

GTransform::Transform::Transform(const GTransform::Transform &trans)
{
   operator=(trans);
}

GTransform::Transform& GTransform::Transform::operator=(const GTransform::Transform &trans)
{
    pos = trans.pos;
    rot = trans.rot;
    scale = trans.scale;

    return *this;
}

GTransform::Transform GTransform::Transform::Inverted() const
{
    Transform trans;

    trans.rot = glm::conjugate(rot);
    trans.pos = trans.rot * (-pos / scale);
    trans.scale = glm::vec3(1.0f / scale.x, 1.0f / scale.y, 1.0f / scale.z);
    return trans;
}

GTransform::Transform GTransform::Transform::operator*(const Transform& rhs) const
{
    return {(rot * (rhs.pos * scale)) + pos, rot * rhs.rot, scale * rhs.scale};
}

glm::mat4 GTransform::Transform::ToMat4() const
{
    auto mtx = glm::mat4_cast(rot);
    mtx = glm::scale(mtx, scale);
    //mtx = glm::translate(mtx, pos);
    mtx[3] = glm::vec4(pos.x, pos.y, pos.z, mtx[3].w);
    return mtx;
}

GTransform::GTransform()
{}

GTransform::~GTransform()
{}

void GTransform::SetPosition(glm::vec3 pos)
{
    wld_trans.pos = pos;
	m_position_changed_sig(pos.x, pos.y, pos.z);
    UpdateTransform(Obj(), true);
}

void GTransform::SetScale(glm::vec3 s)
{
    bool parent_valid = (Obj()->Parent() == nullptr) ? false : true;
    if(!parent_valid){
        wld_trans.scale = s;
        UpdateTransform(Obj(), true);
        return;
    }
    local_trans.scale = s;
    UpdateGlobalTransform(Obj());
}

void GTransform::SetRotation(glm::vec3 eulers)
{
    auto rot = glm::quat(glm::vec3(0,0,0));
	// RPY
	// PITCH: x, YAW: y, ROLL: z
    rot = glm::rotate(rot, glm::radians(eulers.y), glm::vec3(0, 1, 0));
    rot = glm::rotate(rot, glm::radians(eulers.x), glm::vec3(1, 0, 0));
    rot = glm::rotate(rot, glm::radians(eulers.z), glm::vec3(0, 0, 1));

    SetRotation(rot);
}

void GTransform::SetRotation(glm::quat q)
{
    wld_trans.rot = q;
	m_rotation_changed_sig(q.x, q.y, q.z, q.w);
    UpdateTransform(Obj(), true);
}

void GTransform::RotateAround(glm::vec3 target, glm::vec3 axis, float euler)
{
    auto q = glm::angleAxis(glm::radians(euler), glm::normalize(axis));
    wld_trans.pos = (q * (wld_trans.pos - target)) + target;
    UpdateTransform(Obj(), true);
}

void GTransform::LookAt(glm::vec3 target, glm::vec3 wld_up)
{
    wld_trans.rot = glm::quatLookAt(glm::normalize(target - wld_trans.pos) * -1.0f, wld_up);
    UpdateTransform(Obj(), true);
}

void GTransform::Translate(float x, float y, float z)
{
    Translate(glm::vec3(x, y, z));
}

void GTransform::Translate(glm::vec3 translation)
{
    wld_trans.pos += translation;
    UpdateTransform(Obj(), true);
}

glm::vec3 GTransform::Position()
{
    return wld_trans.pos;
}

glm::vec3 GTransform::EulerAngles()
{
    // 角度转弧度 π/180×角度
    // 弧度变角度 180/π×弧度
    auto radians = glm::eulerAngles(wld_trans.rot);
    return glm::vec3(glm::degrees<float>(radians.x), glm::degrees<float>(radians.y), glm::degrees<float>(radians.z));
}

glm::quat GTransform::Rotation()
{
    return wld_trans.rot;
}

glm::vec3 GTransform::Up()
{
    auto q = wld_trans.rot;
    return glm::normalize(mat3_cast(q)[1]);
}

glm::vec3 GTransform::Right()
{
    auto q = wld_trans.rot;
    return glm::normalize(mat3_cast(q)[0]);
}

glm::vec3 GTransform::Forward()
{
    auto q = wld_trans.rot;
    return glm::normalize(mat3_cast(q)[2]);
}

glm::vec3 GTransform::LocalPosition()
{
    bool parent_valid = (Parent() == nullptr) ? false : true;
    if(!parent_valid){
        return wld_trans.pos;
    }
    return local_trans.pos;
}
glm::vec3 GTransform::LocalEulerAngles()
{
    glm::vec3 radians;
    bool parent_valid = (Parent() == nullptr) ? false : true;
    if(!parent_valid){
        radians = glm::eulerAngles(wld_trans.rot);
    }else{
        radians = glm::eulerAngles(local_trans.rot);
    }
    return glm::vec3(glm::degrees<float>(radians.x), glm::degrees<float>(radians.y), glm::degrees<float>(radians.z));
}
glm::quat GTransform::LocalRotation()
{
    bool parent_valid = (Parent() == nullptr) ? false : true;
    if(!parent_valid){
        return wld_trans.rot;
    }
    return local_trans.rot;
}
glm::vec3 GTransform::Scale()
{
    bool parent_valid = (Parent() == nullptr) ? false : true;
    if(!parent_valid){
        return wld_trans.scale;
    }
    return local_trans.scale;
}

void GTransform::SetLocalPosition(glm::vec3 pos)
{
    bool parent_valid = (Parent() == nullptr) ? false : true;
    if(!parent_valid){
        SetPosition(pos);
        return;
    }
    local_trans.pos = pos;
    UpdateGlobalTransform(Obj());

	auto wpos = wld_trans.pos;
	m_position_changed_sig(wpos.x, wpos.y, wpos.z);
}
void GTransform::SetLocalRotation(glm::vec3 eulers)
{
    auto rot = glm::quat(glm::vec3(0,0,0));
    rot = glm::rotate(rot, glm::radians(eulers.y), glm::vec3(0, 1, 0));
    rot = glm::rotate(rot, glm::radians(eulers.x), glm::vec3(1, 0, 0));
    rot = glm::rotate(rot, glm::radians(eulers.z), glm::vec3(0, 0, 1));
    SetLocalRotation(rot);
}
void GTransform::SetLocalRotation(glm::quat q)
{
    bool parent_valid = (Parent() == nullptr) ? false : true;
    if(!parent_valid){
        SetRotation(q);
        return;
    }
    local_trans.rot = q;
    UpdateGlobalTransform(Obj());

	auto wrot = wld_trans.rot;
	m_rotation_changed_sig(wrot.x, wrot.y, wrot.z, wrot.w);
}

glm::mat4 GTransform::ToMat4()
{
    return wld_trans.ToMat4();
}

void GTransform::Start()
{
    auto parent = Parent();
    if(parent){
        m_parent_trans = parent->GetCom<GTransform>();
    }
	
	auto wrot = wld_trans.rot;
	m_rotation_changed_sig(wrot.x, wrot.y, wrot.z, wrot.w);
	
	auto wpos = wld_trans.pos;
	m_position_changed_sig(wpos.x, wpos.y, wpos.z);

    UpdateTransform(Obj(), true);
}

// update DFS
void GTransform::UpdateTransform(std::shared_ptr<GObj> obj, bool update_local)
{
    if(obj == nullptr) return;
    bool parent_valid = (obj->Parent() == nullptr) ? false : true;

    // {
    //     auto p = local_trans.pos;
    //     auto s = local_trans.scale;
    //     std::cout << "before local  pos   : " << p.x << " " << p.y << " " << p.z << std::endl;
    //     std::cout << "before local  scale : " << s.x << " " << s.y << " " << s.z << std::endl;

    //     p = wld_trans.pos;
    //     s = wld_trans.scale;
    //     std::cout << "wld pos             : " << p.x << " " << p.y << " " << p.z << std::endl;
    //     std::cout << "wld scale           : " << s.x << " " << s.y << " " << s.z << std::endl;
    // }

    if(update_local && parent_valid){
        auto inverted_trans = obj->Parent()->GetCom<GTransform>()->wld_trans.Inverted();
        // auto p = inverted_trans.pos;
        // auto s = inverted_trans.scale;
        // std::cout << "invert pos          : " << p.x << " " << p.y << " " << p.z << std::endl;
        // std::cout << "invert scale        : " << s.x << " " << s.y << " " << s.z << std::endl;
        local_trans = (inverted_trans * wld_trans);
        // p = local_trans.pos;
        // s = local_trans.scale;
        // std::cout << "local  pos          : " << p.x << " " << p.y << " " << p.z << std::endl;
        // std::cout << "local  scale        : " << s.x << " " << s.y << " " << s.z << std::endl;
    }

    auto children = obj->Children();
    for(const auto& o : children){
        o->GetCom<GTransform>()->wld_trans = (wld_trans * o->GetCom<GTransform>()->local_trans);
        // auto p = o->Transform()->wld_trans.pos;
        // auto e = o->Transform()->EulerAngles();
        // auto s = o->Transform()->local_trans.scale;
        // std::cout << "pos   : " << p.x << " " << p.y << " " << p.z << std::endl;
        // std::cout << "euler : " << e.x << " " << e.y << " " << e.z << std::endl;
        // std::cout << "scale : " << s.x << " " << s.y << " " << s.z << std::endl;
        UpdateTransform(o, false);
    }
}

void GTransform::UpdateGlobalTransform(std::shared_ptr<GObj> obj)
{
    wld_trans = obj->Parent()->GetCom<GTransform>()->wld_trans * local_trans;
    UpdateTransform(Obj(), false);
}

boost::signals2::connection GTransform::connect(const position_changed_sig_t::slot_type &subscriber)
{
	return m_position_changed_sig.connect(subscriber);
}

boost::signals2::connection GTransform::connect(const rotation_changed_sig_t::slot_type &subscriber)
{
	return m_rotation_changed_sig.connect(subscriber);
}

// void GTransform::Update()
// {
//     if(!m_parent_trans.expired()){
//         wld_postion = m_parent_trans.lock()->wld_postion + postion;
//         wld_rotate  = m_parent_trans.lock()->wld_rotate + rotate;
//         wld_scale   = m_parent_trans.lock()->wld_scale * scale;
//     }else{
//         wld_postion = postion;
//         wld_rotate  = rotate;
//         wld_scale   = scale;
//     }
//     glm::mat4 t(1.0f);
//     glm::mat4 r(1.0f);
//     glm::mat4 s(1.0f);

//     s = glm::scale(s, scale);

//     r = glm::rotate(r, glm::radians(rotate.x), glm::vec3(1.0f, 0.0f, 0.0f));
//     r = glm::rotate(r, glm::radians(rotate.y), glm::vec3(0.0f, 1.0f, 0.0f));
//     r = glm::rotate(r, glm::radians(rotate.z), glm::vec3(0.0f, 0.0f, 1.0f));

//     t = glm::translate(t, postion);

//     // SRT
//     if(m_parent_trans.expired()){
//         model = t * r * s;
//     }else{
//         model = m_parent_trans.lock()->model * t * r * s;
//     }
// }

GTransformWarp::GTransformWarp() {
	std::cout << "transform warp construct1" << std::endl;
	m_com = std::make_shared<GTransform>();
}

GTransformWarp::GTransformWarp(std::shared_ptr<GTransform> t) {
	std::cout << "transform warp construct2" << std::endl;
	m_com = t;
}

GTransformWarp::GTransformWarp(const GTransformWarp& o) {
	std::cout << "transform warp construct3" << std::endl;
	operator=(o);
}

GTransformWarp& GTransformWarp::operator=(const GTransformWarp& o) {
	std::cout << "transform warp construct4" << std::endl;
	m_com = o.m_com;
	return *this;
}

GTransformWarp::~GTransformWarp()
{
	std::cout << "deconstruct GTransformWarp" << std::endl;
}

void GTransformWarp::setPosition(boost::python::object o) {
	float x = boost::python::extract<float>(o[0]);
	float y = boost::python::extract<float>(o[1]);
	float z = boost::python::extract<float>(o[2]);
	get<GTransform>()->SetPosition(glm::vec3(x, y, z));
}

boost::python::object GTransformWarp::getPosition() {
	auto pos = get<GTransform>()->Position();
	return boost::python::make_tuple(pos.x, pos.y, pos.z);
}

void GTransformWarp::setEulerAngles(boost::python::object o) {
	float x = boost::python::extract<float>(o[0]);
	float y = boost::python::extract<float>(o[1]);
	float z = boost::python::extract<float>(o[2]);
	get<GTransform>()->SetRotation(glm::vec3(x, y, z));
}

boost::python::object GTransformWarp::getEulerAngles() {
	auto pos = get<GTransform>()->EulerAngles();
	return boost::python::make_tuple(pos.x, pos.y, pos.z);
}

void GTransformWarp::setScale(boost::python::object o) {
	float x = boost::python::extract<float>(o[0]);
	float y = boost::python::extract<float>(o[1]);
	float z = boost::python::extract<float>(o[2]);
	get<GTransform>()->SetScale(glm::vec3(x, y, z));
}

boost::python::object GTransformWarp::getScale() {
	auto pos = get<GTransform>()->Scale();
	return boost::python::make_tuple(pos.x, pos.y, pos.z);
}

boost::python::object GTransformWarp::getForward() {
	auto pos = get<GTransform>()->Forward();
	return boost::python::make_tuple(pos.x, pos.y, pos.z);
}

boost::python::object GTransformWarp::getRight() {
	auto pos = get<GTransform>()->Right();
	return boost::python::make_tuple(pos.x, pos.y, pos.z);
}

boost::python::object GTransformWarp::getUp() {
	auto pos = get<GTransform>()->Up();
	return boost::python::make_tuple(pos.x, pos.y, pos.z);
}

void GTransformWarp::translate(boost::python::object o) {
	float x = boost::python::extract<float>(o[0]);
	float y = boost::python::extract<float>(o[1]);
	float z = boost::python::extract<float>(o[2]);
	get<GTransform>()->Translate(x, y, z);
}

boost::python::object GTransformWarp::getMethodList()
{
	boost::python::list l;
	l.append("setPosition");
	l.append("setEulerAngles");
	l.append("setScale");
	l.append("getPosition");
	l.append("getEulerAngles");
	l.append("getScale");
	l.append("getForward");
	l.append("getRight");
	l.append("getUp");
	return l;
}

std::string GTransformWarp::getMethodInfo(const std::string& method_name)
{
	if (method_name == "setPosition"
		|| method_name == "setEulerAngles"
		|| method_name == "setScale"
		|| method_name == "translate")
	{
/*
{
	"static":false,
	"args":[
		{
			"type":"flow"
		},
		{
			"type":"GTransform",
			"desc":"target"
		},
		{
			"type":"float",
			"desc":"x",
			"value":"0.0"
		},
		{
			"type":"float",
			"desc":"y",
			"value":"0.0"
		},
		{
			"type":"float",
			"desc":"z",
			"value":"0.0"
		}
	],
	"ret":[
		{
			"type":"flow"
		}
	]
}
*/
		return "{\n"
			"    \"static\":false,\n"
			"	\"args\":[\n"
			"		{\n"
			"			\"type\":\"flow\"\n"
			"		},\n"
			"		{\n"
			"			\"type\":\"GTransform\",\n"
			"			\"desc\":\"target\"\n"
			"		},\n"
			"		{\n"
			"			\"type\":\"float\",\n"
			"			\"desc\":\"x\",\n"
			"			\"value\":\"0.0\"\n"
			"		},\n"
			"		{\n"
			"			\"type\":\"float\",\n"
			"			\"desc\":\"y\",\n"
			"			\"value\":\"0.0\"\n"
			"		},\n"
			"		{\n"
			"			\"type\":\"float\",\n"
			"			\"desc\":\"z\",\n"
			"			\"value\":\"0.0\"\n"
			"		}\n"
			"	],\n"
			"	\"ret\":[\n"
			"		{\n"
			"			\"type\":\"flow\"\n"
			"		}\n"
			"	]\n"
			"}	";
	}
	if (method_name == "getPosition"
		|| method_name == "getEulerAngles"
		|| method_name == "getScale"
		|| method_name == "getForward"
		|| method_name == "getRight"
		|| method_name == "getUp") {
/*
{
	"static":false,
	"args":[
		{
			"type":"flow"
		},
		{
			"type":"GTransform",
			"desc":"target"
		}
	],
	"ret":[
		{
			"type":"flow"
		},
		{
			"type":"float",
			"desc":"x",
			"value":"0.0"
		},
		{
			"type":"float",
			"desc":"y",
			"value":"0.0"
		},
		{
			"type":"float",
			"desc":"z",
			"value":"0.0"
		}
	]
}
*/
		return "{\n"
			"    \"static\":false,\n"
			"	\"args\":[\n"
			"		{\n"
			"			\"type\":\"flow\"\n"
			"		},\n"
			"		{\n"
			"			\"type\":\"GTransform\",\n"
			"			\"desc\":\"target\"\n"
			"		}\n"
			"	],\n"
			"	\"ret\":[\n"
			"		{\n"
			"			\"type\":\"flow\"\n"
			"		},\n"
			"		{\n"
			"			\"type\":\"float\",\n"
			"			\"desc\":\"x\",\n"
			"			\"value\":\"0.0\"\n"
			"		},\n"
			"		{\n"
			"			\"type\":\"float\",\n"
			"			\"desc\":\"y\",\n"
			"			\"value\":\"0.0\"\n"
			"		},\n"
			"		{\n"
			"			\"type\":\"float\",\n"
			"			\"desc\":\"z\",\n"
			"			\"value\":\"0.0\"\n"
			"		}\n"
			"	]\n"
			"}	";
	}
	return "";
}

NS_G4Y_END