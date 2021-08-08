#include "GCom.hpp"
#include "GObj.hpp"
#include "GWorld.hpp"

NS_G4Y_BEGIN

GCom::GCom() :
    m_start(false),
    m_destroy(false)
{}

GCom::~GCom()
{}

void GCom::OnStart()
{
    if(!m_start){
        m_start = true;
        Start();
    }
}

NS_G4Y_END