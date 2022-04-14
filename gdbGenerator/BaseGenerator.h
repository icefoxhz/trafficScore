//
// Created by 黄铮 on 2022/4/2.
//

#ifndef TRAFFIC_SCORE_GDB_GENERATOR_BASE_GENERATOR_H_
#define TRAFFIC_SCORE_GDB_GENERATOR_BASE_GENERATOR_H_

#include <string>
#include "regex"
#include <iostream>
#include <queue>
#include <fmt/format.h>
#include "soci/soci.h"
#include "yaml-cpp/yaml.h"
#include "soci/postgresql/soci-postgresql.h"
#include "oneapi/tbb/concurrent_queue.h"
//#include "oneapi/tbb/concurrent_vector.h"
#include "../tools/CommonTools.h"
#include "../tools/DataBaseTools.h"
#include "../tools/GdbTools.h"

// 2000坐标系下  1代表111.2公里
inline double oneMeter()
{
//	return 1 / 111.2 / 1000;
	return 1.0;
}

class BaseGenerator
{
 public:
	std::string m_resultTable;
	std::vector<std::string> m_sourceTableList;
	std::string m_connString;
	std::string m_roadTable;
	std::vector<std::string> m_stationTableList;
	std::map<std::string, std::string> m_ch2enTableMapper;
	double m_bufferDistance{};
	double m_gridSide{};

	static std::string wktField;
 private:
	std::string _tempGdbName;
 public:
	const std::string& GetTempGdbName() const;
 private:
	void init();
 public:
	virtual bool generateGdbFile()
	{
		return false;
	};

	virtual bool createResultTable() = 0;

	/**
	 * 批量入库
	 * @param sqlList
	 * @return
	 */
	virtual bool batchInsert(std::vector<std::string>& sqlList) = 0;

 public:
	BaseGenerator();
	BaseGenerator(std::string resultTable,
		const std::string& sourceTable,
		std::string connString);

	/**
	 * 生成临时gdb
	 * @return GDALDataset*
	 */
	static GDALDataset* createTempGdbOnly(const std::string& gdbName="");

	GDALDataset* createMainTempGdbOnly();

	/**
	 * 解析出哪个是 路网表， 哪些是站点表
	 * @param sql
	 * @param fmtStrSql
	 * @return
	 */
	bool parseTables(soci::session& sql, const std::string& fmtStrSql);

	/**
	 * 生成临时gdb
	 * @param sql
	 * @param fmtStrSql
	 * @param fmtStrSelectSql
	 * @return
	 */
	bool createTempGdb(soci::session& sql, const std::string& fmtStrSql, const std::string& fmtStrSelectSql);

	/**
	 * 打开数据
	 * @return
	 */
	GDALDataset* openDataset();

	/**
	 * 创建图层
	 * @param pDataset
	 * @param sql
	 * @param fmtStrSelectSql
	 * @param tableName
	 * @param geoType
	 * @return
	 */
	bool createLayer(GDALDataset* pDataset,
		soci::session& sql,
		const std::string& fmtStrSelectSql,
		const std::string& tableName,
		const OGRwkbGeometryType& geoType);

	/**
	 *
	 * @param q
	 * @return
	 */
	bool generateResult(tbb::concurrent_queue<GridScore>& q);

};

#endif //TRAFFIC_SCORE_GDB_GENERATOR_BASE_GENERATOR_H_
