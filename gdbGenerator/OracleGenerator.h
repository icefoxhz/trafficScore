//
// Created by 黄铮 on 2022/4/6.
//

#ifndef TRAFFICSCORE_GDBGENERATOR_ORACLEGENERATOR_H_
#define TRAFFICSCORE_GDBGENERATOR_ORACLEGENERATOR_H_

#include "BaseGenerator.h"
#include "iostream"
#include "string"
#include "soci/soci.h"
#include <soci/oracle/soci-oracle.h>
#include "fmt/format.h"
#include "../tools/MyMacros.h"

using namespace std;

class OracleGenerator: public BaseGenerator
{
 public:
	OracleGenerator(const string& resultTable,
		const string& sourceTable,
		const string& connString): BaseGenerator(resultTable, sourceTable, connString){}
 private:
	soci::backend_factory const& _backEnd = *soci::factory_oracle();
	// 连接池不好直接定义， 只能用个容易间接定义
	vector<shared_ptr<soci::connection_pool>> _poolList;

 private:
	void initDB();
 public:
	bool generateGdbFile() override;
	bool createResultTable() override;
	bool batchInsert(vector<std::string>& sqlList) override;
};

#endif //TRAFFICSCORE_GDBGENERATOR_ORACLEGENERATOR_H_
