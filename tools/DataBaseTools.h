//
// Created by 黄铮 on 2022/4/6.
//

#ifndef TRAFFICSCORE_TOOLS_DATABASETOOLS_H_
#define TRAFFICSCORE_TOOLS_DATABASETOOLS_H_

#include <soci/row.h>
#include "MyMacros.h"

namespace DataBaseTools
{
	void getFieldStringValue(const soci::row &itRet, const std::string &fieldName, std::string &fieldVal);
	void getFieldStringValue(const soci::row &itRet, const int &pos, std::string &fieldVal);
};

#endif //TRAFFICSCORE_TOOLS_DATABASETOOLS_H_
