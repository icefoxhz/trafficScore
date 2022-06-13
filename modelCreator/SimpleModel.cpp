//
// Created by 黄铮 on 2022/4/7.
//

#include "SimpleModel.h"
#include "../ScoreCalculator/SimpleCalculator.h"
#include "../tools/ThreadPool.h"

using namespace std;

bool SimpleModel::createLocalGdbFile()
{
	return _pGdbGenerator->generateGdbFile();
}

void SimpleModel::calAllGridScore()
{
	ThreadPool pool(THREAD_COUNT);
	std::atomic<size_t> current(0);
	std::atomic<size_t> percent(0);
	std::mutex lock;

	string roadLayerName = _pGdbGenerator->m_ch2enTableMapper.at(_pGdbGenerator->m_roadTable);
	int rowSize = (int)_wktVectorList.size();
	for(int i = 0 ; i < rowSize; ++i)
	{
		vector<std::string> colWktList = _wktVectorList[i];
		int colSize = (int)colWktList.size();

		// 从右往左
		bool firstIntersects = false;
		for (int j = colSize - 1; j >= 0; --j)
		{
			const auto& wkt = colWktList[j];

			// 一行里面第1个和路网相交的
			if (!firstIntersects)
			{
				auto* poDS = _pGdbGenerator->openDataset();
				OGRLayer* pLayer = poDS->GetLayerByName(roadLayerName.c_str());
				OGRGeometry* poGeometry;
				OGRGeometryFactory::createFromWkt(wkt.c_str(), nullptr, &poGeometry);
				pLayer->SetSpatialFilter(poGeometry);
				if (pLayer->GetFeatureCount() > 0)
				{
					firstIntersects = true;
				}else{
					current ++;

					printProgress(current, percent, lock);
				}
				GDALClose(poDS);
			}

			if (firstIntersects)
			{
				pool.enqueue([=, &current, &percent, &lock]
				{
				  GridScore gridScore{ wkt, doSpatialEngine(wkt) };
				  _resultQueue.push(gridScore);

				  string pro = to_string(i) + "/" + to_string(rowSize);
//				  printf("进度 %s\n", pro.c_str());

				  // 结束数据放入结果队列
				  if (i == rowSize - 1 && j == 0)
				  {
					  LOG4CPLUS_INFO(MyLogger::getInstance(), "全部格网计算结束");
					  GridScore gridScore{ "", -1 };
					  _resultQueue.push(gridScore);
				  }
				  current ++;

				  printProgress(current, percent, lock);
				});
			}
		}
	}
}

void SimpleModel::printProgress(const atomic<size_t>& current, atomic<size_t>& percent, std::mutex& lock) const
{
	auto d = (double)current;
	int p = (int)((d / (double)_dataCount) * 100);
	if (p > percent && p % 10 == 0){
		LOG4CPLUS_INFO(MyLogger::getInstance(), p << "%");
		lock.lock();
		percent = p;
		lock.unlock();
	}
}

float SimpleModel::doSpatialEngine(const string& wkt)
{
	float singleGridScore = 0;

	auto* poDS = _pGdbGenerator->openDataset();
	if (poDS == nullptr)
	{
		LOG4CPLUS_ERROR(MyLogger::getInstance(), "打开数据集失败！");
		return singleGridScore;
	}

	string roadLayerName = _pGdbGenerator->m_ch2enTableMapper.at(_pGdbGenerator->m_roadTable);
	OGRLayer* pLayer = poDS->GetLayerByName(roadLayerName.c_str());
	if (pLayer == nullptr)
	{
		LOG4CPLUS_ERROR(MyLogger::getInstance(), "打开图层失败！layer=" << roadLayerName);
		GDALClose(poDS);
		return singleGridScore;
	}

//	// 格网和路网相交了，这个格网符合要求
//	OGRGeometry* poGeometry;
//	OGRGeometryFactory::createFromWkt(wkt.c_str(), nullptr, &poGeometry);
//	pLayer->SetSpatialFilter(poGeometry);
//	if (pLayer->GetFeatureCount() > 0)
//	{
//		OGRGeometry* pBuffer = poGeometry->Buffer(_pGdbGenerator->m_bufferDistance);
//		double minRoadDistance = getCent2RoadMinDistance(pLayer, pBuffer);
//		singleGridScore = calScoreSingleGrid(pBuffer, minRoadDistance);
//	}

	OGRGeometry* poGeometry;
	OGRGeometryFactory::createFromWkt(wkt.c_str(), nullptr, &poGeometry);
	OGRGeometry* pBuffer = poGeometry->Buffer(_pGdbGenerator->m_bufferDistance);
	double minRoadDistance = getCent2RoadMinDistance(pLayer, pBuffer);
	singleGridScore = calScoreSingleGrid(pBuffer, minRoadDistance);

	GDALClose(poDS);
	return singleGridScore;
}


