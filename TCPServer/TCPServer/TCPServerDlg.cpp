// TCPServerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TCPServer.h"
#include "TCPServerDlg.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//定义TCP 收到客户端数据消息
#define WM_RECV_TCP_DATA WM_USER + 101
//定义TCP客户端连接消息
#define WM_TCP_CLIENT_CONNECT WM_USER + 102

// CTCPServerDlg 对话框

CTCPServerDlg::CTCPServerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTCPServerDlg::IDD, pParent)
	, m_localPort(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTCPServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDT_LOCALPORT, m_localPort);
}

BEGIN_MESSAGE_MAP(CTCPServerDlg, CDialog)
	ON_MESSAGE(WM_RECV_TCP_DATA,OnRecvTCPData)
	ON_MESSAGE(WM_TCP_CLIENT_CONNECT,OnClientConnect)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_LISTEN, &CTCPServerDlg::OnBnClickedBtnListen)
	ON_BN_CLICKED(IDC_BTN_CLOSE, &CTCPServerDlg::OnBnClickedBtnClose)
END_MESSAGE_MAP()


// CTCPServerDlg 消息处理程序

BOOL CTCPServerDlg::OnInitDialog()
{
	m_bFullScreen = FALSE; //禁止全屏
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	//设置默认值
	m_localPort = 5000;
	UpdateData(FALSE);

	
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}


//客户端连接建立事件处理函数
void CALLBACK  CTCPServerDlg::OnClientConnect(void* pOwner,CTCPCustom_CE* pTcpCustom)
{
	TCHAR *szAddress =NULL;
	DWORD dwBufLen = pTcpCustom->m_RemoteHost.GetLength() + 1;
	szAddress = new TCHAR[dwBufLen];
	ZeroMemory(szAddress,dwBufLen*2);
	//拷贝内存，得到客户端IP地址
	wcscpy(szAddress,pTcpCustom->m_RemoteHost);
	
	CTCPServerDlg *pThis = (CTCPServerDlg*)pOwner;
	
	//发送异步消息，表示有客户端连接，消息处理完后，需要释放内存
	pThis->PostMessage(WM_TCP_CLIENT_CONNECT,0,LPARAM(szAddress));
}

//客户端SOCKET关闭事件处理函数
void  CALLBACK CTCPServerDlg::OnClientClose(void* pOwner,CTCPCustom_CE* pTcpCustom)
{
	TCHAR *szAddress =NULL;
	DWORD dwBufLen = pTcpCustom->m_RemoteHost.GetLength() + 1;
	szAddress = new TCHAR[dwBufLen];
	ZeroMemory(szAddress,dwBufLen*2);
	//拷贝内存，得到客户端IP地址
	wcscpy(szAddress,pTcpCustom->m_RemoteHost);

	CTCPServerDlg *pThis = (CTCPServerDlg*)pOwner;
	
	//发送异步消息，表示有客户端连接，消息处理完后，需要释放内存
	pThis->PostMessage(WM_TCP_CLIENT_CONNECT,1,LPARAM(szAddress));

}

//服务器端收到来自客户端的数据
void CALLBACK CTCPServerDlg::OnClientRead(void* pOwner,CTCPCustom_CE* pTcpCustom,const char * buf,DWORD dwBufLen )
{
	DATA_BUF *pGenBuf = new DATA_BUF;
	char *pRecvBuf = NULL; //接收缓冲区
	//得到父对象指针
	CTCPServerDlg* pThis = (CTCPServerDlg*)pOwner;
	//将接收的缓冲区拷贝到pRecvBuf种
	pRecvBuf = new char[dwBufLen];
	CopyMemory(pRecvBuf,buf,dwBufLen);

	ZeroMemory(pGenBuf,sizeof(DATA_BUF));
	pGenBuf->dwBufLen = dwBufLen;
	pGenBuf->sBuf = pRecvBuf;
	
	//
	wcscpy(pGenBuf->szAddress,pTcpCustom->m_RemoteHost);

	//发送异步消息，表示收到TCP数据，消息处理完，应释放内存
	pThis->PostMessage(WM_RECV_TCP_DATA,WPARAM(pGenBuf),LPARAM(pTcpCustom));

   
}

