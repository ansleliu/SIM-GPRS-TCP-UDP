// TCPClientDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TCPClient.h"
#include "TCPClientDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//定义TCP 客户端接收到数据消息
#define WM_RECV_TCP_DATA WM_USER + 101
//定义TCP客户端连接断开消息
#define WM_TCP_CLIENT_DISCONNECT WM_USER + 102

// CTCPClientDlg 对话框

CTCPClientDlg::CTCPClientDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTCPClientDlg::IDD, pParent)
	, m_remoteHost(_T(""))
	, m_remotePort(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTCPClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDT_REMOTEHOST, m_remoteHost);
	DDX_Text(pDX, IDC_EDT_REMOTEPORT, m_remotePort);


}

BEGIN_MESSAGE_MAP(CTCPClientDlg, CDialog)

	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_RECV_TCP_DATA,OnRecvTCPData)
	ON_MESSAGE(WM_TCP_CLIENT_DISCONNECT,OnClientDisconnect)
	ON_BN_CLICKED(IDC_BTN_CONN, &CTCPClientDlg::OnBnClickedBtnConn)
	ON_BN_CLICKED(IDC_BTN_DISCONN, &CTCPClientDlg::OnBnClickedBtnDisconn)
	ON_BN_CLICKED(IDC_BTN_SENDDATA, &CTCPClientDlg::OnBnClickedBtnSenddata)
END_MESSAGE_MAP()


// CTCPClientDlg 消息处理程序

