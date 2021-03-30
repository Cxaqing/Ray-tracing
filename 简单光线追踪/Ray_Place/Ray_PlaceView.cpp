
// Ray_PlaceView.cpp: CRayPlaceView 类的实现
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "Ray_Place.h"
#endif

#include "Ray_PlaceDoc.h"
#include "Ray_PlaceView.h"
#include <math.h>
#define  PI 3.14159265
#define ROUND(d) int(d + 0.5)

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CRayPlaceView

IMPLEMENT_DYNCREATE(CRayPlaceView, CView)

BEGIN_MESSAGE_MAP(CRayPlaceView, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CRayPlaceView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

// CRayPlaceView 构造/析构

CRayPlaceView::CRayPlaceView() noexcept
{
	// TODO: 在此处添加构造代码

	InitializeLightingScene();  //初始化光照场景
}

CRayPlaceView::~CRayPlaceView()
{
	delete pLight;
}

void CRayPlaceView::InitializeLightingScene(void)//初始化光照环境
{
	nLightSourceNumber = 1;//光源数量
	bkColor = CRGB(0.0, 0.0, 0.0);//设置屏幕背景色
	pLight = new CLighting(nLightSourceNumber);//一维光源动态数组
	pLight->LightSource[nLightSourceNumber - 1].SetPosition(0, 2000, -800);//设置光源位置
	for (int i = 0; i < nLightSourceNumber; i++)
	{
		pLight->LightSource[i].L_Diffuse = CRGB(1.0, 1.0, 1.0); //光源的漫反射颜色	
		pLight->LightSource[i].L_Specular = CRGB(1.0, 1.0, 1.0);//光源镜面高光颜色
		pLight->LightSource[i].L_C0 = 1.0;//常数衰减系数
		pLight->LightSource[i].L_C1 = 0.0000001;//线性衰减系数
		pLight->LightSource[i].L_C2 = 0.00000001;//二次衰减系数
		pLight->LightSource[i].L_OnOff = TRUE;//光源开启	
	}

	//设置绿色不透明球体材质
	Material[0].SetAmbient(CRGB(0.1, 0.1, 0.1));//环境反射率
	Material[0].SetDiffuse(CRGB(0.0, 0.508, 0.0));//环境光反射率和漫反射率相等
	Material[0].SetSpecular(CRGB(0.0, 0.508, 0.0));//镜面反射率
	Material[0].SetEmission(CRGB(0.0, 0.0, 0.0));//自发光
	Material[0].M_n = 50.0;//高光指数
	Material[0].M_eta = 0.0;//折射率等于零表示不透明

	//设置透明球体材质
	Material[1].SetAmbient(CRGB(0.2, 0.2, 0.2));//环境反射率
	Material[1].SetDiffuse(CRGB(0.2, 0.2, 0.2));//环境反射率和漫反射率相等
	Material[1].SetSpecular(CRGB(1.0, 1.0, 1.0));//镜面反射率
	Material[1].SetEmission(CRGB(0.0, 0.0, 0.0));//自发光
	Material[1].M_n = 100.0;//高光指数
	Material[1].M_eta = 1.1;//折射率

	//设置红色不透明圆环材质
	Material[2].SetAmbient(CRGB(0.5, 0.0, 0.0));//环境光反射率
	Material[2].SetDiffuse(CRGB(0.5, 0.0, 0.0));//漫反射率
	Material[2].SetSpecular(CRGB(1.0, 1.0, 1.0));//镜面反射率
	Material[2].SetEmission(CRGB(0.0, 0.0, 0.0));//材质自身发散的颜色
	Material[2].M_n = 30.0;//高光指数
	Material[2].M_eta = 0.0;//折射率等于零表示不透明
}

BOOL CRayPlaceView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CRayPlaceView 绘图

void CRayPlaceView::OnDraw(CDC* pDC)
{
	CRayPlaceDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码

	CRect rect;//定义客户区矩形
	GetClientRect(&rect);//获得客户区的信息
	nClientWidth = rect.Width();//获得客户区的宽度
	nClientHeight = rect.Height();//获得客户区的高度
	pDC->SetMapMode(MM_ANISOTROPIC);//pDC自定义坐标系
	pDC->SetWindowExt(rect.Width(), rect.Height());//设置窗口范围
	pDC->SetViewportExt(rect.Width(), -rect.Height());//x轴水平向右，y轴垂直向上
	pDC->SetViewportOrg(rect.Width() / 2, rect.Height() / 2);//客户区中心为原点
	rect.OffsetRect(-rect.Width() / 2, -rect.Height() / 2);
	pDC->FillSolidRect(rect, RGB(bkColor.red * 255, bkColor.green * 255, bkColor.blue * 255));
	nObjectNumber = 4;//物体总数
	int RecursionTimes = 5;//递归次数
	SetScene(pDC);//设置场景
	RenderScene(pDC, RecursionTimes);//渲染场景
	//RenderAntiAliasingScene(pDC, RecursionTimes);//渲染反走样场景_速度慢
	for (int i = 0; i < nObjectNumber; i++)
		delete pObjects[i];
}

void CRayPlaceView::SetScene(CDC* pDC)//设置场景函数
{
	//CP3 LeftBottom2(-500, -200, -1000), RightBottom2(500, -200, -1000), RightTop2(500, 400, -1000);//背景的左下角点，右下角点和右上角点
	//pObjects[1] = new CPlane(LeftBottom2, RightBottom2, RightTop2);//构造背景面（三点）
	//CP3 LeftBottom3(-500, 400, 0), RightBottom3(-500, -200, 0), RightTop3(-500, -200, -1000);//左墙的前上角点，前下角点和后下角点
	//pObjects[2] = new CPlane(LeftBottom3, RightBottom3, RightTop3);//构造左墙面（三点）
	//CP3 LeftBottom4(500, -200, 0), RightBottom4(500, 400, 0), RightTop4(500, 400, -1000);//右墙的前下角点，前上角点和后上角点
	//pObjects[3] = new CPlane(LeftBottom4, RightBottom4, RightTop4);//构造右墙面（三点）
	
	CP3 LeftBottom1(-500, -200, 0), RightBottom1(500, -200, 0), RightTop1(500, -200, -1000);//地面的左下角点，右下角点和右上角点
	pObjects[0] = new CPlane(LeftBottom1, RightBottom1, RightTop1);//构造平面（三点）
	int R1 = 120, R2 = 150;
	CP3	CenterPoint1(-50, -70, -500), CenterPoint2(120, -30, -300);
	pObjects[1] = new CSphere(R1, CenterPoint1, Material[0]);//构造不透明球体（半径，球心位置，材质）
	pObjects[2] = new CSphere(R2, CenterPoint2, Material[1]);//构造透明球体（半径，球心位置，材质）
	pObjects[3] = new CTorus(CP3(-160, -50, -300), 100, 30, Material[2]);//构造不透明圆环面（球心位置，主半径，辅半径，材质）
	PixelSize = 1;//新像素大小
	ViewPlane.SetResolution(ROUND(nClientWidth / PixelSize), ROUND(nClientHeight / PixelSize));
	R = 600;//视径
	zViewPlane = -200;//视平面在zw轴上的位置
	d = R - zViewPlane;//视距
	EyePoint = CP3(0, 0, R);//设置视点
}

void CRayPlaceView::RenderScene(CDC* pDC, int nDepth)//渲染场景函数
{
	for (int nRow = 0; nRow < ViewPlane.VerRes; nRow++)//行扫描
	{
		for (int nCol = 0; nCol < ViewPlane.HorRes; nCol++)//列扫描
		{
			CP2 Pixel(PixelSize*(nCol - ViewPlane.HorRes / 2 + 0.5), PixelSize*(nRow - ViewPlane.VerRes / 2 + 0.5));//屏幕像素中心
			CVector3 Direction(EyePoint, CP3(Pixel.x, Pixel.y, zViewPlane));//从视点到屏幕某一像素的向量为光线的方向向量
			Direction = Direction.Normalize();//规范化方向向量
			CVector3 Origin = CVector3(EyePoint);//将视点作为光线的起点
			CRay Ray(Origin, Direction);//定义光线
			CRGB PixelColor = RayTracing(Ray, nDepth);
			PixelColor.Normalize();//颜色规范化处理
			CPoint LBPoint(ROUND(Pixel.x - PixelSize / 2.0), ROUND(Pixel.y - PixelSize / 2.0));
			CPoint RTPoint(ROUND(Pixel.x + PixelSize / 2.0), ROUND(Pixel.y + PixelSize / 2.0));
			CRect PixelRect(LBPoint, RTPoint);
			pDC->FillSolidRect(PixelRect, RGB(PixelColor.red * 255, PixelColor.green * 255, PixelColor.blue * 255));
		}
	}
}

void CRayPlaceView::RenderAntiAliasingScene(CDC* pDC, int nDepth)//渲染反走样场景函数
{
	ViewPlane.SetSampleNumber(25);//像素采样数量，为平方级别
	int n = (int)sqrt((double)ViewPlane.SampleNumber);
	for (int nRow = 0; nRow < ViewPlane.VerRes; nRow++)//行扫描
	{
		for (int nCol = 0; nCol < ViewPlane.HorRes; nCol++)//列扫描
		{
			CP2 SPoint;
			CRGB SPointColor = bkColor;
			for (int p = 0; p < n; p++)//扫描子像素
			{
				for (int q = 0; q < n; q++)
				{
					SPoint.x = PixelSize * (nCol - ViewPlane.HorRes / 2 + (q + 0.5) / n);//子像素中心
					SPoint.y = PixelSize * (nRow - ViewPlane.VerRes / 2 + (p + 0.5) / n);
					CVector3 Direction(EyePoint, CP3(SPoint.x, SPoint.y, zViewPlane));//从视点到屏幕某一像素的向量为光线的方向向量
					Direction = Direction.Normalize();//规范化方向向量
					CVector3 Origin = CVector3(EyePoint);//将视点作为光线的起点
					CRay Ray(Origin, Direction);//定义光线
					SPointColor += RayTracing(Ray, nDepth);
				}
			}
			CPoint LBPoint(ROUND(SPoint.x - PixelSize / 2), ROUND(SPoint.y - PixelSize / 2));
			CPoint RTPoint(ROUND(SPoint.x + PixelSize / 2), ROUND(SPoint.y + PixelSize / 2));
			CRect PixelRect(LBPoint, RTPoint);
			CRGB PixelRectColor = SPointColor / ViewPlane.SampleNumber;//平均像素颜色
			PixelRectColor.Normalize();//颜色规范化处理
			pDC->FillSolidRect(PixelRect, RGB(PixelRectColor.red * 255, PixelRectColor.green * 255, PixelRectColor.blue * 255));
		}
	}
}

CRGB CRayPlaceView::RayTracing(CRay Ray, int nDepth)//光线跟踪函数
{
	if (!nDepth)//渲染深度为nDepth
		return bkColor;
	CIntersection MiniHit;//最小碰撞点
	MiniHit.t = HugeValue;
	for (int i = 0; i < nObjectNumber; i++)//访问所有物体
	{
		CIntersection hit;
		if (pObjects[i]->Hit(Ray, hit) && hit.t < MiniHit.t)
			MiniHit = hit;//保存最小碰撞点
	}
	if (MiniHit.t == HugeValue)//光线与物体没有碰撞点
		return bkColor;
	else
		return Shader(Ray, MiniHit, nDepth);//对最小碰撞点进行着色	
}

CRGB CRayPlaceView::Shader(CRay Ray, CIntersection  Hit, int nDepth)//着色函数
{
	CRGB LocalI(0.0, 0.0, 0.0), TotalI;//局部光强与总光强
	BOOL bShowShadow = TRUE;//阴影开关
	if (ShadowCast(Hit) && bShowShadow)
		LocalI += pLight->Ambient * Hit.Material.M_Ambient;//只有环境光
	else
		LocalI += pLight->Illuminate(EyePoint, Hit.Point, Hit.Normal, &Hit.Material);//局部光强
	double ks = 0.4, kt = 0.9;//ks为反射系数，kt为折射系数
	CRGB ReflectionI = RayTracing(Reflect(Ray, Hit), nDepth - 1); //反射光强
	CRGB RefractionI = RayTracing(Refract(Ray, Hit), nDepth - 1); //折射光强
	if (Hit.Material.M_eta)//物体透明
		TotalI = LocalI + ks * ReflectionI + kt * RefractionI;//考虑折射
	else
		TotalI = LocalI + ks * ReflectionI;//不考虑折射
	return TotalI;
}

CRay CRayPlaceView::Reflect(CRay IncidentLight, CIntersection Hit)//反射函数
{
	CRay ReflectionRay;//反射光线
	ReflectionRay.Origin = Hit.Point;//反射光线的源点
	CVector3 V(-IncidentLight.Direction);//入射光线向量
	CVector3 N = Hit.Normal.Normalize();//碰撞点法向量
	CVector3 R = 2.0 * N * fabs(DotProduct(N, V)) - V;//反射方向单位向量
	ReflectionRay.Direction = R;//反射光线的单位方向向量
	return ReflectionRay;
}

CRay CRayPlaceView::Refract(CRay IncidentLight, CIntersection Hit)//折射函数
{
	CRay RefractionRay;//折射光线
	RefractionRay.Origin = Hit.Point;//折射光线的源点
	Hit.Normal = Hit.Normal.Normalize();//规范化碰撞点的法向量
	CVector3 V(-IncidentLight.Direction);//入射光线向量
	double NdotV = DotProduct(Hit.Normal, V);//计算法向量与视向量的点积
	double Eta = Hit.Material.M_eta;//相对折射率
	if (NdotV < 0.0)
	{
		NdotV = -NdotV;
		Hit.Normal = -Hit.Normal;
		Eta = 1.0 / Eta;
	}
	double costheta2 = sqrt(1 - (1 - NdotV * NdotV) / (Eta * Eta));
	CVector3 T = IncidentLight.Direction / Eta - (costheta2 - NdotV / Eta) * Hit.Normal;//折射方向单位向量
	RefractionRay.Direction = T;//折射光线的单位方向向量	
	return RefractionRay;
}

BOOL CRayPlaceView::ShadowCast(CIntersection Hit)//阴影函数
{
	for (int i = 0; i < nLightSourceNumber; i++)
	{
		CVector3 SL(Hit.Point, pLight->LightSource[i].L_Position);// L为入射光向量
		nHittoLightSource = SL.Magnitude();//碰撞点到光源的距离
		SL = SL.Normalize();//规范化阴影光线的方向向量
		CRay ShadowRay(Hit.Point, SL);//阴影光线
		CIntersection ShadowHit;
		for (int j = 0; j < nObjectNumber; j++)//遍历场景中的各个物体
			pObjects[j]->Hit(ShadowRay, ShadowHit);
		if (ShadowHit.t < nHittoLightSource)
			return TRUE;
	}
	return FALSE;
}


// CRayPlaceView 打印

BOOL CRayPlaceView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CRayPlaceView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CRayPlaceView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}


// CRayPlaceView 诊断

#ifdef _DEBUG
void CRayPlaceView::AssertValid() const
{
	CView::AssertValid();
}

void CRayPlaceView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CRayPlaceDoc* CRayPlaceView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CRayPlaceDoc)));
	return (CRayPlaceDoc*)m_pDocument;
}
#endif //_DEBUG


// CRayPlaceView 消息处理程序


void CRayPlaceView::OnFilePrintPreview()
{
	// TODO: 在此添加命令处理程序代码
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}


void CRayPlaceView::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	// TODO: 在此处添加消息处理程序代码
//#ifndef SHARED_HANDLERS
//	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_MENU1, point.x, point.y, this, TRUE);
//#endif
}
