//
// Created by 黄铮 on 2022/4/2.
//

#ifndef TRAFFIC_SCORE_GDB_OPERATOR_PG_GENERATOR_H_
#define TRAFFIC_SCORE_GDB_OPERATOR_PG_GENERATOR_H_

#include "BaseGenerator.h"
#include "iostream"
#include "string"
#include "soci/soci.h"
#include <soci/postgresql/soci-postgresql.h>
#include "fmt/format.h"

using namespace std;

class PgGenerator : public BaseGenerator
{
 public:
	PgGenerator(const string& resultTable,
		const string& sourceTable,
		const string& connString): BaseGenerator(resultTable, sourceTable, connString){}
 private:
	soci::backend_factory const& _backEnd = *soci::factory_postgresql();
	// 连接池不好直接定义， 只能用个容易间接定义
	vector<shared_ptr<soci::connection_pool>> _poolList;
	void initDB();
 public:
	bool generateGdbFile() override;
	bool createResultTable() override;
	bool batchInsert(vector<std::string>& sqlList) override;
};

#endif //TRAFFIC_SCORE_GDB_OPERATOR_PG_GENERATOR_H_
