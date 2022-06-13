//
// Created by 黄铮 on 2022/4/2.
//

#include "BaseGenerator.h"

using namespace std;

string BaseGenerator::wktField = "wkt";  /* NOLINT */

BaseGenerator::BaseGenerator() {
	init();
};
BaseGenerator::BaseGenerator(string resultTable,
	const string& sourceTable,
	string connString)
	: m_resultTable(std::move(resultTable)),
	  m_connString(std::move(connString))
{
	init();

	// 分割获取所有表
	m_sourceTableList = move(CommonTools::split(sourceTable, ","));
}

void BaseGenerator::init()
{
    double oneIsHowMeter = 1;
	try
	{
		YAML::Node config = YAML::LoadFile("config.yaml");
        oneIsHowMeter = config["oneIsHowMeter"].as<double>();

		auto bufferDistance = config["bufferDistance"];
		m_bufferDistance = bufferDistance.as<double>();
		auto gridSide = config["gridSide"];
		m_gridSide = gridSide.as<double>();
	}catch (...){

		m_bufferDistance = 1000 * oneIsHowMeter;
		m_gridSide = 100 * oneIsHowMeter;
	}
}

bool BaseGenerator::parseTables(soci::session& sql, const string& fmtStrSql)
{
	try
	{
		string startLineStr("LINESTRING");
		string startPtStr("POINT");
		int i = 0;
		for (const auto& tableName : m_sourceTableList)
		{
			string wktVal;
			string strSql = fmt::format(fmtStrSql, wktField, tableName);
			sql << strSql, soci::into(wktVal);
			if (wktVal.empty())
			{
				LOG4CPLUS_ERROR(MyLogger::getInstance(), "表数据为空! table = " << tableName);
				return false;
			}
			transform(wktVal.begin(), wktVal.end(), wktVal.begin(), ::toupper);

			// 道路表.  中文
			if (wktVal.compare(0, startLineStr.size(), startLineStr) == 0)
			{
				m_roadTable = tableName;
				m_ch2enTableMapper.emplace(tableName, "roadLine");
			}
			else if (wktVal.compare(0, startPtStr.size(), startPtStr) == 0)
			{
				// 站点表
				m_stationTableList.push_back(tableName);
				m_ch2enTableMapper.emplace(tableName, "station_" + to_string(++i));
			}
		}

		return true;
	}
	catch (const exception& e)
	{
		LOG4CPLUS_ERROR(MyLogger::getInstance(), "parseTables error! => " << e.what());
	}
}

GDALDataset* BaseGenerator::openDataset(){
	auto *poDS = static_cast<GDALDataset*>(
		GDALOpenEx( _tempGdbName.c_str(), GDAL_OF_VECTOR, nullptr, nullptr, nullptr ));
	if( poDS == nullptr )
	{
		LOG4CPLUS_ERROR(MyLogger::getInstance(), "打开图层失败！");
		return nullptr;
	}

	return poDS;
}

bool BaseGenerator::createTempGdb(soci::session& sql, const std::string& fmtStrSql, const std::string& fmtStrSelectSql)
{
	// 解析表
	if (!parseTables(sql, fmtStrSql))
	{
		return false;
	}

	// 创建临时gdb
	GDALDataset* pDataset = createMainTempGdbOnly();
	if (pDataset == nullptr)
	{
		return false;
	}

	// 创建相关图层
	bool createOk = true;
	if (!createLayer(pDataset, sql, fmtStrSelectSql, m_roadTable, wkbLineString))
	{
		createOk = false;
		goto _releaseLabel;
	}
	for (const auto& tableName : m_stationTableList)
	{
		if (!createLayer(pDataset, sql, fmtStrSelectSql, tableName, wkbPoint))
		{
			createOk = false;
			goto _releaseLabel;
		}
	}

_releaseLabel:
	GDALClose(pDataset);
	if (!createOk)
	{
		return false;
	}

	return true;
}

