# g4y

include(${G4Y_ROOT_PATH}/g4y/gcom/CMakeLists.txt)

set(G4Y_SRC
    ${G4Y_ROOT_PATH}/g4y/GConfig.hpp
	${G4Y_ROOT_PATH}/g4y/GCommon.hpp
	${G4Y_ROOT_PATH}/g4y/G4Y.hpp
    ${G4Y_ROOT_PATH}/g4y/G4Y.cpp
	${G4Y_ROOT_PATH}/g4y/GLogger.hpp
    ${G4Y_ROOT_PATH}/g4y/GLogger.cpp
	${G4Y_ROOT_PATH}/g4y/GObj.hpp
    ${G4Y_ROOT_PATH}/g4y/GObj.cpp
	${G4Y_ROOT_PATH}/g4y/GCom.hpp
    ${G4Y_ROOT_PATH}/g4y/GCom.cpp
	${G4Y_ROOT_PATH}/g4y/GScene.hpp
    ${G4Y_ROOT_PATH}/g4y/GScene.cpp
	${G4Y_ROOT_PATH}/g4y/GWorld.hpp
    ${G4Y_ROOT_PATH}/g4y/GWorld.cpp
	${G4Y_ROOT_PATH}/g4y/GPhyWorld.hpp
    ${G4Y_ROOT_PATH}/g4y/GPhyWorld.cpp
	${G4Y_ROOT_PATH}/g4y/GOpenGLView.hpp
    ${G4Y_ROOT_PATH}/g4y/GOpenGLView.cpp
	${G4Y_ROOT_PATH}/g4y/GResourceMgr.hpp
    ${G4Y_ROOT_PATH}/g4y/GResourceMgr.cpp
)