#include "perclose.h"

/** 快速求开方 */
float IVA_sqrt(float number)
{ 
	long i;
	float x, y;
	const float f = 1.5F;
	x = number * 0.5F;
	y = number;
	i = * ( long * ) &y;
	i = 0x5f3759df - ( i >> 1 );

	y = * ( float * ) &i;
	y = y * ( f - ( x * y * y ) );
	y = y * ( f - ( x * y * y ) );
	return number * y;
}
int DistofTowPoint(CvPoint p1,CvPoint p2)
{
	//return (sqrt((p2.y-p1.y)*(p2.y-p1.y)+(p2.x-p1.x)*(p2.x-p1.x)));
	//取近似值
	int dist = (int)IVA_sqrt((p2.y-p1.y)*(p2.y-p1.y)+(p2.x-p1.x)*(p2.x-p1.x));
	return dist;
}
CPerClose::CPerClose(void)
{

}


CPerClose::~CPerClose(void)
{

}


/* 分配内存 */
STATUS_E CPerClose::Create()
{
	m_frameNum = 0;
	m_fatigueFlag = false;

	SetBlinkEmpty(m_leftEyeBlinkQueue);
	SetBlinkEmpty(m_rightEyeBlinkQueue);

	return CAS_STATUS_OK;
}
/* 配置数据 */
STATUS_E CPerClose::Config(PERCLOSE_PARA para)
{
	m_para = para;
	return CAS_STATUS_OK;
}
/* 处理函数 */
STATUS_E CPerClose::Process(CvPoint featurePoint[FACE_FEATURE_POINT_NUM])
{	
	FeaturePointCopy(featurePoint); /* 特征点数据赋值 */

	BlinkDetect();                  /* 眨眼统计 */
	
	YawningDetect();                /* 打哈气统计 */
	
	NodDetect();                    /* 点头统计 */
	
	BlinkFrequency();               /* 眨眼频率统计 */
	
	FatigueJudgment();              /* 疲劳判断 */

	m_frameNum++;                   //帧数增加

	return CAS_STATUS_OK;
}
/* 释放内存 */
STATUS_E CPerClose::Release()
{
	return CAS_STATUS_OK;
}

/* 数据复制 */
STATUS_E CPerClose::FeaturePointCopy(CvPoint featurePoint[])
{
	int i,j;
	//根据点的排列，分别赋值到m_faceFeaturePoint，类内都处理m_faceFeaturePoint
	for (i= 0,j = 0;j < FACE_CONTOUR_POINT_NUM; i++,j++)
	{
		m_faceFeaturePoint.faceContour[j] = featurePoint[i];
	}
	for (j = 0;j < LEFT_EYE_BROW_POINT_NUM; i++,j++)
	{
		m_faceFeaturePoint.leftBrow[j] = featurePoint[i];
	}
	for (j = 0;j < RIGHT_EYE_BROW_POINT_NUM; i++,j++)
	{
		m_faceFeaturePoint.rightBrow[j] = featurePoint[i];
	}
	for (j = 0;j < NOSE_POINT_NUM; i++,j++)
	{
		m_faceFeaturePoint.nose[j] = featurePoint[i];
	}
	for (j = 0;j < LEFT_EYE_POINT_NUM; i++,j++)
	{
		m_faceFeaturePoint.leftEye[j] = featurePoint[i];
	}
	for (j = 0;j < RIGHT_EYE_POINT_NUM; i++,j++)
	{
		m_faceFeaturePoint.rightEye[j] = featurePoint[i];
	}
	for (j = 0;j < MOUSE_POINT_NUM; i++,j++)
	{
		m_faceFeaturePoint.mouse[j] = featurePoint[i];
	}

	return CAS_STATUS_OK;
}
/** 
左眼眨眼检测 
左眼特征点的个数37-42
   38 39
37       40
   42 41
   1  2 
0       3
   5  4
检测方法：
1、di=abs(38+39-41-42)/2
2、把di存入队列，统计di
*/
STATUS_E CPerClose::LeftEyeBlinkDetect()
{
	BLINK_ELEM_S blinkElem;
	CvPoint *leftEyePoints = m_faceFeaturePoint.leftEye;

	blinkElem.dist = (DistofTowPoint(leftEyePoints[1],leftEyePoints[5])
	                 +DistofTowPoint(leftEyePoints[2],leftEyePoints[4])
					 ) >>1;
	EnBlinkQueue(m_leftEyeBlinkQueue,blinkElem);

	return CAS_STATUS_OK;
}
/** 
右眼眨眼检测 
右眼特征点的个数43-48
   44 45
43       46
   48 47
   1  2 
0       3
   5  4
*/
STATUS_E CPerClose::RightEyeBlinkDetect()
{
	BLINK_ELEM_S blinkElem;

	CvPoint *rightEyePoints = m_faceFeaturePoint.rightEye;

	blinkElem.dist = (DistofTowPoint(rightEyePoints[1],rightEyePoints[5])
		             +DistofTowPoint(rightEyePoints[2],rightEyePoints[4])
		              ) >>1;

	EnBlinkQueue(m_rightEyeBlinkQueue,blinkElem);

	//统计队列中的数据
	if (m_rightEyeBlinkQueue.full)//队列满了后开始统计
	{
		//均值
		int mean = 0;
		for (int i = 0; i < MAX_BLINK_QUEUE_NUM; i++)
		{
			mean += m_rightEyeBlinkQueue.astElems[(m_rightEyeBlinkQueue.rear+i)% MAX_BLINK_QUEUE_NUM].dist;
		}
		mean /= MAX_BLINK_QUEUE_NUM;
		
		m_leftEyeStatistics.mean = mean;
	}
	

	return CAS_STATUS_OK;
}

/* 眨眼检测 */
STATUS_E CPerClose::BlinkDetect()
{
	LeftEyeBlinkDetect();
	RightEyeBlinkDetect();

	//对队列进行统计
	//MAX_BLINK_QUEUE_NUM

	return CAS_STATUS_OK;
}
/* 打哈气检测 */
STATUS_E CPerClose::YawningDetect()
{
	return CAS_STATUS_OK;
}
/* 点头检测 */
STATUS_E CPerClose::NodDetect()
{
	return CAS_STATUS_OK;
}
/* 眨眼频率统计 */
STATUS_E CPerClose::BlinkFrequency()
{
	return CAS_STATUS_OK;
}
/* 疲劳判断 */
STATUS_E CPerClose::FatigueJudgment()
{
	return CAS_STATUS_OK;
}