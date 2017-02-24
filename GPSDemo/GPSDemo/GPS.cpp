/*-----------------------------------------
* Copyright (c) 2008 Eric Wong
* 本版紧供读者参考，不得用于任何商业行为
*
* 文件名称： GPS.h
* 文件标识： 
* 摘要：用于封装GPS通讯协议
*
* 当前版本： 1.0
* 作者： 汪兵 Eric Wong
* 完成日期： 2008年1月29日
*
* 取代版本：
* 原作者： 
* 完成日期： 
----------------------------------------*/
#include "StdAfx.h"
#include "GPS.h"

//构造函数
CGPS::CGPS()
{
	m_gpsDev_State = GPS_DEV_NOTOPENED; //GPS状态
	m_hGpsThread = NULL;				//GPS检测线程句柄
	ZeroMemory(&m_gpsCurData,sizeof(m_gpsCurData));  //GPS当前数据
 	ZeroMemory(&m_gpsLastData,sizeof(m_gpsLastData)); //GPS上一次数据
}

//析构函数
CGPS::~CGPS(void)
{
}

/*
*函数介绍：打开GPS设备
*入口参数：pWnd	:使用此GPS类的窗体句柄
		   portNo		:串口号
		   baud			:波特率
		   parity		:奇偶校验
		   databits		:数据位
		   stopbits		:停止位
*出口参数：(无)
*返回值：TRUE:成功打开GPS设备;FALSE:打开GPS设备失败
*/
BOOL CGPS::Open(CWnd *pWnd , /*拥有者窗口句柄*/
				UINT portNo,		/*串口号*/
				UINT baud,	/*波特率*/
				UINT parity, /*奇偶校验*/
				UINT databits,		/*数据位*/
				UINT stopbits    /*停止位*/
				)
{
	m_pWnd = pWnd;  //储存窗口句柄
	//创建GPS检测线程退出事件
	m_hThreadQuitEvent = CreateEvent(NULL,false,false,L"EVENT_GPS_THREAD");
	//指定串口读回调函数
	m_ceSeries.m_OnSeriesRead = GpsOnSeriesRead;
	//打开GPS设备串口
	BOOL bResult = m_ceSeries.OpenPort(this,portNo,baud,parity,databits,stopbits);

	if (bResult)
	{
		//设置当前GPS状态
		m_gpsDev_State = GPS_DEV_OPENED;
		//发送GPS状态变化消息
		::PostMessage(m_pWnd->m_hWnd,WM_GPS_STATE_CHANGE_MESSAGE,WPARAM(GPS_DEV_OPENED),1);

		//创建GPS状态检测线程
		m_hGpsThread = CreateThread(NULL,0,GpsCheckThreadFunc,this,0,&m_dwGpsThreadID);
	}
	else
	{
		//设置当前GPS状态
		m_gpsDev_State = GPS_DEV_NOTOPENED;
		//发送GPS状态变化消息
		::PostMessage(m_pWnd->m_hWnd,WM_GPS_STATE_CHANGE_MESSAGE,WPARAM(GPS_DEV_NOTOPENED),1);
	}
	return bResult;
}

/*
*函数介绍：关闭GPS设备
*入口参数：(无)
*出口参数：(无)
*返回值：TRUE:成功关闭GPS设备;FALSE:关闭GPS设备失败
*/
void CGPS::Close()
{
	//先退出GPS检测线程
	if (m_hGpsThread != NULL)
	{
		//发送线程退出信号
		SetEvent(m_hThreadQuitEvent);
		//等待线程退出
		if (WaitForSingleObject(m_hGpsThread,1000) == WAIT_TIMEOUT)
		{
			TerminateThread(m_hGpsThread,0);
		}
	}

	m_hGpsThread = NULL;
	CloseHandle(m_hThreadQuitEvent);
	//将接收数据回掉函数置空
	m_ceSeries.m_OnSeriesRead = NULL;
	//关闭GPS串口
	m_ceSeries.ClosePort();
	//设置GPS状态
	m_gpsDev_State = GPS_DEV_NOTOPENED;
	//发送GPS状态变化消息
	::PostMessage(m_pWnd->m_hWnd,WM_GPS_STATE_CHANGE_MESSAGE,WPARAM(GPS_DEV_NOTOPENED),1);
}

/*
*函数介绍：获取GPS设备状态
*入口参数：(无)
*出口参数：(无)
*返回值：返回GPS设备状态
*/
GPSDEV_STATE CGPS::GetGpsState()
{
	return m_gpsDev_State;
}


