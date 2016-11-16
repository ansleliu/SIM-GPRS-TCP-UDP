// TCPClientDlg.h : 头文件
//

#pragma once

#include "TCPClient_CE.h"

// CTCPClientDlg 对话框
class CTCPClientDlg : public CDialog
{
// 构造
public:
	CTCPClientDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_TCPCLIENT_DIALOG };


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
	afx_msg LONG OnClientDisconnect(WPARAM wParam,LPARAM lParam);
	DECLARE_MESSAGE_MAP()

private:
	//定义CTCPClient_CE对象
	CTCPClient_CE m_tcpClient;
	CTCPClient_CE m_tcpClient1;
	CTCPClient_CE m_tcpClient2;
public:
	CString m_remoteHost;
	DWORD m_remotePort;
private:
	//连接断开事件处理函数
	static void CALLBACK OnDisConnect(void* pOwner);
	//当有数据接收事件处理函数
	static void CALLBACK OnTcpRead(void* pOwner,const char * buf,DWORD dwBufLen );
	//Socket错误事件处理函数
	static void CALLBACK OnTcpError(void* pOwner,int nErrorCode);
public:
	afx_msg void OnBnClickedBtnConn();
	afx_msg void OnBnClickedBtnDisconn();
	afx_msg void OnBnClickedBtnSenddata();
private:
	//得到本地的IP地址
	CString GetLocalIP();
};