GDALDataset* BaseGenerator::createTempGdbOnly(const std::string& gdbName)
{
	try
	{
		GDALDriver* poGdbDriver = GdbTools::initGDAL();
		string tempGdbName =  gdbName.empty() ? to_string(CommonTools::getTimeStamp()) + ".gdb" : gdbName + ".gdb";
		GDALDataset* poDS = poGdbDriver->Create(tempGdbName.c_str(), 0, 0, 0, GDT_Unknown, nullptr);
		if (poDS == nullptr)
		{
			LOG4CPLUS_ERROR(MyLogger::getInstance(), "创建临时gdb失败！");
			return nullptr;
		}

		return poDS;
	}
	catch (const exception& e)
	{
		LOG4CPLUS_ERROR(MyLogger::getInstance(), "createTempGdb error! => " << e.what());
	}
	return nullptr;
}

GDALDataset* BaseGenerator::createMainTempGdbOnly()
{
	try
	{
		GDALDriver* poGdbDriver = GdbTools::initGDAL();
		_tempGdbName =  to_string(CommonTools::getTimeStamp()) + ".gdb" ;
		GDALDataset* poDS = poGdbDriver->Create(_tempGdbName.c_str(), 0, 0, 0, GDT_Unknown, nullptr);
		if (poDS == nullptr)
		{
			LOG4CPLUS_ERROR(MyLogger::getInstance(),"创建临时gdb失败！");
			return nullptr;
		}

		return poDS;
	}
	catch (const exception& e)
	{
		LOG4CPLUS_ERROR(MyLogger::getInstance(),"createTempGdb error! => " << e.what());
	}
	return nullptr;
}

bool BaseGenerator::createLayer(GDALDataset* pDataset,
	soci::session& sql,
	const std::string& fmtStrSelectSql,
	const string& tableName,
	const OGRwkbGeometryType& geoType)
{
	try
	{
		string enTableName = m_ch2enTableMapper.at(tableName);
		OGRLayer* poLayer = pDataset->CreateLayer(enTableName.c_str(), nullptr, geoType, nullptr);
		if (poLayer == nullptr)
		{
			LOG4CPLUS_ERROR(MyLogger::getInstance(),"创建图层失败！layer=" << enTableName);
			return false;
		}

		string strSelectSql = fmt::format(fmtStrSelectSql, wktField, tableName);
		soci::rowset<soci::row> rs = (sql.prepare << strSelectSql);
		for (auto& row : rs)
		{
			string strWkt;
			DataBaseTools::getFieldStringValue(row, 0, strWkt);

			OGRGeometry* poGeometry;
			OGRGeometryFactory::createFromWkt(strWkt.c_str(), nullptr, &poGeometry);
			OGRFeature* poFeature = OGRFeature::CreateFeature(poLayer->GetLayerDefn());
			poFeature->SetGeometry(poGeometry);
			if (poLayer->CreateFeature(poFeature) != OGRERR_NONE)
			{
				LOG4CPLUS_ERROR(MyLogger::getInstance(),"创建元素失败！");
				OGRFeature::DestroyFeature(poFeature);
				break;
			}

			OGRFeature::DestroyFeature(poFeature);
		}

		return true;
	}
	catch (const exception& e)
	{
		LOG4CPLUS_ERROR(MyLogger::getInstance(),"createLayer error! => " << e.what());
	}
	return false;
}

bool BaseGenerator::generateResult(tbb::concurrent_queue<GridScore>& q)
{
	if (q.empty()){
		LOG4CPLUS_ERROR(MyLogger::getInstance(),"没有符合条件的数据要入库！");
		return false;
	}

	if (!createResultTable()){
		LOG4CPLUS_ERROR(MyLogger::getInstance(),"创建结果表失败！");
		return false;
	}

	const int batchSize = 50;
	string strSql;
	std::vector<string> sqlList;
	int i = 0;
	while (true)
	{
		GridScore gridScore;
		if (!q.try_pop(gridScore)){
			this_thread::sleep_for(chrono::seconds(1));
			continue;
		}

		if (gridScore.score == -1){
			break;
		}

		strSql = fmt ::format("insert into {}({}, WKT, SCORE) values({}, '{}', {})", m_resultTable, idField, i++, gridScore.wkt, gridScore.score);
//		LOG4CPLUS_ERROR(MyLogger::getInstance(), strSql);
		sqlList.push_back(strSql);

		if (sqlList.size() == batchSize){
			if (!batchInsert(sqlList)){
				return false;
			}
		}
	}

	if (!sqlList.empty()){
		if (!batchInsert(sqlList)){
			return false;
		}
	}

	return true;
}

const string& BaseGenerator::GetTempGdbName() const
{
	return _tempGdbName;
}