/*
*函数介绍：得到当前GPS数据
*入口参数：(无)
*出口参数：(无)
*返回值：返回GPS设备当前GPS数据
*/
GPSData CGPS::GetCurGpsData()
{
	return m_gpsCurData;
}

/*--------------------------------------------------------------------
【函数介绍】: 在pArray缓冲区，查找subString字符串，如存在，返回当前位置，否则返回-1
【入口参数】: pArray:指定接收到的缓冲区队列
【出口参数】: pArray:指定接收到的缓冲区队列，解析后需要进行适当修改
【返回  值】: -1表示没有找到指定的子串，>=0表示发现第1个子串的位置
---------------------------------------------------------------------*/
int CGPS::Pos(LPCSTR subString , CByteArray * pArray,int iPos)
{
	//得到子串长度
	int subLen = strlen(subString);
	//得到缓冲区的长度
	int bufLen = pArray->GetUpperBound()+1;

	bool aResult = TRUE;
	//
	for ( int i=iPos;i<bufLen-subLen+1;i++)
	{
		aResult = TRUE;
		for (int j=0;j<subLen;j++)
		{
			if (pArray->GetAt(i+j) != *(subString + j))
			{
				aResult = FALSE;
				break;
			}
			int k = 0;
		}
		if (aResult)
		{
			return i;
		}
	}
	return -1;
}


/*
*函数介绍：判断是否存在有效GPS数据
*入口参数：aRecvStr ：缓冲数据
*出口参数：aRecvStr : 缓冲数据，outStr:得到的一个完整的GPS数据
*返回值：TRUE : 成功初始化 , FALSE : 初始化失败
*/
BOOL CGPS::HaveValidGPSData(CByteArray * pArray,CString &outStr)
{
	int tmpPos1,tmpPos2;

	tmpPos1 = Pos("$GPRMC",pArray,0);

	tmpPos2 = Pos("$GPRMC",pArray,tmpPos1+6);

	if (tmpPos2 >= 0)  //代表已包含两个$GPRMC
	{   
		if (tmpPos1 >= 0 )
		{
			BYTE *pBuf = pArray->GetData();
			char *sBuf = new char[tmpPos2-tmpPos1+1];
			ZeroMemory(sBuf,tmpPos2-tmpPos1+1);
			CopyMemory(sBuf,pBuf+tmpPos1,tmpPos2-tmpPos1+1);
			outStr = CString(sBuf);

			//释放内存
			delete[] sBuf;
			sBuf = NULL;
			pArray->RemoveAt(0,tmpPos2);
			return TRUE;
		}
	}
	return FALSE;
}

