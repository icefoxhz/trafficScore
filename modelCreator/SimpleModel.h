//
// Created by 黄铮 on 2022/4/7.
//

#ifndef TRAFFICSCORE_MODELCREATOR_SIMPLEMODEL_H_
#define TRAFFICSCORE_MODELCREATOR_SIMPLEMODEL_H_

#include <iostream>
#include <queue>
#include <fmt/format.h>
#include "soci/soci.h"
#include "../gdbGenerator/BaseGenerator.h"

class SimpleModel
{
 public:
	explicit SimpleModel(BaseGenerator* pGdbGenerator): _pGdbGenerator(pGdbGenerator) {};
	virtual ~SimpleModel()= default;;
 private:
	BaseGenerator* _pGdbGenerator;
//	std::queue<std::string> _wktQueue;
	std::vector<std::vector<std::string>> _wktVectorList;
	tbb::concurrent_queue<GridScore> _resultQueue;
	const int THREAD_COUNT = 4;
	std::string _gridTempGdb;
	size_t _dataCount{};
 public:
	/**
	 * 创建本地缓存的gdb
	 * @return
	 */
	bool createLocalGdbFile();

	/**
	 * 创建格网的wkt字符串
	 * @return
	 */
	bool createWktGrid(bool createLayer=false);

	/**
	 * 空间算子
	 * @param wkt
	 * @return
	 */
	float doSpatialEngine(const std::string& wkt);

	/**
	 * 中心点到路网的最短距离
	 * @param pLayer
	 * @param pBuffer
	 * @return
	 */
	static double getCent2RoadMinDistance(OGRLayer* pLayer, OGRGeometry* pBuffer);

	/**
	 * 计算单个站点图层的分数
	 * @param pBuffer
	 * @param layerName
	 * @return
	 */
	float calLayerScore(OGRGeometry* pBuffer, const std::string& layerName, const double& minRoadDistance);

	/**
	 * 计算单个格网得分
	 * @param pBuffer
	 * @param minRoadDistance
	 * @return
	 */
	float calScoreSingleGrid(OGRGeometry* pBuffer, const double& minRoadDistance);

	/**
	 *  计算所有格网的得分
	 */
	void calAllGridScore();

//	OGRGeometry* createRoadJoin();

	/**
	 * 删除临时文件
	 */
	void removeTempFiles();

	static void createGdbByResult(tbb::concurrent_queue<GridScore>& q);

	/**
	 * 主调函数（入口）
	 */
	bool work();

	/**
	 * 打印进度
	 * @param current
	 * @param percent
	 * @param lock
	 * @return
	 */
	void printProgress(
		const std::atomic<unsigned long long int>& current,
		std::atomic<unsigned long long int>& percent,
		std::mutex& lock) const;
};

#endif //TRAFFICSCORE_MODELCREATOR_SIMPLEMODEL_H_
