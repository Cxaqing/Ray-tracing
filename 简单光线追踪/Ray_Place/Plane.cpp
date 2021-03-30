#include "pch.h"
#include "Plane.h"
#include <math.h>
#define ROUND(d) int(d + 0.5)

CPlane::CPlane(void)
{
}

CPlane::CPlane(CP3 LB, CP3 RB, CP3 RT)//平面的左下角点、右下角点和右上角点
{
	this->LB = LB;
	this->RB = RB;
	this->RT = RT;
	this->LT.x = LB.x;
	this->LT.y = RT.y;
	this->LT.z = RT.z;
	Normal = CrossProduct(CVector3(LB,RB), CVector3(LB, RT));//计算平面法向量
}

CPlane::~CPlane(void)
{
}

BOOL CPlane::Hit(CRay Ray, CIntersection &Hit)//计算光线与平面的交点
{
	CVector3 Vectora(LB);//定义左下角点的位置向量
	double A = DotProduct(Ray.Direction, Normal);
	double B = DotProduct(Ray.Origin - Vectora, Normal);
	if(0 != A)//光线不平行于平面
	{
		double t = -B / A;
		if(t > Epsilon)
		{
			Hit.t = t;
			Hit.Point = Ray.GetPoint(Hit.t);//交点坐标
			Hit.Normal = Normal;//交点的法向量
		}
		if(ROUND(Hit.Point.x) >= LB.x && ROUND(Hit.Point.x) <= RT.x
			&& ROUND(Hit.Point.z) >= RT.z && ROUND(Hit.Point.z) <= LB.z)
		{
			Hit.Material.SetAmbient(CRGB(0.5, 0.5, 0.0));//材质环境色
			Hit.Material.SetDiffuse(CRGB(0.5, 0.5, 0.0));//材质漫反射色
			Hit.Material.SetSpecular(CRGB(0.5, 0.5, 0.0));//材质镜面反射色

			Hit.Material.M_n = 50.0;//高光指数
			Hit.Material.M_eta = 0; //折射率
			return TRUE;
		}
	}
	return FALSE;
}