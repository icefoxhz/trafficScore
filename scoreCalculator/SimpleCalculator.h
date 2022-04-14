//
// Created by 黄铮 on 2022/4/7.
//

#ifndef TRAFFICSCORE_SCOREMODE_SIMPLECALCULATOR_H_
#define TRAFFICSCORE_SCOREMODE_SIMPLECALCULATOR_H_

#include "IScore.h"
#include <random>
#include <ctime>
#include <iomanip>
#include <ogrsf_frmts.h>
#include "../tools/CommonTools.h"

class SimpleCalculator : IScore
{
 public:
	virtual ~SimpleCalculator();
 public:
	float cal(OGRLayer* pLayer, OGRPoint& centPoint,const double& minRoadDistance);
	static float genScore(int min=1, int max=9);
 private:
	int _stationCount;
	OGRPoint _centPoint;
	double _minRoadDistance;
	std::vector<OGRGeometry*> _stationPointList;
 private:
	float calByStationCount() override;
	float calByWalk2StationDistance() override;
	float calByWalk2StationTime() override;
	float calByStationWeight() override;
};

#endif //TRAFFICSCORE_SCOREMODE_SIMPLECALCULATOR_H_
