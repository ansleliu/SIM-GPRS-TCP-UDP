/*-----------------------------------------
* Copyright (c) 2008 Eric Wong
* 本版紧供读者参考，不得用于任何商业行为
*
* 文件名称： TCPClient_CE.h
* 文件标识： 
* 摘要：用于封装TCP 客户端通讯
*
* 当前版本： 1.0
* 作者： 汪兵 Eric Wong
* 完成日期： 2008年1月19日
*
* 取代版本：
* 原作者： 
* 完成日期： 
----------------------------------------*/
#pragma once

#include <winsock.h>
#pragma comment(lib,"winsock.lib")

//定义连接断开事件
typedef void (CALLBACK* ONDISCONNECT)(void* pOwner);
//定义当有数据接收事件
typedef void (CALLBACK* ONREAD)(void* pOwner,const char * buf,DWORD dwBufLen );
//定义Socket错误事件
typedef void (CALLBACK* ONERROR)(void* pOwner,int nErrorCode);

class CTCPClient_CE
{
public:
	CTCPClient_CE(void);
	~CTCPClient_CE(void);
public:
	//远程主机IP地址
	CString	 m_remoteHost;  
	//远程主机端口
    int      m_port;
	/*--以下客户端通讯事件--*/
	//连接断开事件，回调函数
	ONDISCONNECT OnDisConnect;
	//接收数据事件，回调函数
	ONREAD       OnRead;
	//发生错误事件，回调函数
	ONERROR      OnError;
private:
	//通讯Socket句柄
	SOCKET m_socket;      
	//通讯线程退出事件句柄
	HANDLE m_exitThreadEvent; 
	//通讯线程句柄
	HANDLE m_tcpThreadHandle;
	//父对象句柄
	void * m_pOwner;
	//接收缓冲区
	char m_recvBuf[4096];
private:
    //通讯线程函数
	static DWORD SocketThreadFunc(LPVOID lparam);
public:
	//用于打开客户端socket
	BOOL Open(void * pOwner);
public:
	//用于关闭客户端socket
	BOOL Close();
public:
	//用于建立与TCP服务器连接
	BOOL Connect();
public:
	//向服务器端发送数据
	BOOL SendData(const char * buf , DWORD dwBufLen);
};
