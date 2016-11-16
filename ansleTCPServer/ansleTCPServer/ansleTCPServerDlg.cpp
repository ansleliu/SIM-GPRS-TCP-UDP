
// ansleTCPServerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ansleTCPServer.h"
#include "ansleTCPServerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//定义TCP 收到客户端数据消息
#define WM_RECV_TCP_DATA WM_USER + 101
//定义TCP客户端连接消息
#define WM_TCP_CLIENT_CONNECT WM_USER + 102

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CansleTCPServerDlg 对话框




CansleTCPServerDlg::CansleTCPServerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CansleTCPServerDlg::IDD, pParent)
	,m_localPort(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CansleTCPServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDT_LOCALPORT, m_localPort);
}

BEGIN_MESSAGE_MAP(CansleTCPServerDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_RECV_TCP_DATA,OnRecvTCPData)
	ON_MESSAGE(WM_TCP_CLIENT_CONNECT,OnClientConnect)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_LISTEN, &CansleTCPServerDlg::OnBnClickedBtnListen)
	ON_BN_CLICKED(IDC_BTN_CLOSE, &CansleTCPServerDlg::OnBnClickedBtnClose)
END_MESSAGE_MAP()


// CansleTCPServerDlg 消息处理程序

BOOL CansleTCPServerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	//设置默认值
	m_localPort = 5000;
	UpdateData(FALSE);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CansleTCPServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CansleTCPServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CansleTCPServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//客户端连接建立事件处理函数
void CALLBACK  CansleTCPServerDlg::OnClientConnect(void* pOwner,ansleTCPCustom* pTcpCustom)
{
	TCHAR *szAddress =NULL;
	DWORD dwBufLen = pTcpCustom->m_RemoteHost.GetLength() + 1;
	szAddress = new TCHAR[dwBufLen];
	ZeroMemory(szAddress,dwBufLen*2);
	//拷贝内存，得到客户端IP地址
	wcscpy(szAddress,pTcpCustom->m_RemoteHost);
	
	CansleTCPServerDlg *pThis = (CansleTCPServerDlg*)pOwner;
	
	//发送异步消息，表示有客户端连接，消息处理完后，需要释放内存
	pThis->PostMessage(WM_TCP_CLIENT_CONNECT,0,LPARAM(szAddress));
}

//客户端SOCKET关闭事件处理函数
void  CALLBACK CansleTCPServerDlg::OnClientClose(void* pOwner,ansleTCPCustom* pTcpCustom)
{
	TCHAR *szAddress =NULL;
	DWORD dwBufLen = pTcpCustom->m_RemoteHost.GetLength() + 1;
	szAddress = new TCHAR[dwBufLen];
	ZeroMemory(szAddress,dwBufLen*2);
	//拷贝内存，得到客户端IP地址
	wcscpy(szAddress,pTcpCustom->m_RemoteHost);

	CansleTCPServerDlg *pThis = (CansleTCPServerDlg*)pOwner;
	
	//发送异步消息，表示有客户端连接，消息处理完后，需要释放内存
	pThis->PostMessage(WM_TCP_CLIENT_CONNECT,1,LPARAM(szAddress));

}

//服务器端收到来自客户端的数据
void CALLBACK CansleTCPServerDlg::OnClientRead(void* pOwner,ansleTCPCustom* pTcpCustom,const char * buf,DWORD dwBufLen )
{
	DATA_BUF *pGenBuf = new DATA_BUF;
	char *pRecvBuf = NULL; //接收缓冲区
	//得到父对象指针
	CansleTCPServerDlg* pThis = (CansleTCPServerDlg*)pOwner;
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
void CALLBACK CansleTCPServerDlg::OnClientError(void* pOwner,ansleTCPCustom* pTcpCustom,int nErrorCode)
{
	
}

//服务器端Socket错误事件处理函数
void CALLBACK CansleTCPServerDlg::OnServerError(void* pOwner,ansleTCPServerMain* pansleTCPServerMain,int nErrorCode)
{
	
}

//TCP接收数据处理函数
LONG CansleTCPServerDlg::OnRecvTCPData(WPARAM wParam,LPARAM lParam)
{
	DATA_BUF *pGenBuf = (DATA_BUF*)wParam; //通用缓冲区
	ansleTCPCustom* pTcpCustom= (ansleTCPCustom* )lParam; //TCP客户端对象
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
LONG CansleTCPServerDlg::OnClientConnect(WPARAM wParam,LPARAM lParam)
{
	int iIndex = 0;
	TCHAR *szAddress = (TCHAR*)lParam;
	CString strAddrss = szAddress;
	
	CListBox * pLstConn = (CListBox*)GetDlgItem(IDC_LST_CONN);
	ASSERT(pLstConn != NULL);

	if (wParam == 0)
	{
		pLstConn->AddString(strAddrss + L"建立连接");
	}
	else
	{
		iIndex = pLstConn->FindString(iIndex,strAddrss + L"建立连接");
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
void CansleTCPServerDlg::OnBnClickedBtnListen()
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
void CansleTCPServerDlg::OnBnClickedBtnClose()
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
