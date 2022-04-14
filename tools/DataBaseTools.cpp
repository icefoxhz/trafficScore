//
// Created by 黄铮 on 2022/4/6.
//

#include "DataBaseTools.h"

using namespace std;
using namespace log4cplus;

auto logger_dbTools = MYLOG_DEFINE("DataBaseTools");


void DataBaseTools::getFieldStringValue(const soci::row &itRet, const string &fieldName, string &fieldVal) {
	try {
		const soci::column_properties &props = itRet.get_properties(fieldName);
		if (itRet.get_indicator(fieldName) == soci::i_null) {
			fieldVal = "";
			return;
		}

		auto fieldType = props.get_data_type();
		switch (fieldType) {
		default: fieldVal = "";
			break;
		case soci::dt_string: fieldVal = itRet.get<std::string>(fieldName);
			break;
		case soci::dt_double: fieldVal = std::to_string(itRet.get<double>(fieldName));
			break;
		case soci::dt_integer: fieldVal = std::to_string(itRet.get<int>(fieldName));
			break;
		case soci::dt_long_long: fieldVal = std::to_string(itRet.get<long long>(fieldName));
			break;
		case soci::dt_unsigned_long_long: fieldVal = std::to_string(itRet.get<unsigned long long>(fieldName));
			break;
		case soci::dt_date: std::tm when = itRet.get<std::tm>(fieldName);
			fieldVal = asctime(&when);
			break;
		}
	}
	catch (exception &e) {
		fieldVal = "";
		LOG4CPLUS_ERROR(logger_dbTools, "getFieldStringValue出错！error = " << e.what());
	}
}


void DataBaseTools::getFieldStringValue(const soci::row &itRet, const int &pos, string &fieldVal) {
	try {
		const soci::column_properties &props = itRet.get_properties(pos);
		if (itRet.get_indicator(pos) == soci::i_null) {
			fieldVal = "";
			return;
		}

		auto fieldType = props.get_data_type();
		switch (fieldType) {
		default: fieldVal = "";
			break;
		case soci::dt_string: fieldVal = itRet.get<std::string>(pos);
			break;
		case soci::dt_double: fieldVal = std::to_string(itRet.get<double>(pos));
			break;
		case soci::dt_integer: fieldVal = std::to_string(itRet.get<int>(pos));
			break;
		case soci::dt_long_long: fieldVal = std::to_string(itRet.get<long long>(pos));
			break;
		case soci::dt_unsigned_long_long: fieldVal = std::to_string(itRet.get<unsigned long long>(pos));
			break;
		case soci::dt_date: std::tm when = itRet.get<std::tm>(pos);
			fieldVal = asctime(&when);
			break;
		}
	}
	catch (exception &e) {
		fieldVal = "";
		LOG4CPLUS_ERROR(logger_dbTools, "getFieldStringValue出错！error = " << e.what());
	}
}
