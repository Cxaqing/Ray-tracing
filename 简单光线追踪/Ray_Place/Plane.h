#pragma once
#include "GeometricObject.h"

class CPlane : public CGeometricObject//ƽ����
{
public:
	CPlane(void);
	virtual ~CPlane(void);
	CPlane(CP3 LB, CP3 RB, CP3 RT);//ƽ�湹�캯��
    BOOL Hit(CRay Ray, CIntersection &Hit);//������ײ��⺯��
public:
	CP3 LB,RT,RB,LT;//�ֱ�Ϊƽ������½ǵ㡢���Ͻǵ㡢���½ǵ㡢���Ͻǵ�
	CVector3 Normal;//ƽ��ķ�����
};