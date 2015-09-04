#pragma once
#include "cv.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"


/* 人脸特征点的个数 */
#define FACE_FEATURE_POINT_NUM 68

/* 人脸轮廓特征点的个数1-17 */
#define FACE_CONTOUR_POINT_NUM 17 
/* 左眉毛特征点的个数18-22*/
#define LEFT_EYE_BROW_POINT_NUM 5
/* 右眉毛特征点的个数23-27 */
#define RIGHT_EYE_BROW_POINT_NUM 5
/* 鼻子特征点的个数28-36 */
#define NOSE_POINT_NUM 9
/* 左眼特征点的个数37-42 */
#define LEFT_EYE_POINT_NUM 6
/* 右眼特征点的个数43-48 */
#define RIGHT_EYE_POINT_NUM 6
/* 嘴特征点的个数49-68 */
#define MOUSE_POINT_NUM 20

/** 状态定义 */
typedef enum casSTATUS
{
	CAS_STATUS_CHECK_LICENSE_TIMEOUT = -3, /**< 验证license失败 */
	CAS_STATUS_CHECK_LICENSE_FAILED = -2,  /**< 验证license失败 */
	CAS_STATUS_READ_MAC_FAILED = -1,       /**< 读取MAC失败 */
	CAS_STATUS_OK = 1,                     /**< 成功 */
	CAS_STATUS_SKIP,                       /**< 跳过 */
	CAS_STATUS_FALSE,                      /**< 不支持 */
	CAS_STATUS_UNSUPPORT
} STATUS_E;

/* 循环队列的长度 */
#define  MAX_BLINK_QUEUE_NUM 256
/** 单个眨眼结构 */
typedef struct casBLINK_ELEM_S
{
	int          dist;                    /**< 上下眼睑的距离 */
	unsigned int u32Used;                 /**< 是否使用 */
}BLINK_ELEM_S;


/** 循环队列，存储眨眼距离，用来判断眨眼频率 */
typedef struct casBLINK_QUEUE_S
{
	BLINK_ELEM_S astElems[MAX_BLINK_QUEUE_NUM];    /**< 元素 */
	int          front;                            /**< 队头指针 */
	int          rear;                             /**< 队尾指针 */
	bool         full;                             /**< 队列已满标记 */
}BLINK_QUEUE_S;

/**
循环队列
当队首指针Q.ftont =MaxSiZe-1后，再前进一个位置就自动到0，这可以利用除法取余运算(%)来实现。
初始时：Q.front=Q.rear=0
队首指针进 1：Q.front=(Q.front+1)%MaxSize
队尾指针进 1：Q.rear=(Q.rear+1)%MaxSize
队列长度：(Q.rear+MaxSize-Q.front)%MaxSize
出队入队时：指针都按顺时针方向进1 
*/
/** 初始化 */
inline void InitBlinkQueue(BLINK_QUEUE_S &Q)
{
	Q.rear = Q.front = 0;  //初始化队首、队尾指针
	Q.full = false;
}

/** 判队空 */
inline bool isBlinkEmpty(BLINK_QUEUE_S Q) 
{
	if(Q.rear == Q.front) return true;  //队空条件
	else return false;
}
/** 判队空 */
inline void SetBlinkEmpty(BLINK_QUEUE_S &Q) 
{
	Q.rear = Q.front = 0;  //初始化队首、队尾指针
	Q.full = false;
	memset(Q.astElems,0,sizeof(BLINK_ELEM_S)*MAX_BLINK_QUEUE_NUM);

}
/** 入队，每次增加数据，队尾指针++，直到存满后，队尾指针重新指向队首 */
inline bool EnBlinkQueue(BLINK_QUEUE_S &Q, BLINK_ELEM_S x)
{
	if((Q.rear + 1) % MAX_BLINK_QUEUE_NUM == Q.front) //队满
	{
		Q.rear = Q.front; //队满后队尾指针指向队首位置
		Q.full = true;
	}
	Q.astElems[Q.rear] = x;
	Q.rear = (Q.rear + 1) % MAX_BLINK_QUEUE_NUM; //队尾指针加 1 取模
	return true;
}
/** 按照时序从当前往后获取count个数据 */
inline void GetElemBlinkQueue(BLINK_QUEUE_S &Q,int count, BLINK_ELEM_S QCount[])
{
	for (int i = 0; i < count; i++)
	{
		QCount[i] = Q.astElems[(Q.rear-count+MAX_BLINK_QUEUE_NUM)% MAX_BLINK_QUEUE_NUM+i];
	}
}
/** 出队 */
//inline bool DeBlinkQueue(BLINK_QUEUE_S &Q, BLINK_ELEM_S &x)
//{
//	if(Q.rear == Q.front) return false;  //队空，报错
//	x = Q.astElems[Q.front];
//	Q.front= (Q.front + 1) % MAX_BLINK_QUEUE_NUM;  //队头指针加 1 取模
//	return true;
//}
/** 队列中元素的个数：(Q.rear-Q.front+MaxSize)%MaxSize */
//inline bool GetBlinkQueueLength(BLINK_QUEUE_S &Q)
//{
//	return (Q.rear - Q.front + MAX_BLINK_QUEUE_NUM) % MAX_BLINK_QUEUE_NUM;
//}


