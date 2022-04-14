//
// Created by 黄铮 on 2022/4/7.
//

#ifndef TRAFFICSCORE_SCOREMODE_ISCORE_H_
#define TRAFFICSCORE_SCOREMODE_ISCORE_H_

class IScore
{
	/**
	 * 根据站点数量计算得分
	 * @return
	 */
	virtual float calByStationCount() = 0;

	/**
	 * 根据格网到站点的步行时间计算得分
	 * @return
	 */
	virtual float calByWalk2StationDistance() = 0;

	/**
	 * 根据格网到站点的步行时间计算得分
	 * @return
	 */
	virtual float calByWalk2StationTime() = 0;

	/**
	 * 根据站点权重计算得分
	 * @return
	 */
	virtual float calByStationWeight() = 0;

};

#endif //TRAFFICSCORE_SCOREMODE_ISCORE_H_
