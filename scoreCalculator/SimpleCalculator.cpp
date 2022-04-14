//
// Created by 黄铮 on 2022/4/7.
//

#include "SimpleCalculator.h"

using namespace std;

SimpleCalculator::~SimpleCalculator()
{
	for(auto pt : _stationPointList){
		delete pt;
	}
}

// 测试用， 生成随机分值
float SimpleCalculator::genScore(int min, int max){
	std::random_device rd;
	std::default_random_engine e {rd()};
	uniform_int_distribution<unsigned> u(min,max);

	return u(e);
}

float SimpleCalculator::calByStationCount()
{
	// 几个站点得几分
	return static_cast<float>(_stationCount * 5);
//	return genScore();
}

float simpleDistanceCal(double distance){
	float score = 0;
	if (distance <= 100){
		score = 5;
	}else if (distance > 100 && distance <= 200){
		score = 4;
	}else if (distance > 200 && distance <= 300){
		score = 3;
	}else if (distance > 300 && distance <= 400){
		score = 2;
	}else if (distance > 400 && distance <= 500){
		score = 1;
	}
	return score;
}

float SimpleCalculator::calByWalk2StationDistance()
{
	float score = 0;
	for(auto pt : _stationPointList){
		double d = _centPoint.Distance(pt);
		score += simpleDistanceCal(d);
	}
	return score;
//	return genScore();
}

float SimpleCalculator::calByWalk2StationTime()
{
	// 这里用的其实是，到路网的最小距离。 并不是到站点的时间
	return simpleDistanceCal(_minRoadDistance);
//	return genScore();
}

float SimpleCalculator::calByStationWeight()
{
//	return genScore();
	return 1;
}

float SimpleCalculator::cal(OGRLayer* pLayer, OGRPoint& centPoint,const double& minRoadDistance)
{
	_stationCount = static_cast<int>(pLayer->GetFeatureCount());
	_centPoint = centPoint;
	_minRoadDistance = minRoadDistance;

	pLayer->ResetReading();
	OGRFeature *poFeature;
	while( (poFeature = pLayer->GetNextFeature()) != nullptr )
	{
		OGRGeometry* pGeometry = poFeature->GetGeometryRef();
		_stationPointList.push_back(pGeometry->clone());
		OGRFeature::DestroyFeature( poFeature);
	}
	return (calByStationCount() + calByWalk2StationDistance() + calByWalk2StationTime() + calByStationWeight()) / 4 ;

}