double SimpleModel::getCent2RoadMinDistance(OGRLayer* pLayer, OGRGeometry* pBuffer)
{
	double minDistance = 0;
	pLayer->ResetReading();
	OGRFeature *poFeature;
	while( (poFeature = pLayer->GetNextFeature()) != nullptr )
	{
		OGRGeometry* pGeometry = poFeature->GetGeometryRef();
		double d = pBuffer->Distance(pGeometry);
		minDistance = minDistance > d ? d : minDistance;
		OGRFeature::DestroyFeature( poFeature);
	}

	return minDistance;
}

float SimpleModel::calScoreSingleGrid(OGRGeometry* pBuffer, const double& minRoadDistance)
{
	float score = 0;
	for (const auto& tableName : _pGdbGenerator->m_stationTableList)
	{
		string layerName = _pGdbGenerator->m_ch2enTableMapper.at(tableName);
		score += calLayerScore(pBuffer, layerName, minRoadDistance);
	}
	// 平均
	score = score / static_cast<float>(_pGdbGenerator->m_stationTableList.size());

	// 保留一位小数
	return CommonTools::round(score, 1);
}

float SimpleModel::calLayerScore(OGRGeometry* pBuffer, const string& layerName, const double& minRoadDistance)
{
	float retScore = 0;
	auto* poDS = _pGdbGenerator->openDataset();
	if (poDS == nullptr)
	{
		LOG4CPLUS_ERROR(MyLogger::getInstance(), "打开数据集失败！");
		return retScore;
	}

	OGRLayer* pLayer = poDS->GetLayerByName(layerName.c_str());
	if (pLayer == nullptr)
	{
		GDALClose(poDS);
		return retScore;
	}

	pLayer->SetSpatialFilter(pBuffer);
	if (pLayer->GetFeatureCount() > 0)
	{
		OGRPoint centPoint;
		pBuffer->Centroid(&centPoint);

		SimpleCalculator calculator;
		retScore = calculator.cal(pLayer, centPoint, minRoadDistance);
	}

	GDALClose(poDS);

	return retScore;
}