/*
*函数介绍：解析GPS数据
*入口参数：aRecvStr ：指待解析的GPS缓冲数据
*出口参数：(无)
*返回值：指CGPSData结构体的指针,如果无效即为：NULL;
*/
PGPSData CGPS::AnalyseGpsData(CString &aRecvStr)
{
	CString tmpTime;
	CString tmpState;
	CString tmpDate;
	CString tmpLONG;
	CString tmpLONGType;
	CString tmpLAT;
	CString tmpLATType;
	CString tmpSpeed;

	LPSTR pStrDate = NULL;
	LPSTR pStrTime = NULL;
	LPSTR pStrLong = NULL;
	LPSTR pStrLongType = NULL;
	LPSTR pStrLat = NULL;
	LPSTR pStrLatType = NULL;
	LPSTR pStrSpeed = NULL;

	PGPSData pGpsData = NULL;
	int tmpPos,tmpPos1;
	int len;

	tmpPos = aRecvStr.Find(',',0); //第1个值
	tmpPos1 = aRecvStr.Find(',',tmpPos+1);

	//得到时间
	tmpTime = aRecvStr.Mid(tmpPos+1,tmpPos1-tmpPos-1);
	tmpTime = tmpTime.Mid(0,2)+L":"+tmpTime.Mid(2,2)+L":"+tmpTime.Mid(4,2);

	len = tmpTime.GetLength();
	pStrTime = LPSTR(LocalAlloc(LMEM_ZEROINIT,len));
	WideCharToMultiByte(CP_ACP,WC_COMPOSITECHECK,tmpTime.GetBuffer(len),len
		,pStrTime,len ,NULL,NULL);

	//数据状态，是否有效
	tmpPos = aRecvStr.Find(',',tmpPos+1);  //第2个值
	tmpPos1 = aRecvStr.Find(',',tmpPos+1);
	tmpState = aRecvStr.Mid(tmpPos+1,tmpPos1-tmpPos-1);

	if (tmpState != 'A')//代表数据无效，返回
	{
		if (m_gpsDev_State != GPS_INVALID_DATA)
		{
			//设置GPS状态
			m_gpsDev_State = GPS_INVALID_DATA;
			//发送GPS状态变化消息
			::PostMessage(m_pWnd->m_hWnd,WM_GPS_STATE_CHANGE_MESSAGE,WPARAM(GPS_INVALID_DATA),1);
		}
		LocalFree(pStrTime);
		return NULL;
	}
	else  //代表数据有效
	{
		if (m_gpsDev_State != GPS_VALID_DATA)
		{
			//设置GPS状态
			m_gpsDev_State = GPS_VALID_DATA;
			//发送GPS状态变化消息
			::PostMessage(m_pWnd->m_hWnd,WM_GPS_STATE_CHANGE_MESSAGE,WPARAM(GPS_VALID_DATA),1);
		}
	}

	//得到纬度值
	tmpPos = aRecvStr.Find(',',tmpPos+1);//第3个值
	tmpPos1 = aRecvStr.Find(',',tmpPos+1);
	tmpLAT	= aRecvStr.Mid(tmpPos+1,tmpPos1-tmpPos-1);

	len = tmpLAT.GetLength();
	pStrLat = LPSTR(LocalAlloc(LMEM_ZEROINIT,len));
	WideCharToMultiByte(CP_ACP,WC_COMPOSITECHECK,tmpLAT.GetBuffer(len),len
		,pStrLat,len ,NULL,NULL);

	tmpPos = aRecvStr.Find(',',tmpPos+1);//第4个值
	tmpPos1 = aRecvStr.Find(',',tmpPos+1);
	tmpLATType = aRecvStr.Mid(tmpPos+1,tmpPos1-tmpPos-1);

	len = tmpLATType.GetLength();
	pStrLatType = LPSTR(LocalAlloc(LMEM_ZEROINIT,len));
	WideCharToMultiByte(CP_ACP,WC_COMPOSITECHECK,tmpLATType.GetBuffer(len),len
		,pStrLatType,len ,NULL,NULL);

	//得到经度值
	tmpPos = aRecvStr.Find(',',tmpPos+1);//第5个值
	tmpPos1 = aRecvStr.Find(',',tmpPos+1);
	tmpLONG = aRecvStr.Mid(tmpPos+1,tmpPos1-tmpPos-1);

	len = tmpLONG.GetLength();
	pStrLong = LPSTR(LocalAlloc(LMEM_ZEROINIT,len));
	WideCharToMultiByte(CP_ACP,WC_COMPOSITECHECK,tmpLONG.GetBuffer(len),len
		,pStrLong,len ,NULL,NULL);

	tmpPos = aRecvStr.Find(',',tmpPos+1);//第6个值
	tmpPos1 = aRecvStr.Find(',',tmpPos+1);
	tmpLONGType = aRecvStr.Mid(tmpPos+1,tmpPos1-tmpPos-1);

	len = tmpLONGType.GetLength();
	pStrLongType = LPSTR(LocalAlloc(LMEM_ZEROINIT,len));
	WideCharToMultiByte(CP_ACP,WC_COMPOSITECHECK,tmpLONGType.GetBuffer(len),len
		,pStrLongType,len ,NULL,NULL);

	//得到车速
	tmpPos = aRecvStr.Find(',',tmpPos+1);////第7个值
	tmpPos1 = aRecvStr.Find(',',tmpPos+1);
	tmpSpeed = aRecvStr.Mid(tmpPos+1,tmpPos1-tmpPos-1);

	len = tmpSpeed.GetLength();
	pStrSpeed = LPSTR(LocalAlloc(LMEM_ZEROINIT,len));
	WideCharToMultiByte(CP_ACP,WC_COMPOSITECHECK,tmpSpeed.GetBuffer(len),len
		,pStrSpeed,len ,NULL,NULL);

	tmpPos = aRecvStr.Find(',',tmpPos+1);////第8个值

	//得到日期
	tmpPos = aRecvStr.Find(',',tmpPos+1);////第9个值
	tmpPos1 = aRecvStr.Find(',',tmpPos+1);
	//格式化一下
	tmpDate = aRecvStr.Mid(tmpPos+1,tmpPos1-tmpPos-1);
	tmpDate = L"20"+tmpDate.Mid(4,2)+L"-"+tmpDate.Mid(2,2)+L"-"+tmpDate.Mid(0,2);

	len = tmpDate.GetLength();
	pStrDate = LPSTR(LocalAlloc(LMEM_ZEROINIT,len));
	WideCharToMultiByte(CP_ACP,WC_COMPOSITECHECK,tmpDate.GetBuffer(len),len
		,pStrDate,len ,NULL,NULL); 

	pGpsData = new GPSData();
	ZeroMemory(pGpsData,sizeof(GPSData));
	//得到GPS数据指针
	CopyMemory(pGpsData->date,pStrDate,10);
	CopyMemory(pGpsData->time,pStrTime,8);
	CopyMemory(pGpsData->latitude_type,pStrLatType,1);
	CopyMemory(pGpsData->latitude,pStrLat,9);
	CopyMemory(pGpsData->longitude_type,pStrLongType,1);
	CopyMemory(pGpsData->longitude,pStrLong,10);
	//先置默认速度0
	FillMemory(pGpsData->speed,5,'0');
	CopyMemory(pGpsData->speed,pStrSpeed,5);

	//释放内存
	LocalFree(pStrTime);
	LocalFree(pStrDate);
	LocalFree(pStrLatType);
	LocalFree(pStrLat);
	LocalFree(pStrLongType);
	LocalFree(pStrLong);
	LocalFree(pStrSpeed);

	return pGpsData;
}

