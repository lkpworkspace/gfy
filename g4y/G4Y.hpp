#ifndef __G4Y_H__
#define __G4Y_H__
#include "GConfig.hpp"
#include "GCommon.hpp"

#include "GWorld.hpp"
#include "GScene.hpp"

#include "GCom.hpp"
#include "GObj.hpp"

#include "GResourceMgr.hpp"
#include "GOpenGLView.hpp"
#include "GPhyWorld.hpp"

#include "GTransform.hpp"
#include "GCamera.hpp"

#include "GPhysics.hpp"
#include "GRigibody.hpp"
#include "GCollider.hpp"

#include "GGrid.hpp"
#include "GAxis.hpp"
#include "GSkybox.hpp"
#include "GCube.hpp"

#include "GShader.hpp"
#include "GMesh.hpp"
#include "GTexture.hpp"
#include "GMaterial.hpp"
#include "GMeshRenderer.hpp"

NS_G4Y_BEGIN

	class G4Y_DLL G4Y
	{
	public:
		std::string Version();
	};

NS_G4Y_END

#endif