bool SimpleModel::createWktGrid(bool createLayer)
{
	bool ret = true;
	auto* poDS = _pGdbGenerator->openDataset();
	if (poDS == nullptr)
	{
		LOG4CPLUS_ERROR(MyLogger::getInstance(), "打开数据集失败！");
		return false;
	}

	GDALDataset* poDSTmp = nullptr;
	try
	{
		string roadLayerName = _pGdbGenerator->m_ch2enTableMapper.at(_pGdbGenerator->m_roadTable);
		OGRLayer* pLayer = poDS->GetLayerByName(roadLayerName.c_str());
		if (pLayer == nullptr){
			LOG4CPLUS_ERROR(MyLogger::getInstance(), "打开图层失败！layer=" << roadLayerName);
			return false;
		}

		OGREnvelope envelope;
		if (pLayer->GetExtent(&envelope) != 0)
		{
			LOG4CPLUS_ERROR(MyLogger::getInstance(), "获取图层范围失败！");
			ret = false;
			goto __last;
		}

		double sideRealLen = _pGdbGenerator->m_gridSide;
		int rowNum = static_cast<int>((envelope.MaxY - envelope.MinY) / sideRealLen) + 1;
		int colNum = static_cast<int>((envelope.MaxX - envelope.MinX) / sideRealLen) + 1;
		LOG4CPLUS_INFO(MyLogger::getInstance(), "需要创建的格网数量: " << rowNum * colNum);

		// ----------------------
		OGRLayer* pGridLayer = nullptr;
		if (createLayer)
		{
			// 创建临时gdb
			string gridTempGdbName = "gird_" +  to_string(CommonTools::getTimeStamp()) ;
			poDSTmp = BaseGenerator::createTempGdbOnly(gridTempGdbName);
			_gridTempGdb = gridTempGdbName + ".gdb";
			if (poDSTmp == nullptr)
			{
				ret = false;
				goto __last;
			}

			string gridLayer = "grid";
			pGridLayer = poDSTmp->CreateLayer(gridLayer.c_str(), nullptr, wkbPolygon, nullptr);
			if (pGridLayer == nullptr)
			{
				LOG4CPLUS_ERROR(MyLogger::getInstance(), "创建图层失败！layer=" << gridLayer);
				ret = false;
				goto __last;
			}
		}
		// ----------------------
		// 左上角
		int pNow = 0;
		_dataCount = 0;
		for (int i = 0; i < rowNum; ++i)
		{
			bool firstIntersects = false;
			vector<string> wktVector;
			for (int j = 0; j < colNum; ++j)
			{
				string strWkt = fmt::format("POLYGON (({}, {}, {}, {}, {}))",
					to_string(envelope.MinX + sideRealLen * j) + " " + to_string(envelope.MinY + sideRealLen * i),
					to_string(envelope.MinX + sideRealLen * (j + 1)) + " " + to_string(envelope.MinY + sideRealLen * i),
					to_string(envelope.MinX + sideRealLen * (j + 1)) + " " + to_string(envelope.MinY + sideRealLen * (i + 1)),
					to_string(envelope.MinX + sideRealLen * j) + " " + to_string(envelope.MinY + sideRealLen * (i + 1)),
					to_string(envelope.MinX + sideRealLen * j) + " " + to_string(envelope.MinY + sideRealLen * i)
				);

				//---------------------
				if (createLayer)
				{
					OGRGeometry* poGeometry;
					OGRGeometryFactory::createFromWkt(strWkt.c_str(), nullptr, &poGeometry);
					OGRFeature* poFeature = OGRFeature::CreateFeature(pGridLayer->GetLayerDefn());
					poFeature->SetGeometry(poGeometry);
					if (pGridLayer->CreateFeature(poFeature) != OGRERR_NONE)
					{
						LOG4CPLUS_ERROR(MyLogger::getInstance(), "创建元素失败！");
						OGRFeature::DestroyFeature(poFeature);
						break;
					}

					OGRFeature::DestroyFeature(poFeature);
				}
				//-----------------------

				// 一行里面第1个和路网相交的
				if (!firstIntersects)
				{
					auto* poDS = _pGdbGenerator->openDataset();
					pLayer = poDS->GetLayerByName(roadLayerName.c_str());
					OGRGeometry* poGeometry;
					OGRGeometryFactory::createFromWkt(strWkt.c_str(), nullptr, &poGeometry);
					pLayer->SetSpatialFilter(poGeometry);
					if (pLayer->GetFeatureCount() > 0)
					{
						firstIntersects = true;
					}
					GDALClose(poDS);
				}

//				_wktQueue.push(strWkt);
				if (firstIntersects)
				{
					wktVector.push_back(strWkt);
					_dataCount ++;
				}
			}
			_wktVectorList.push_back(wktVector);

			int p = (int)(((float)(i + 1) / (float)rowNum) *100);
			if (p > pNow && p % 10 == 0)
			{
				LOG4CPLUS_INFO(MyLogger::getInstance(),  p << "%");
				pNow = p;
			}
		}
	}
	catch (const exception& e)
	{
		ret = false;
		LOG4CPLUS_ERROR(MyLogger::getInstance(), e.what());
	}

__last:
//	_wktQueue.push("stop");
	GDALClose(poDS);

	if (poDSTmp != nullptr)
	{
		GDALClose(poDSTmp);
	}

	return ret;
}

