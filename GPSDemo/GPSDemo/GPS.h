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
#pragma once

#include "CESeries.h"

//定义GPS数据接收显示
#define WM_GPS_RECV_BUF				WM_USER + 101
//定义GPS状态改变消息
#define WM_GPS_STATE_CHANGE_MESSAGE	WM_USER + 102
//定义收到正确的GPS位置信息
#define WM_GPS_RECV_VALID_LONGLAT	WM_USER + 103

//定义GPS设备状态常量
enum GPSDEV_STATE
{
	GPS_VALID_DATA = 0,   //获取有效数据
	GPS_INVALID_DATA,//获取无效数据
	GPS_DEV_NOTOPENED,  //GPS串口未打开
	GPS_DEV_OPENED, //GPS串口已打开
	GPS_NODATA//GPS未收到数据
};

//GPS数据结构
typedef struct _GPSData
{
	char date[11] ; //Gps数据日期
	char time[9] ;  //Gps数据时间
	char latitude_type[2]; //纬度类型，北纬，南纬
	char latitude[10] ; //纬度值
	char longitude_type[2]; //经度类型，东经，西经
	char longitude[11] ;//经度值
	char speed[6];//速度
	char starNum; //卫星数目
}GPSData,*PGPSData;

class CGPS
{
public:
	CGPS(void);
	~CGPS(void);
public:
	//打开GPS设备
	BOOL Open(	   CWnd *pWnd , /*拥有者窗口句柄*/
				  UINT portNo	= 1,		/*串口号*/
				  UINT baud		= 9600,	/*波特率*/
				  UINT parity	= NOPARITY, /*奇偶校验*/
				  UINT databits	= 8,		/*数据位*/
				  UINT stopbits	= 0 );
	//关闭GPS设备
	void Close();
	//获取GPS设备状态
	GPSDEV_STATE GetGpsState();
	//得到当前GPS数据
	GPSData GetCurGpsData();

private:
	//在缓冲区中查找子字符串
	int Pos(LPCSTR subString , CByteArray * pArray,int iPos);
	//判断是否存在有效GPS数据
	BOOL HaveValidGPSData(CByteArray * pArray,/*分析的缓冲区队列*/
		CString &outStr);
	//解析GPS数据
	PGPSData AnalyseGpsData(CString &aRecvStr);
private:
	//串口接收数据回调函数
	static void CALLBACK GpsOnSeriesRead(void* pOwner,BYTE* buf,DWORD dwBufLen);
private:
	//GPS数据检测线程函数
	static DWORD WINAPI GpsCheckThreadFunc(LPVOID lparam);
private:
	GPSDEV_STATE m_gpsDev_State; //GPS当前设备状态
	GPSData  m_gpsCurData;       // GPS当前数据
	GPSData m_gpsLastData;		//GPS上一次数据
	CCESeries m_ceSeries;        //GPS串口通讯类
	CByteArray  m_aRecvBuf  ;   //接收缓冲区
	CWnd *m_pWnd; //存储主窗体句柄
	HANDLE m_hThreadQuitEvent;	//线程退出事件
	HANDLE m_hGpsThread;		//GPS检测线程句柄
	DWORD m_dwGpsThreadID;		//GPS检测线程标识
};
