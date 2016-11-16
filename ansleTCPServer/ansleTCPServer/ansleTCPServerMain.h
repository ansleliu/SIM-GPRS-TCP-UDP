#pragma once
#include <winsock.h>
class ansleTCPCustom;
class ansleTCPServerMain;

//定义客户端连接建立事件
typedef void (CALLBACK* ONCLIENTCONNECT)(void* pOwner,ansleTCPCustom*);
//定义客户端SOCKET关闭事件
typedef void (CALLBACK* ONCLIENTCLOSE)(void* pOwner,ansleTCPCustom*);
//定义客户端当有数据接收事件
typedef void (CALLBACK* ONCLIENTREAD)(void* pOwner,ansleTCPCustom*,const char * buf,DWORD dwBufLen );
//定义客户端Socket错误事件
typedef void (CALLBACK* ONCLIENTERROR)(void* pOwner,ansleTCPCustom*,int nErrorCode);
//定义服务器端Socket错误事件
typedef void (CALLBACK* ONSERVERERROR)(void* pOwner,ansleTCPServerMain*,int nErrorCode);

class ansleTCPServerMain
{
public:
	ansleTCPServerMain(void);
	~ansleTCPServerMain(void);
	public:
  int m_LocalPort; //设置服务端口号
  void * m_pOwner;   //父对象句柄
private:
	SOCKET m_ServerSocket;     //TCP服务监听socket
	HANDLE m_serverThreadHandle;  //通讯线程句柄
	HANDLE m_exitThreadEvent;  //通讯线程退出事件句柄
public:  //定义事件
	//客户端连接建立事件，回调函数
	ONCLIENTCONNECT    OnClientConnect;
	//客户端连接断开事件，回调函数
	ONCLIENTCLOSE OnClientClose;
	//客户端接收数据事件，回调函数
	ONCLIENTREAD       OnClientRead;
	//客户端发生错误事件，回调函数
	ONCLIENTERROR      OnClientError;
	//服务器端发生错误事件,回调函数
	ONSERVERERROR	   OnServerError;
private:
	//TCP服务器监听线程函数
	static DWORD WINAPI SocketThreadFunc(PVOID lparam);
public:
	//删除客户端对象
	void RemoteClient(ansleTCPCustom *pClient /*客户端对象*/);
public:
	//打开TCP服务
	int Open();
public:
	//关闭TCP服务
	int Close();
public:
	//发送数据
	BOOL SendData(ansleTCPCustom* pCustom, const char * buf , DWORD dwBufLen);
};