//GPS接收数据事件
void CALLBACK CGPS::GpsOnSeriesRead(void * powner,BYTE* buf,DWORD  dwBufLen)
{
	CGPS * pGps = (CGPS*)powner;
	//得到本类指针
	CByteArray * pArray = &(pGps->m_aRecvBuf);

	//得到缓冲区大小
	int iMaxSize = pArray->GetSize();
	//得到缓冲区所使用的大小
	int iUpperBound = pArray->GetUpperBound();
	for (int i=0;i<dwBufLen;i++)
	{
		pArray->Add(*(buf+i));
	}

	//将收到的数据发给主程序显示出来
	char* pRecvBuf = new char[dwBufLen+1];
	ZeroMemory(pRecvBuf,dwBufLen+1);
	CopyMemory(pRecvBuf,buf,dwBufLen);

	//发送接收串口原始数据WINDOWS消息通知
	//消息处理完毕后，应释放内存
	::PostMessage(pGps->m_pWnd->m_hWnd,WM_GPS_RECV_BUF,WPARAM(pRecvBuf),dwBufLen+1);

	CString strGps;
	//检查是否已经存在有效的GPS数据
	if (pGps->HaveValidGPSData(pArray,strGps))
	{
		PGPSData pGpsData = NULL;
		pGpsData = pGps->AnalyseGpsData(strGps);
		if (pGpsData != NULL) 
		{
			//将接收到的GPS数据填充到最新当前数据
			pGps->m_gpsCurData = (*pGpsData);
			//发送接收有效GPS位置信息WINDOWS消息通知
			//由消息处理函数释放内存
			::PostMessage(pGps->m_pWnd->m_hWnd,WM_GPS_RECV_VALID_LONGLAT,WPARAM(pGpsData),0);
		}
	}
}

//检测GPS当前数据
DWORD WINAPI CGPS::GpsCheckThreadFunc(LPVOID lparam)
{
	//得到当前GPS指针
	CGPS *pGps = (CGPS*)lparam;

	int iRecCount = 0;
	//然后开始做循环检测，间隔为1秒
	while (TRUE)
	{
		//判断两次收到的时间是否相同
		if (strcmp(pGps->m_gpsCurData.time,pGps->m_gpsLastData.time) == 0)
		{
			//计数加1
			iRecCount++;
		}
		else
		{
			//将当前的GPS数据赋给历史值
			pGps->m_gpsLastData = pGps->m_gpsCurData;
			iRecCount = 0 ;
		}

		//代表连续三次没有收到数据
		if (iRecCount == 3)
		{
			if (pGps->m_gpsDev_State != GPS_NODATA)
			{
				//将GPS状态置为“无数据”
				pGps->m_gpsDev_State = GPS_NODATA;
				//发送GPS状态改变消息
				::PostMessage(pGps->m_pWnd->m_hWnd,WM_GPS_STATE_CHANGE_MESSAGE,WPARAM(GPS_NODATA),1);
			}
		}

		//延时1秒
		for (int i =0; i<10;i++)
		{
			//线程退出
			if (WaitForSingleObject(pGps->m_hThreadQuitEvent,100) == WAIT_OBJECT_0)
			{
				goto finish;
			}
		}
	}
finish:
	TRACE(L"GPS 检测线程退出\n");
	return 0;
}