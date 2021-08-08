#ifndef __GLOG_H__
#define __GLOG_H__
#include <string>

#include "GConfig.hpp"

NS_G4Y_BEGIN

enum class GlogLevel {
	GLOG_ERROR,
	GLOG_WARING,
	GLOG_INFO,
	GLOG_DEBUG,
	GLOG_TRACE,
};

class G4Y_DLL GLogger
{
public:
    GLogger();
    void Init();

	static void Log(std::string msg, GlogLevel lv = GlogLevel::GLOG_INFO, std::string file = "", std::string function = "", int line = -1);
};

#define GLOG(lv, msg) \
do {                                                               \
    GLogger::Log(msg, lv, __FILE__, "", __LINE__); \
} while (0)

NS_G4Y_END

#endif