//
// Created by 黄铮 on 2022/4/6.
//

#include "OracleGenerator.h"

bool OracleGenerator::generateGdbFile() {
	try {


		string fmtStrSql = "select {} as wkt from {} where rownum=1";
		string fmtStrSelectSql = "select {} as wkt from {}";

//		soci::session sql(*_poolList[0]);
		soci::session sql(_backEnd, m_connString);
		createTempGdb(sql, fmtStrSql, fmtStrSelectSql);

		return true;
	}catch(exception& e){
		LOG4CPLUS_ERROR(MyLogger::getInstance(), "generateGdbFile error: " << e.what());
	}

	return false;
}

bool OracleGenerator::createResultTable()
{
	try
	{
//		soci::session sql(*_poolList[0]);
		soci::session sql(_backEnd, m_connString);
		string strSql =
			fmt::format("create table {} ({} INTEGER not null, WKT CLOB not null, SCORE NUMBER not null)",m_resultTable, idField);
		sql << strSql;
		LOG4CPLUS_INFO(MyLogger::getInstance(), strSql);
		LOG4CPLUS_INFO(MyLogger::getInstance(), "create table ok => " << m_resultTable);

		return true;
	}catch (const exception& e){
		LOG4CPLUS_ERROR(MyLogger::getInstance(), "createResultTable error: " << e.what());
	}

	return false;
}

bool OracleGenerator::batchInsert(vector<std::string>& sqlList)
{
	initDB();
	soci::session sql(*_poolList[0]);
	try
	{
		soci::session sql(_backEnd, m_connString);
		soci::transaction tr(sql);
		for (const auto& strSql : sqlList)
		{
			sql << strSql;
		}
		sqlList.clear();
		tr.commit();

		return true;
	}catch (const exception& e){
		LOG4CPLUS_ERROR(MyLogger::getInstance(), "batchInsert error: " << e.what());
	}

	return false;
}

void OracleGenerator::initDB()
{
	if (_poolList.empty()){
		shared_ptr<soci::connection_pool> pool(new soci::connection_pool(POOL_SIZE));
		for (int i = 0; i < POOL_SIZE; ++i)
		{
			soci::session &sql = pool->at(i);
			sql.open(_backEnd, m_connString);
		}
		_poolList.push_back(pool);
	}
}
