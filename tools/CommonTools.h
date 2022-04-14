//
// Created by 黄铮 on 2022/4/6.
//

#ifndef TRAFFICSCORE_TOOLS_COMMONTOOLS_H_
#define TRAFFICSCORE_TOOLS_COMMONTOOLS_H_

#include <locale>
#include <codecvt>
#include "string"
#include "vector"
#include <iomanip>
#include "iostream"
#include "regex"
#include "MyMacros.h"
#include "MyLogger.h"

namespace CommonTools
{
	std::vector<std::string> split(const std::string& in, const std::string& delim);
	long long getTimeStamp();
	float round(float f, int bits);
	std::wstring toWideString(const std::string& input);
	std::string toByteString(const std::wstring& input);
};

#endif //TRAFFICSCORE_TOOLS_COMMONTOOLS_H_
