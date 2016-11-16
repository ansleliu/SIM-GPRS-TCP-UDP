// TCPServerDlg.h : 头文件
//

#pragma once

#include "TCPServer_CE.h"
#include "TCPCustom_CE.h"

//定义通用缓冲区
typedef struct  _DATA_BUF
{
	DWORD dwBufLen;
	char* sBuf;	
	TCHAR szAddress[MAX_PATH];
}DATA_BUF,*PDATA_BUF;

// CTCPServerDlg 对话框
class CTCPServerDlg : public CDialog
{
// 构造
public:
	CTCPServerDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_TCPSERVER_DIALOG };


	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	//TCP接收数据处理函数
	afx_msg LONG OnRecvTCPData(WPARAM wParam,LPARAM lParam);
	//客户端连接断开消息函数
	afx_msg LONG OnClientConnect(WPARAM wParam,LPARAM lParam);
	DECLARE_MESSAGE_MAP()
private:
	//定义CTCPServer_CE对象
	CTCPServer_CE m_tcpServer;

public:
	DWORD m_localPort;

private:
	//客户端连接建立事件处理函数
	static void CALLBACK	OnClientConnect(void* pOwner,CTCPCustom_CE *pTcpCustom);
	//客户端SOCKET关闭事件处理函数
	static void  CALLBACK OnClientClose(void* pOwner,CTCPCustom_CE*pTcpCustom);
	//服务器端收到来自客户端的数据
	static  void CALLBACK OnClientRead(void* pOwner,CTCPCustom_CE* pTcpCustom,const char * buf,DWORD dwBufLen );
	//客户端Socket错误事件处理函数
	static  void CALLBACK OnClientError(void* pOwner,CTCPCustom_CE* pTcpCustom,int nErrorCode);
	//服务器端Socket错误事件处理函数
	static void CALLBACK OnServerError(void* pOwner,CTCPServer_CE* pTcpServer_CE,int nErrorCode);

public:
	afx_msg void OnBnClickedBtnListen();
	afx_msg void OnBnClickedBtnClose();
};