//客户端Socket错误事件处理函数
void CALLBACK CTCPServerDlg::OnClientError(void* pOwner,CTCPCustom_CE* pTcpCustom,int nErrorCode)
{
	
}

//服务器端Socket错误事件处理函数
void CALLBACK CTCPServerDlg::OnServerError(void* pOwner,CTCPServer_CE* pTcpServer_CE,int nErrorCode)
{
	
}

//TCP接收数据处理函数
LONG CTCPServerDlg::OnRecvTCPData(WPARAM wParam,LPARAM lParam)
{
	DATA_BUF *pGenBuf = (DATA_BUF*)wParam; //通用缓冲区
	CTCPCustom_CE* pTcpCustom= (CTCPCustom_CE* )lParam; //TCP客户端对象
	//接收显示列表
	CListBox * pLstRecv = (CListBox*)GetDlgItem(IDC_LST_RECV);
	ASSERT(pLstRecv != NULL);
	//接收到的数据
	CString strRecv;
	CString strLen;
	strLen.Format(L"%d",pGenBuf->dwBufLen);
	strRecv = CString(pGenBuf->sBuf);
	
	pLstRecv->AddString(_T("************************************"));
	pLstRecv->AddString(_T("来自: ") + CString(pGenBuf->szAddress) );
	pLstRecv->AddString(_T("数据长度:")+strLen);
	pLstRecv->AddString(strRecv);

	//发送回应命令
	if (!m_tcpServer.SendData(pTcpCustom,"recv ok",strlen("recv ok")))
	{
		AfxMessageBox(_T("发送失败"));
	}

	//释放内存
	delete[] pGenBuf->sBuf;
	pGenBuf->sBuf = NULL;
	delete pGenBuf;
	pGenBuf = NULL;
	return 0;
}

//客户端连接断开消息函数
LONG CTCPServerDlg::OnClientConnect(WPARAM wParam,LPARAM lParam)
{
	int iIndex;
	TCHAR *szAddress = (TCHAR*)lParam;
	CString strAddrss = szAddress;
	
	CListBox * pLstConn = (CListBox*)GetDlgItem(IDC_LST_CONN);
	ASSERT(pLstConn != NULL);

	if (wParam == 0)
	{
		pLstConn->AddString(strAddrss + _T("建立连接"));
	}
	else
	{
		iIndex = pLstConn->FindString(iIndex,strAddrss + _T("建立连接"));
		if (iIndex != LB_ERR)
		{
			pLstConn->DeleteString(iIndex); 
		}
	}

	//释放内存
	delete[] szAddress;
	szAddress = NULL;
	return 0;
}


//监听
void CTCPServerDlg::OnBnClickedBtnListen()
{
	UpdateData(TRUE);
	//设置m_tcpServer属性
   	m_tcpServer.m_LocalPort = m_localPort;
	m_tcpServer.m_pOwner = this;
	m_tcpServer.OnClientConnect = OnClientConnect;
	m_tcpServer.OnClientClose = OnClientClose;
	m_tcpServer.OnClientRead = OnClientRead;
	m_tcpServer.OnClientError = OnClientError;
	m_tcpServer.OnServerError = OnServerError;
	if (m_tcpServer.Open() <= 0)
	{
		AfxMessageBox(_T("监听失败"));
		return;
	}
}


//关闭
void CTCPServerDlg::OnBnClickedBtnClose()
{
	CListBox * pLstConn = (CListBox*)GetDlgItem(IDC_LST_CONN);
	ASSERT(pLstConn != NULL);
	
	CListBox * pLstRecv = (CListBox*)GetDlgItem(IDC_LST_RECV);
	ASSERT(pLstRecv != NULL);
	
	//
	if (m_tcpServer.Close() <=0)
	{
		AfxMessageBox(_T("关闭TCP服务器失败"));
		return;
	}
	
	//清空列表
	pLstConn->ResetContent();
	pLstRecv->ResetContent();
}
