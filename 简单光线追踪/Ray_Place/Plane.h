#pragma once
#include "GeometricObject.h"

class CPlane : public CGeometricObject//平面类
{
public:
	CPlane(void);
	virtual ~CPlane(void);
	CPlane(CP3 LB, CP3 RB, CP3 RT);//平面构造函数
    BOOL Hit(CRay Ray, CIntersection &Hit);//光线碰撞检测函数
public:
	CP3 LB,RT,RB,LT;//分别为平面的左下角点、右上角点、右下角点、左上角点
	CVector3 Normal;//平面的法向量
};