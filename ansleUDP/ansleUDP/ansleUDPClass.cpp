#include "StdAfx.h"
#include "ansleUDPClass.h"

ansleUDPClass::ansleUDPClass(void)
{
	m_UDPSocket = 0;                   	//UDP Socket通讯套接字
	m_pOwner = NULL;                  	//存储父对象句柄
	ZeroMemory(m_recvBuf,4096) ;		//接收数据缓冲区
	m_OnUdpError = NULL;				//UDP发生错误事件
	m_OnUdpRecv = NULL;					//UDP接收数据事件
}

ansleUDPClass::~ansleUDPClass(void)
{
}
/*
*函数介绍：打开UDP通讯端口
*入口参数：pOwner: 指定父对象指针 
           localPort: 指定远程UDP端口
           romoteHost:指定远程IP地址
           remotePort:指定远程UDP端口
*出口参数：(无)
*返回值：1代表成功；-1，-2，-3等都代表失败
*/
DWORD ansleUDPClass::Open(void* pOwner,int localPort,LPCTSTR remoteHost,int remotePort)
{
	WSADATA wsa;
	//传递父对象指针
	m_pOwner = pOwner;
	//加载winsock动态链接库
	if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
	{
		return -1;//代表失败
	}
	//创建UDP套接字
	m_UDPSocket = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
	if (m_UDPSocket == INVALID_SOCKET)
	{
		return -2;
	}
	
	//设置本地地址
	SOCKADDR_IN localAddr;
	localAddr.sin_family = AF_INET;
	localAddr.sin_port = htons(localPort);
	localAddr.sin_addr.s_addr=INADDR_ANY;
	
	//绑定地址
	if(bind(m_UDPSocket,(sockaddr*)&localAddr,sizeof(localAddr))!=0)
	{
		return -3;
	}
	
	//设置非堵塞通讯
	DWORD ul= 1;
	ioctlsocket(m_UDPSocket,FIONBIO,&ul);

	//创建一个线程退出事件
	m_ExitThreadEvent	= CreateEvent(NULL,TRUE,FALSE,NULL);
	
	//创建通讯线程
	AfxBeginThread(RecvThread,this);
	
	//设置对方地址
	m_RemoteAddr.sin_family = AF_INET;
	m_RemoteAddr.sin_port = htons(remotePort);
	//此处要将双字节转换成单字节
	char ansiRemoteHost[255];
	ZeroMemory(ansiRemoteHost,255);
	WideCharToMultiByte(CP_ACP,WC_COMPOSITECHECK,remoteHost,wcslen(remoteHost)
		,ansiRemoteHost,wcslen(remoteHost),NULL,NULL);
	m_RemoteAddr.sin_addr.s_addr=inet_addr(ansiRemoteHost);

	return 1;
}

/*
*函数介绍：关闭UDP通讯端口
*入口参数：(无)
*出口参数：(无)
*返 回 值：1代表成功；-1，-2等都代表失败
*/
DWORD ansleUDPClass::Close(void)
{
	//设置通讯线程退出事件，通知线程退出
	SetEvent(m_ExitThreadEvent);
	Sleep(1000);
	//关闭线程句柄
	CloseHandle(m_ExitThreadEvent);
	//关闭socket
	if (closesocket(m_UDPSocket) == SOCKET_ERROR)
	{
		return -1;
	}
	
	//释放socket资源
	if (WSACleanup() == SOCKET_ERROR)
	{
		return -2;
	}

	return 1;
}

/*
*函数介绍：发送数据
*入口参数：buf:缓冲区数据
           len:缓冲数据长度
*出口参数：(无)
*返回值：发送成功代表实际发送的字节数，否则返回-1
*/
DWORD ansleUDPClass::SendData(const char *buf, int len)
{
	int nBytes = 0; //每次发送出去的字节
	int nSendBytes=0;  //已经发送出去的字节
	int nErrorCode =0; //错误码
	
	//发送数据
	while (nSendBytes < len)
	{
		//发送数据
		nBytes = sendto(m_UDPSocket,buf+nSendBytes,len-nSendBytes,0,(sockaddr*)&m_RemoteAddr,sizeof(m_RemoteAddr));
		if (nBytes==SOCKET_ERROR )
		{
			nErrorCode = WSAGetLastError();
			if (m_OnUdpError)
			{
				m_OnUdpError(m_pOwner,nErrorCode);
			}
			return -1;
		}
		//全部发送完毕
		if (nSendBytes == len)
		{
			break;
		}
		//循环发送
		Sleep(500);
		nSendBytes = nSendBytes + nBytes;
	}
	return nSendBytes;
}

/*
*函数介绍：接收线程函数
*入口参数：lparam : 指传进线程的参数
*出口参数：(无)
*返 回 值：无意义。
*/
UINT ansleUDPClass::RecvThread(LPVOID lparam)
{
	//
	ansleUDPClass *pSocket = (ansleUDPClass*)lparam;
	fd_set fdRead;
	int ret;
	TIMEVAL	aTime;
	aTime.tv_sec = 1;
	aTime.tv_usec = 0;
	SOCKADDR_IN tmpAddr;
	int tmpRecvLen;
	int recvLen;
	int iErrorCode;
	
	while (TRUE)
	{
        //收到退出事件，结束线程
		if (WaitForSingleObject(pSocket->m_ExitThreadEvent,0) == WAIT_OBJECT_0)
		{
			break;
		}
		//将set初始化空集合
		FD_ZERO(&fdRead);
		//将pSocket->m_UDPSocket套接字添加到集合中
		FD_SET(pSocket->m_UDPSocket,&fdRead);
		//调用select函数，判断套接字I/O状态
		ret = select(0,&fdRead,NULL,NULL,&aTime);	
		if (ret == SOCKET_ERROR)
		{
			iErrorCode = WSAGetLastError();
			if (pSocket->m_OnUdpError)
			{
				pSocket->m_OnUdpError(pSocket->m_pOwner,iErrorCode);
			}
			break;
		}
		
		//有事件发生
		if (ret > 0)
		{
			//收到数据
			if (FD_ISSET(pSocket->m_UDPSocket,&fdRead))
			{		
				//
                tmpAddr.sin_family=AF_INET;             
                tmpAddr.sin_port = htons(pSocket->m_RemoteAddr.sin_port);
				tmpAddr.sin_addr.s_addr =INADDR_ANY;
                tmpRecvLen = sizeof(tmpAddr);

				//置空接收缓冲区
				ZeroMemory(pSocket->m_recvBuf,4096);
				//接收数据
				recvLen = recvfrom(pSocket->m_UDPSocket,pSocket->m_recvBuf, 4096,0,(SOCKADDR*)&tmpAddr,&tmpRecvLen); 
				if (recvLen == SOCKET_ERROR)
				{
					iErrorCode = WSAGetLastError();
					if (pSocket->m_OnUdpError)
					{
						pSocket->m_OnUdpError(pSocket->m_pOwner,iErrorCode);
					}
				}
				else if (recvLen == 0)
				{
					iErrorCode = WSAGetLastError();
					if (pSocket->m_OnUdpError)
					{
						pSocket->m_OnUdpError(pSocket->m_pOwner,iErrorCode);	
					}
				}
				else
				{
					//调用回调函数将数据发送出去
					if (pSocket->m_OnUdpRecv)
					{
						pSocket->m_OnUdpRecv(pSocket->m_pOwner,pSocket->m_recvBuf,recvLen,(SOCKADDR*)&tmpAddr);
					}
				}	

			}
		}
	}
	TRACE(L"The read thread had exited.\n");
	return 0;
}
