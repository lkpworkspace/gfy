#ifndef __GConfig_hpp__
#define __GConfig_hpp__

#define G4Y_PLATFORM_UNKNOWN            0
#define G4Y_PLATFORM_IOS                1
#define G4Y_PLATFORM_ANDROID            2
#define G4Y_PLATFORM_WIN32              3
#define G4Y_PLATFORM_LINUX              5

#define G4Y_TARGET_PLATFORM             G4Y_PLATFORM_UNKNOWN

#if defined(__APPLE__) && !defined(ANDROID) // exclude android for binding generator.
#include <TargetConditionals.h>
#if TARGET_OS_IPHONE // TARGET_OS_IPHONE includes TARGET_OS_IOS TARGET_OS_TV and TARGET_OS_WATCH. see TargetConditionals.h
#undef  G4Y_TARGET_PLATFORM
#define G4Y_TARGET_PLATFORM         G4Y_PLATFORM_IOS
#elif TARGET_OS_MAC
#undef  G4Y_TARGET_PLATFORM
#define G4Y_TARGET_PLATFORM         G4Y_PLATFORM_MAC
#endif
#endif

// android
#if defined(ANDROID)
#undef  G4Y_TARGET_PLATFORM
#define G4Y_TARGET_PLATFORM         G4Y_PLATFORM_ANDROID
#endif

// win32
#if defined(_WIN32) && defined(_WINDOWS)
#undef  G4Y_TARGET_PLATFORM
#define G4Y_TARGET_PLATFORM         G4Y_PLATFORM_WIN32
#endif

// linux
#if defined(LINUX) && !defined(__APPLE__)
#undef  G4Y_TARGET_PLATFORM
#define G4Y_TARGET_PLATFORM         G4Y_PLATFORM_LINUX
#endif

#if ! G4Y_TARGET_PLATFORM
#error  "Cannot recognize the target platform; are you targeting an unsupported platform?"
#endif

#if G4Y_TARGET_PLATFORM == G4Y_PLATFORM_WIN32
#if defined(G4Y_EXPORT_DLL)
#define G4Y_DLL     __declspec(dllexport)
#else         /* use a DLL library */
#define G4Y_DLL     __declspec(dllimport)
#endif
#endif

#define NS_G4Y_BEGIN      namespace g4y {
#define NS_G4Y_END        }
#define USING_NS_G4Y      using namespace g4y
#define NS_G4Y            ::g4y

#endif