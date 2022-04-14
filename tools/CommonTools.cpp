//
// Created by 黄铮 on 2022/4/6.
//

#include "CommonTools.h"

using namespace std;

std::vector<std::string> CommonTools::split(const std::string& in, const std::string& delim)
{
//	LOG4CPLUS_DEBUG(MyLogger::getInstance(), "test split");

	vector<string> ret;
	try
	{
		regex re{ delim };
		return vector<string>{
			sregex_token_iterator(in.begin(), in.end(), re, -1),
			sregex_token_iterator()
		};
	}
	catch (const std::exception& e)
	{
		LOG4CPLUS_ERROR(MyLogger::getInstance(), "error:" << e.what());
	}
	return ret;

}

long long CommonTools::getTimeStamp()
{
	chrono::time_point<chrono::system_clock, chrono::milliseconds>
		tp = chrono::time_point_cast<chrono::milliseconds>(chrono::system_clock::now());
	return chrono::duration_cast<chrono::milliseconds>(tp.time_since_epoch()).count();
}

float CommonTools::round(float f, int bits)
{
	stringstream ss;
	ss << fixed << setprecision(bits) << f;
	ss >> f;

	return f;
}

// convert string to wstring
std::wstring CommonTools::toWideString(const std::string& input)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	return converter.from_bytes(input);
}
// convert wstring to string
std::string CommonTools::toByteString(const std::wstring& input)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	return converter.to_bytes(input);
}