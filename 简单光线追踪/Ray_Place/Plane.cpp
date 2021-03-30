#include "pch.h"
#include "Plane.h"
#include <math.h>
#define ROUND(d) int(d + 0.5)

CPlane::CPlane(void)
{
}

CPlane::CPlane(CP3 LB, CP3 RB, CP3 RT)//ƽ������½ǵ㡢���½ǵ�����Ͻǵ�
{
	this->LB = LB;
	this->RB = RB;
	this->RT = RT;
	this->LT.x = LB.x;
	this->LT.y = RT.y;
	this->LT.z = RT.z;
	Normal = CrossProduct(CVector3(LB,RB), CVector3(LB, RT));//����ƽ�淨����
}

CPlane::~CPlane(void)
{
}

BOOL CPlane::Hit(CRay Ray, CIntersection &Hit)//���������ƽ��Ľ���
{
	CVector3 Vectora(LB);//�������½ǵ��λ������
	double A = DotProduct(Ray.Direction, Normal);
	double B = DotProduct(Ray.Origin - Vectora, Normal);
	if(0 != A)//���߲�ƽ����ƽ��
	{
		double t = -B / A;
		if(t > Epsilon)
		{
			Hit.t = t;
			Hit.Point = Ray.GetPoint(Hit.t);//��������
			Hit.Normal = Normal;//����ķ�����
		}
		if(ROUND(Hit.Point.x) >= LB.x && ROUND(Hit.Point.x) <= RT.x
			&& ROUND(Hit.Point.z) >= RT.z && ROUND(Hit.Point.z) <= LB.z)
		{
			Hit.Material.SetAmbient(CRGB(0.5, 0.5, 0.0));//���ʻ���ɫ
			Hit.Material.SetDiffuse(CRGB(0.5, 0.5, 0.0));//����������ɫ
			Hit.Material.SetSpecular(CRGB(0.5, 0.5, 0.0));//���ʾ��淴��ɫ

			Hit.Material.M_n = 50.0;//�߹�ָ��
			Hit.Material.M_eta = 0; //������
			return TRUE;
		}
	}
	return FALSE;
}