#include "StdAfx.h"
#include "ansleTCPCustom.h"

ansleTCPCustom::ansleTCPCustom(void)
{
	m_TCPServer = NULL;
	m_RemoteHost = L"";
	m_RemotePort = 0;
	m_socket = 0;
}

ansleTCPCustom::~ansleTCPCustom(void)
{
}
DWORD ansleTCPCustom::SocketThreadFunc(PVOID lparam)
{
	ansleTCPCustom *pSocket;
	//得到CTCPCustom类实例指针
	pSocket = (ansleTCPCustom*)lparam;
	//定义读事件集合
	fd_set fdRead;  
	int ret;
	TIMEVAL	aTime;
	aTime.tv_sec = 1;
	aTime.tv_usec = 0;
	while (TRUE)
	{
		//收到退出事件，结束线程
		if (WaitForSingleObject(pSocket->m_exitThreadEvent,0) == WAIT_OBJECT_0)
		{
			break;
		}
		//置空读事件集合
		FD_ZERO(&fdRead);
		//给pSocket设置读事件
		FD_SET(pSocket->m_socket,&fdRead);
		//调用select函数，判断是否有读事件发生
		ret = select(0,&fdRead,NULL,NULL,&aTime);

		if (ret == SOCKET_ERROR)
		{
			if (pSocket->m_TCPServer->OnClientError)
			{
				//触发错误事件
				pSocket->m_TCPServer->OnClientError(pSocket->m_TCPServer->m_pOwner,pSocket,1);
			}
			//关闭socket
			closesocket(pSocket->m_socket);
			break;
		}

		if (ret > 0)
		{
			//判断是否读事件
			if (FD_ISSET(pSocket->m_socket,&fdRead))
			{
				char recvBuf[4096];
				int recvLen;
				ZeroMemory(recvBuf,4096);
				recvLen = recv(pSocket->m_socket,recvBuf, 4096,0); 
				if (recvLen == SOCKET_ERROR)
				{
					int nErrorCode = WSAGetLastError();
					//触发与客户端端连接的Socket错误
					if (pSocket->m_TCPServer->OnClientError)
					{
						pSocket->m_TCPServer->OnClientError(pSocket->m_TCPServer->m_pOwner,pSocket,nErrorCode);
					}
					//触发与客户端端连接的Socket关闭事件
					if (pSocket->m_TCPServer->OnClientClose)
					{
						pSocket->m_TCPServer->OnClientClose(pSocket->m_TCPServer->m_pOwner,pSocket);
					}
					//关闭socket
					closesocket(pSocket->m_socket);
					//移出客户端
					pSocket->m_TCPServer->RemoteClient(pSocket);
					break;

				}
				//表示连接已经从容关闭
				else if (recvLen == 0)
				{
					if (pSocket->m_TCPServer->OnClientClose)
					{
						pSocket->m_TCPServer->OnClientClose(pSocket->m_TCPServer->m_pOwner,pSocket);
					}
					//关闭socket
					closesocket(pSocket->m_socket);
					//移出客户端
					pSocket->m_TCPServer->RemoteClient(pSocket);
					break;
				}
				else
				{
					//触发与客户端端连接的Socket读事件
					if (pSocket->m_TCPServer->OnClientRead)
					{
						pSocket->m_TCPServer->OnClientRead(pSocket->m_TCPServer->m_pOwner,pSocket,recvBuf,recvLen);
					}
				}
			}
		}
	}
	TRACE(L"客户端线程退出\n");
	return 0;
}

/*-----------------------------------------------------------------
【函数介绍】: 打开socket，创建通讯线程
【入口参数】:  pTCPServer指向服务器端监听socket
【出口参数】:  (无)
【返回  值】:  TRUE:打开成功;FALSE:打开失败
------------------------------------------------------------------*/
BOOL ansleTCPCustom::Open(ansleTCPServerMain *pTCPServer)
{
	CString strEvent;
	strEvent.Format(L"EVENT_CLIENT_THREAD_EXIT %d",m_socket);
	//创建线程退出事件
	m_exitThreadEvent = CreateEvent(NULL,FALSE,FALSE,strEvent);

	//创建通讯线程
	m_tcpThreadHandle = CreateThread(NULL,0,SocketThreadFunc,this,0,NULL);
	if (m_tcpThreadHandle == NULL)
	{
		closesocket(m_socket);
		return FALSE;
	}
	//设置通讯模式为异步模式
	DWORD ul= 1;
	ioctlsocket(m_socket,FIONBIO,&ul);
	m_TCPServer = pTCPServer;
	return TRUE;
}

/*-----------------------------------------------------------------
【函数介绍】: 关闭socket，关闭线程，释放Socket资源
【入口参数】:  (无)
【出口参数】:  (无)
【返回  值】:  TRUE:成功关闭;FALSE:关闭失败
------------------------------------------------------------------*/
BOOL ansleTCPCustom::Close()
{
	//发送通讯线程结束事件
	SetEvent(m_exitThreadEvent);

	//等待1秒，如果读线程没有退出，则强制退出
	if (WaitForSingleObject(m_tcpThreadHandle,1000) == WAIT_TIMEOUT)
	{
		TerminateThread(m_tcpThreadHandle,0);
		TRACE(L"强制终止客户端线程\n");
	}
	m_tcpThreadHandle = NULL;
	//关闭句柄
	CloseHandle(m_exitThreadEvent);
	//关闭Socket，释放资源
	int err = closesocket(m_socket);
	if (err == SOCKET_ERROR)
	{
		return FALSE;
	}

	TRACE(L"客户端对象被成功关闭\n");
	return TRUE;
}

/*-----------------------------------------------------------------
【函数介绍】: 向客户端发送数据
【入口参数】: buf:待发送的数据
dwBufLen:待发送的数据长度
【出口参数】: (无)
【返回  值】: TRUE:发送数据成功;FALSE:发送数据失败
------------------------------------------------------------------*/
BOOL ansleTCPCustom::SendData(const char * buf , DWORD dwBufLen)
{
	int nBytes = 0;
	UINT nSendBytes=0;

	while (nSendBytes < dwBufLen)
	{
		nBytes = send(m_socket,buf+nSendBytes,dwBufLen-nSendBytes,0);
		if (nBytes==SOCKET_ERROR )
		{
			int iErrorCode = WSAGetLastError();
			//触发socket的Error事件
			if (m_TCPServer->OnClientError)
			{
				m_TCPServer->OnClientError(m_TCPServer->m_pOwner,this,iErrorCode);
			}
			//触发与服务器端断开连接事件
			if (m_TCPServer->OnClientClose)
			{
				m_TCPServer->OnClientClose(m_TCPServer->m_pOwner,this);
			}
			//关闭socket
			Close();
			return FALSE;
		}

		nSendBytes = nSendBytes + nBytes;

		if (nSendBytes < dwBufLen)
		{
			Sleep(1000);
		}
	} 
	return TRUE; 
}
