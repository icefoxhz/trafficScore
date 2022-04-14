#include <iostream>
#include "soci/soci.h"
#include "nlohmann/json.hpp"
#include <soci/postgresql/soci-postgresql.h>
#include "tools/CommonTools.h"
#include "gdbGenerator/OracleGenerator.h"
#include "gdbGenerator/PgGenerator.h"
#include "modelCreator/SimpleModel.h"
#include "ScoreCalculator/SimpleCalculator.h"
#include <string>

using namespace std;
using namespace nlohmann;

void testPg()
{
//    std::string connectString = "host=192.168.101.152 port=5432 dbname=kgtest user=postgres m_password='postgres'";
	std::string
		connectString = "host=10.230.0.66 port=5432 dbname=datamining user=cloud_user m_password='cgcloud#123_pg'";
	soci::backend_factory const& backEnd = *soci::factory_postgresql();
	soci::session sql(backEnd, connectString);

	int limit = 10;;
	soci::rowset<soci::row>
		rs = (sql.prepare << "select * from datafilter_1644558919171 limit :limit", soci::use(limit));
	for (auto& row : rs)
	{
		cout << "smid = " << row.get<int>("smid") << endl;
		cout << " name = " << row.get<string>("objname") << endl;
//        cout << " name = " << row.get<string>("name") << endl;
		cout << "---------------" << endl;
	}
}

void testSplit()
{
	string s = "a b c d";
	vector<std::string> vector = move(CommonTools::split(s, " "));
	for (auto it = vector.begin(); it != vector.end(); ++it)
	{
		cout << *it << endl;
	}
}

void testSimpleCalculator()
{
	for (int i = 0; i < 10; ++i)
	{
		cout << SimpleCalculator::genScore() << endl;
	}

}

int main(int argc, char* argv[])
{
	// 初始化日志模块
	log4cplus::Initializer initializer;
	log4cplus::PropertyConfigurator pc(LOG4CPLUS_TEXT("log4cplus.properties"));

	YAML::Node config = YAML::LoadFile("config.yaml");
	bool enableLog = config["enableLog"].as<bool>();
	if (enableLog)
	{
		pc.configure();
	}

	//	putenv("PGCLIENTENCODING=GBK");

	if (argc != 8){
		LOG4CPLUS_ERROR(MyLogger::getInstance(), "参数个数不正确！");
		exit(0);
	}

	for (int i = 0; i < argc; ++i)
	{
		LOG4CPLUS_INFO(MyLogger::getInstance(), "参数" << i << " = " << argv[i]);
	}

	// 连接串
	string connString = argv[1];
	// 结果表
	string resultTable = argv[2];
	// 路网表
	string roadTable = argv[3];
	// 公交表
	string busTable = argv[4];
	// 地铁表
	string metroTable = argv[5];
	// 格网大小
	string gridSize= argv[6];
	// 缓冲区大小
	string bufferDistance = argv[7];

	string sourceTable = fmt::format("{},{},{}", roadTable,busTable,metroTable);

	//-----------------
	auto startTime = std::chrono::steady_clock::now();  //获取开始时间

	LOG4CPLUS_INFO(MyLogger::getInstance(), ">>>>>>>>>>>>>>>>>>>>>>> 开始指定模型算子 <<<<<<<<<<<<<<<<<<<<<<<<<");

	// ------------------- oracle ---------------------------
//	OracleGenerator
//		generator("resulttb_score1",
//		"dl,dt,gj",
//		"service=192.168.101.153:1521/orcl user=gmsde password=gmsde");

//	OracleGenerator generator(resultTable,sourceTable,connString);
//	generator.m_gridSide = strtod(gridSize.c_str(), nullptr);
//	generator.m_bufferDistance = strtod(bufferDistance.c_str(), nullptr);
//	SimpleModel simpleModel(&generator);
//	bool success = simpleModel.work();

	// ------------------- pg ---------------------------

//	PgGenerator
//		generator("resulttb_score1",
//		"dl,dt,gj",
//		"host=10.230.0.66 port=5432 dbname=datamining user=cloud_user password='cgcloud#123_pg'");

	PgGenerator generator(resultTable, sourceTable, connString);
	generator.m_gridSide = strtod(gridSize.c_str(), nullptr);
	generator.m_bufferDistance = strtod(bufferDistance.c_str(), nullptr);
	SimpleModel simpleModel(&generator);
	bool success = simpleModel.work();

//	{
//		'result': 1,
//			'msg': '执行成功',
//			'dataType': 0      # 属性0  点1  线2  面3
//	}

	json jRst = {
		{"result", success ? 1 : 0},
		{"msg", success ? "success" : "fail"},
		{"dataType", 3}
	};
	cout << "myResult=" + jRst.dump() << endl;
	// --------------------------------------
	auto endTime = std::chrono::steady_clock::now();  //获取结束时间
	auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
	LOG4CPLUS_INFO(MyLogger::getInstance(), "总耗时: " << elapsedTime.count() / 1000 << " 秒");

	return 0;
}