/* 参数配置 */
typedef struct PERCLOSE_PARA_S
{
	int threshBlinkFreq;                        /**< 眨眼频率阈值，判断是否疲劳 */
}PERCLOSE_PARA;

/* 眨眼序列统计的参数 */
typedef struct PERCLOSE_STATISTICS_S
{
	int mean;                                   /**< 距离均值 */
}PERCLOSE_STATISTICS_S;

/* 人脸特征点分布 */
typedef struct FACE_FEATURE_POINT_S 
{
	CvPoint faceContour[FACE_CONTOUR_POINT_NUM]; /**< 人脸轮廓特征点的个数 */
	CvPoint leftBrow[LEFT_EYE_BROW_POINT_NUM];   /**< 左眉毛特征点的个数 */
	CvPoint rightBrow[RIGHT_EYE_BROW_POINT_NUM]; /**< 右眉毛特征点的个数 */
	CvPoint nose[NOSE_POINT_NUM];                /**< 鼻子特征点的个数 */
	CvPoint leftEye[LEFT_EYE_POINT_NUM];         /**< 左眼特征点的个数 */
	CvPoint rightEye[RIGHT_EYE_POINT_NUM];       /**< 右眼特征点的个数 */
	CvPoint mouse[MOUSE_POINT_NUM];              /**< 嘴特征点的个数 */
		
}FACE_FEATURE_POINT_S;


/* 眨眼频率统计 */
class CPerClose
{
public:
	PERCLOSE_PARA m_para;                        /**< perclose检测的参数 */
	FACE_FEATURE_POINT_S m_faceFeaturePoint;     /**< 人脸特征点 */
	float m_leftEyeBlinkFrq;                     /**< 左眼眨眼频率 */
	float m_rightEyeBlinkFrq;                    /**< 右眼眨眼频率 */
	float m_YawningFrq;                          /**< 打哈气频率 */
	float m_NodFrq;                              /**< 点头频率 */


	BLINK_QUEUE_S m_leftEyeBlinkQueue;            /**< 左眼眨眼统计循环队列 */
	BLINK_QUEUE_S m_rightEyeBlinkQueue;           /**< 右眼眨眼统计循环队列 */

	PERCLOSE_STATISTICS_S m_leftEyeStatistics;    /**< 左眼眨眼统计循环队列 */
	PERCLOSE_STATISTICS_S m_rightEyeStatistics;   /**< 右眼眨眼统计循环队列 */
	
	long m_frameNum;                              /**< 帧数统计用来计算时间和频率 */
	int m_timePerFrame;                           /**< 每帧的时间 */
	bool m_fatigueFlag;                           /**< 是否疲劳的标记 */
public:

	/* 分配内存 */
	STATUS_E Create();
	/* 配置数据 */
	STATUS_E Config(PERCLOSE_PARA para);
	/* 处理函数 */
	STATUS_E Process(CvPoint featurePoint[FACE_FEATURE_POINT_NUM]);
	/* 释放内存 */
	STATUS_E Release();

	/* 数据复制 */
	STATUS_E FeaturePointCopy(CvPoint featurePoint[]);
	/* 左眼眨眼检测 */
	STATUS_E LeftEyeBlinkDetect();
	/* 右眼眨眼检测 */
	STATUS_E RightEyeBlinkDetect();
	/* 眨眼检测 */
	STATUS_E BlinkDetect();
	/* 打哈气检测 */
	STATUS_E YawningDetect();
	/* 点头检测 */
	STATUS_E NodDetect();
	/* 眨眼频率统计 */
	STATUS_E BlinkFrequency();
	/* 疲劳判断 */
	STATUS_E FatigueJudgment();

	/* 构造函数 */
	CPerClose(void);
	/* 析构函数 */
	~CPerClose(void);

	
};

