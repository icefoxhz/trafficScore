//
// Created by 黄铮 on 2022/4/6.
//

#ifndef TRAFFICSCORE_TOOLS_MYMACROS_H_
#define TRAFFICSCORE_TOOLS_MYMACROS_H_

#ifndef MYLOG_DEFINE
#define MYLOG_DEFINE(name) (log4cplus::Logger::getInstance(LOG4CPLUS_TEXT(name)))
#endif

#define POOL_SIZE 2
#define idField "WXGEOID"

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/configurator.h>
#include <log4cplus/initializer.h>
#include <log4cplus/loglevel.h>
#include <log4cplus/ndc.h>

struct GridScore
{
	std::string wkt;
	float score;
};


#endif //TRAFFICSCORE_TOOLS_MYMACROS_H_
