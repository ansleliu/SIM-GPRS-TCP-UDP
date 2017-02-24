#pragma once

//定义串口接收数据函数类型
typedef void (CALLBACK* ONSERIESREAD)(void* pOwner /*父对象指针*/
									  ,BYTE* buf  /*接收到的缓冲区*/
									  ,DWORD dwBufLen /*接收到的缓冲区长度*/);
class CCESeries
{
public:
	CCESeries(void);
	~CCESeries(void);
public:
	//打开串口
	BOOL OpenPort(CWnd* pOwner,/*指向父指针*/
				  UINT portNo	= 1,		/*串口号*/
				  UINT baud		= 9600,	/*波特率*/
				  UINT parity	= NOPARITY, /*奇偶校验*/
				  UINT databits	= 8,		/*数据位*/
				  UINT stopbits	= 0        /*停止位*/
				  );
	//关闭串口
	void ClosePort();
	//同步写入数据
	BOOL WriteSyncPort(const BYTE*buf , DWORD bufLen);
	BOOL WriteChar(char *buf,int nsize);
	BOOL WriteChar(CString str);
	BOOL WriteChar(LPCTSTR lpString);
	//设置串口读取、写入超时
	BOOL SetSeriesTimeouts(COMMTIMEOUTS CommTimeOuts);
	//得到串口是否打开
	BOOL GetComOpened();
private:
    //串口读线程函数
    static  DWORD WINAPI ReadThreadFunc(LPVOID lparam);
	//关闭读线程
	void CloseReadThread();
private:
    //已打开的串口句柄
	HANDLE	m_hComm;
	//读线程句柄
	HANDLE m_hReadThread;
	//读线程ID标识
	DWORD m_dwReadThreadID;
	//读线程退出事件
	HANDLE m_hReadCloseEvent;
	BOOL m_bOpened; //串口是否打开
	CWnd* m_pOwner; //指定父对象指针
public:
	ONSERIESREAD m_OnSeriesRead; //串口读取回调函数
};