BOOL CTCPClientDlg::OnInitDialog()
{
	m_bFullScreen = FALSE; //禁止全屏
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	//初始化输入值
	m_remoteHost = GetLocalIP();
	m_remotePort = 5000;
	UpdateData(FALSE);

	
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

//连接断开事件
void CALLBACK CTCPClientDlg::OnDisConnect(void* pOwner)
{
	//得到父对象指针
	CTCPClientDlg* pThis = (CTCPClientDlg*)pOwner;
	//发送消息表示客户端连接断开
	pThis->PostMessage(WM_TCP_CLIENT_DISCONNECT,0,0);
}

//数据接收事件
void CALLBACK CTCPClientDlg::OnTcpRead(void* pOwner,const char * buf,DWORD dwBufLen )
{
	BYTE *pRecvBuf = NULL; //接收缓冲区
	//得到父对象指针
	CTCPClientDlg* pThis = (CTCPClientDlg*)pOwner;
	//将接收的缓冲区拷贝到pRecvBuf种
	pRecvBuf = new BYTE[dwBufLen];
	CopyMemory(pRecvBuf,buf,dwBufLen);

	//发送异步消息，表示收到TCP数据，消息处理完，应释放内存
	pThis->PostMessage(WM_RECV_TCP_DATA,WPARAM(pRecvBuf),dwBufLen);

}

//Socket错误事件
void CALLBACK CTCPClientDlg::OnTcpError(void* pOwner,int nErrorCode)
{
	TRACE(L"客户端socket发生错误");
}

//TCP接收数据处理函数
LONG CTCPClientDlg::OnRecvTCPData(WPARAM wParam,LPARAM lParam)
{
	CString strOldRecv = L"";
	CString strRecv = L"";
	//接收到的BUF
	CHAR *pBuf = (CHAR*)wParam;
	//接收到的BUF长度
	DWORD dwBufLen = lParam;
	//接收框
	CEdit *pEdtRecvMsg = (CEdit*)GetDlgItem(IDC_EDT_RECV);
	ASSERT(pEdtRecvMsg != NULL);

	//得到接收框中的历史文本
	pEdtRecvMsg->GetWindowTextW(strOldRecv);
	//
	strRecv = CString(pBuf);
	//将新接收到的文本添加到接收框中
	strOldRecv = strOldRecv + strRecv + L"\r\n";
	pEdtRecvMsg->SetWindowTextW(strOldRecv);

	//释放内存
	delete[] pBuf;
	pBuf = NULL;
	return 0;
}

//客户端连接断开消息函数
LONG CTCPClientDlg::OnClientDisconnect(WPARAM wParam,LPARAM lParam)
{
	//得到状态栏标签
	CStatic * pStatus = (CStatic *)GetDlgItem(IDC_LBL_CONNSTATUS);
	ASSERT(pStatus != NULL);

	pStatus->SetWindowText(_T("连接断开"));
	return 0;
}

//建立连接
void CTCPClientDlg::OnBnClickedBtnConn()
{
	UpdateData(TRUE);
	CStatic *pStatus = (CStatic*)GetDlgItem(IDC_LBL_CONNSTATUS);
	ASSERT(pStatus != NULL);
	//设置m_tcpClient属性
	m_tcpClient.m_remoteHost = m_remoteHost;
	m_tcpClient.m_port = m_remotePort;
	m_tcpClient.OnDisConnect = OnDisConnect;
	m_tcpClient.OnRead = OnTcpRead;
	m_tcpClient.OnError = OnTcpError;
	//打开客户端socket
	m_tcpClient.Open(this);

	//建立与服务器端连接
	if (m_tcpClient.Connect())
	{

		pStatus->SetWindowText(L"建立连接");
		UpdateData(FALSE);
	}
	else
	{
		AfxMessageBox(_T("建立连接失败"));
		pStatus->SetWindowText(L"连接断开");
		return;
	}
	//设置m_tcpClient属性
	m_tcpClient1.m_remoteHost = m_remoteHost;
	m_tcpClient1.m_port = m_remotePort;
	m_tcpClient1.OnDisConnect = OnDisConnect;
	m_tcpClient1.OnRead = OnTcpRead;
	m_tcpClient1.OnError = OnTcpError;
	//打开客户端socket
	m_tcpClient1.Open(this);

	//建立与服务器端连接
	if (m_tcpClient1.Connect())
	{

		pStatus->SetWindowText(L"建立连接");
		UpdateData(FALSE);
	}
	else
	{
		AfxMessageBox(_T("建立连接失败"));
		pStatus->SetWindowText(L"连接断开");
		return;
	}
	//设置m_tcpClient属性
	m_tcpClient2.m_remoteHost = m_remoteHost;
	m_tcpClient2.m_port = m_remotePort;
	m_tcpClient2.OnDisConnect = OnDisConnect;
	m_tcpClient2.OnRead = OnTcpRead;
	m_tcpClient2.OnError = OnTcpError;
	//打开客户端socket
	m_tcpClient2.Open(this);

	//建立与服务器端连接
	if (m_tcpClient2.Connect())
	{

		pStatus->SetWindowText(L"建立连接");
		UpdateData(FALSE);
	}
	else
	{
		AfxMessageBox(_T("建立连接失败"));
		pStatus->SetWindowText(L"连接断开");
		return;
	}
}

//断开连接
void CTCPClientDlg::OnBnClickedBtnDisconn()
{
	CStatic *pStatus = (CStatic*)GetDlgItem(IDC_LBL_CONNSTATUS);
	ASSERT(pStatus != NULL);
	//关闭客户端套接字
	if (m_tcpClient.Close())
	{
		pStatus->SetWindowText(L"连接断开");
	}
	else
	{
		AfxMessageBox(_T("连接断开失败"));
	}	
	if (m_tcpClient1.Close())
	{
		pStatus->SetWindowText(L"连接断开");
	}
	else
	{
		AfxMessageBox(_T("连接断开失败"));
	}	
	if (m_tcpClient2.Close())
	{
		pStatus->SetWindowText(L"连接断开");
	}
	else
	{
		AfxMessageBox(_T("连接断开失败"));
	}	
}


//发送数据
void CTCPClientDlg::OnBnClickedBtnSenddata()
{
	CString strSendData;
	char * sendBuf;
	int sendLen=0;
	CEdit *pEdtSend = (CEdit*)GetDlgItem(IDC_EDT_SEND);
	pEdtSend->GetWindowTextW(strSendData);

	//设置发送缓冲区
	sendLen = strSendData.GetLength()*2 + 2;
	sendBuf = new char[sendLen];
	ZeroMemory(sendBuf,sendLen);
	wcstombs(sendBuf,strSendData,sendLen);
	sendLen = strlen(sendBuf)+1;
	//发送数据
	if (!m_tcpClient.SendData(sendBuf,sendLen))
	{
		AfxMessageBox(_T("发送失败"));
	}
	//发送数据
	if (!m_tcpClient1.SendData(sendBuf,sendLen))
	{
		AfxMessageBox(_T("发送失败"));
	}
	//发送数据
	if (!m_tcpClient2.SendData(sendBuf,sendLen))
	{
		AfxMessageBox(_T("发送失败"));
	}
	//释放内存
	delete[] sendBuf;
	sendBuf = NULL;		
}

//得到本地的IP地址
CString CTCPClientDlg::GetLocalIP()
{
	HOSTENT *LocalAddress;
	char	*Buff;
	TCHAR	*wBuff;
	CString strReturn = _T("");

	//创建新的缓冲区
	Buff = new char[256];
	wBuff = new TCHAR[256];
	//置空缓冲区
	memset(Buff, '\0', 256);
	memset(wBuff, TEXT('\0'), 256*sizeof(TCHAR));
	//得到本地计算机名
	if (gethostname(Buff, 256) == 0)
	{
		//转换成双字节字符串
		mbstowcs(wBuff, Buff, 256);
		//得到本地地址
		LocalAddress = gethostbyname(Buff);
		//置空buff
		memset(Buff, '\0', 256);
		//组合本地IP地址
		sprintf(Buff, "%d.%d.%d.%d\0", LocalAddress->h_addr_list[0][0] & 0xFF,
			LocalAddress->h_addr_list[0][1] & 0x00FF, LocalAddress->h_addr_list[0][2] & 0x0000FF, LocalAddress->h_addr_list[0][3] & 0x000000FF);
		//置空wBuff
		memset(wBuff, TEXT('\0'), 256*sizeof(TCHAR));
		//转换成双字节字符串
		mbstowcs(wBuff, Buff, 256);
		//设置返回值
		strReturn = wBuff;
	}
	else
	{
	}

	//释放Buff缓冲区
	delete[] Buff;
	Buff = NULL;
	//释放wBuff缓冲区
	delete[] wBuff;
	wBuff = NULL;
	return strReturn;
}
