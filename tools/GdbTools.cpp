//
// Created by 黄铮 on 2022/4/2.
//

#include "GdbTools.h"

using namespace std;

auto logger_gdbTools = MYLOG_DEFINE("GdbTools");

GDALDriver* GdbTools::initGDAL()
{
	const char *pszDriverName = "FileGDB";
	try
	{
		GDALAllRegister();

		CPLSetConfigOption( "GDAL_FILENAME_IS_UTF8", "NO" );
		CPLSetConfigOption( "SHAPE_ENCODING", "CP936" );

		GDALDriver *poGdbDriver = GetGDALDriverManager()->GetDriverByName(pszDriverName);
		if (poGdbDriver == nullptr)
		{
			LOG4CPLUS_ERROR(logger_gdbTools, "驱动加载失败！Driver = " << pszDriverName);
			return nullptr;
		}

		return poGdbDriver;
	}catch (const exception& e){
		LOG4CPLUS_ERROR(logger_gdbTools, "initGDAL出错！error = " << e.what());
	}
	return nullptr;
}
