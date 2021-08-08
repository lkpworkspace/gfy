#ifndef __GTRANSFORM_H__
#define __GTRANSFORM_H__
#include <iostream>
#include <boost/signals2.hpp>

#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtx/quaternion.hpp"

#include "GCom.hpp"

NS_G4Y_BEGIN

class G4Y_DLL GTransform : public GCom
{
    G_COM
public:
	typedef boost::signals2::signal<void (float, float, float)> position_changed_sig_t;
	typedef boost::signals2::signal<void (float, float, float, float)> rotation_changed_sig_t;
public:
    GTransform();
    virtual ~GTransform();

    virtual void Start() override;

    glm::vec3 Position();
    glm::vec3 EulerAngles();
    glm::quat Rotation();

    void SetPosition(glm::vec3);
    void SetRotation(glm::vec3 eulers);
    void SetRotation(glm::quat q);
    void RotateAround(glm::vec3 target, glm::vec3 axis, float euler);

    glm::vec3 LocalPosition();
    glm::vec3 LocalEulerAngles();
    glm::quat LocalRotation();

    void SetLocalPosition(glm::vec3);
    void SetLocalRotation(glm::vec3);
    void SetLocalRotation(glm::quat q);

    void LookAt(glm::vec3 target, glm::vec3 wld_up = glm::vec3(0, 1, 0));

    glm::vec3 Forward();
    glm::vec3 Right();
    glm::vec3 Up();

    void Translate(glm::vec3 translation);
    void Translate(float x, float y, float z);

    glm::mat4 ToMat4();

    glm::vec3 Scale();
    void SetScale(glm::vec3);

	boost::signals2::connection connect(const position_changed_sig_t::slot_type &subscriber);
	boost::signals2::connection connect(const rotation_changed_sig_t::slot_type &subscriber);

	virtual std::string Name() { return m_name; }
	virtual void SetName(std::string name) { m_name = name; }
private:
    void UpdateTransform(std::shared_ptr<GObj> obj, bool update_local);
    void UpdateGlobalTransform(std::shared_ptr<GObj> obj);

    struct G4Y_DLL Transform{
        glm::quat rot;
        glm::vec3 pos;
        glm::vec3 scale;

        Transform();

        Transform(const glm::vec3& _pos, const glm::quat& _rot, glm::vec3 _scale);

        Transform(const Transform &trans);

        Transform& operator=(const Transform &trans);

        Transform Inverted() const;

        Transform operator*(const Transform& rhs) const;

        glm::mat4 ToMat4() const;
    };

    Transform local_trans;
    Transform wld_trans;
	std::string m_name;
    std::weak_ptr<GTransform> m_parent_trans;
	position_changed_sig_t m_position_changed_sig;
	rotation_changed_sig_t m_rotation_changed_sig;
};

class G4Y_DLL GTransformWarp : public GComWarp
{
public:
	GTransformWarp();
	GTransformWarp(std::shared_ptr<GTransform> t);
	GTransformWarp(const GTransformWarp& o);

	~GTransformWarp();

	GTransformWarp& operator=(const GTransformWarp& o);

	static boost::python::object getMethodList();

	static std::string getMethodInfo(const std::string&);

	void setPosition(boost::python::object o);
	boost::python::object getPosition();

	void setEulerAngles(boost::python::object o);

	boost::python::object getEulerAngles();

	void setScale(boost::python::object o);
	boost::python::object getScale();

	boost::python::object getForward();
	boost::python::object getRight();
	boost::python::object getUp();

	void translate(boost::python::object o);
};

NS_G4Y_END

#endif