void SimpleModel::removeTempFiles()
{
	LOG4CPLUS_INFO(MyLogger::getInstance(), "开始删除临时文件" );

	string delCmd = fmt ::format("rmdir /s/q {}", _pGdbGenerator->GetTempGdbName());
	system(delCmd.c_str());

	if (!_gridTempGdb.empty())
	{
		delCmd = fmt::format("rmdir /s/q {}", _gridTempGdb);
		system(delCmd.c_str());
	}
}


void SimpleModel::createGdbByResult(tbb::concurrent_queue<GridScore>& q)
{
	GDALDataset* poDSTmp;
	// 创建临时gdb
	string gridTempGdbName = "result_" +  to_string(CommonTools::getTimeStamp()) ;
	poDSTmp = BaseGenerator::createTempGdbOnly(gridTempGdbName);
	string tempGdb = gridTempGdbName + ".gdb";
	if (poDSTmp == nullptr)
	{
		LOG4CPLUS_ERROR(MyLogger::getInstance(), "创建gdb失败" );
		return;
	}

	string gridLayer = "result_grid";
	OGRLayer* pGridLayer = poDSTmp->CreateLayer(gridLayer.c_str(), nullptr, wkbPolygon, nullptr);
	if (pGridLayer == nullptr)
	{
		LOG4CPLUS_ERROR(MyLogger::getInstance(), "创建图层失败！layer=" << gridLayer );
		return;
	}

	OGRFieldDefn oField( "score", OFTReal);
	oField.SetWidth(20);
	if( pGridLayer->CreateField( &oField ) != OGRERR_NONE )
	{
		LOG4CPLUS_ERROR(MyLogger::getInstance(), "创建score元素失败！" );
		return;
	}

	while (true)
	{
		GridScore gridScore;
		q.try_pop(gridScore);

		// 追加到尾部， 后面还要用~
		q.push(gridScore);

		if (gridScore.score == -1){
			break;
		}

		OGRGeometry* poGeometry;
		OGRGeometryFactory::createFromWkt(gridScore.wkt.c_str(), nullptr, &poGeometry);
		OGRFeature* poFeature = OGRFeature::CreateFeature(pGridLayer->GetLayerDefn());
		poFeature->SetGeometry(poGeometry);
		poFeature->SetField("score", gridScore.score);
		if (pGridLayer->CreateFeature(poFeature) != OGRERR_NONE)
		{
			LOG4CPLUS_ERROR(MyLogger::getInstance(), "创建geo元素失败！" );
			OGRFeature::DestroyFeature(poFeature);
			continue;
		}

		OGRFeature::DestroyFeature(poFeature);
	}

	GDALClose(poDSTmp);
}

bool SimpleModel::work()
{
	try
	{
		// 创建临时gdb
		if (!createLocalGdbFile())
		{
			return false;
		}
		LOG4CPLUS_INFO(MyLogger::getInstance(), "创建临时gdb完成！");

		// 创建wkt 格网
		if (!createWktGrid(false))
		{
			return false;
		}
		LOG4CPLUS_INFO(MyLogger::getInstance(), "创建格网wkt完成！");

		// 计算格网得分
		calAllGridScore();
		unsigned __int64 validGridCount = _resultQueue.unsafe_size() - 1;
		LOG4CPLUS_INFO(MyLogger::getInstance(), "计算完成 ! 有效格网数量: " << validGridCount);
		if (validGridCount <= 0)
		{
			LOG4CPLUS_ERROR(MyLogger::getInstance(), "没有有效格网");
			return false;
		}

		// 结果生成本地gdb文件
//		createGdbByResult(_resultQueue);

		// 结果入库
		if (!_pGdbGenerator->generateResult(_resultQueue))
		{
			return false;
		}

		LOG4CPLUS_INFO(MyLogger::getInstance(), "结果数据入库完成");

		// 删除临时文件
		removeTempFiles();
		return true;

	}catch (const exception& e){
		LOG4CPLUS_ERROR(MyLogger::getInstance(), e.what());
	}
	return false;
}