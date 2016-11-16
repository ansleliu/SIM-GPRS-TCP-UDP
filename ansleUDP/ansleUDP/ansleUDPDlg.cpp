
// ansleUDPDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ansleUDP.h"
#include "ansleUDPDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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


// CansleUDPDlg 对话框




CansleUDPDlg::CansleUDPDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CansleUDPDlg::IDD, pParent)
	, m_RemoteHost(_T(""))
	, m_RemotePort(0)
	, m_LocalPort(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CansleUDPDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_REMOTEHOST, m_RemoteHost);
	DDX_Text(pDX, IDC_EDIT_REMOTEPORT, m_RemotePort);
	DDX_Text(pDX, IDC_EDIT_LOCALPORT, m_LocalPort);
}

BEGIN_MESSAGE_MAP(CansleUDPDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_OPEN, &CansleUDPDlg::OnBnClickedBtnOpen)
	ON_BN_CLICKED(IDC_BTN_CLOSE, &CansleUDPDlg::OnBnClickedBtnClose)
	ON_BN_CLICKED(IDC_BTN_SEND, &CansleUDPDlg::OnBnClickedBtnSend)
	ON_MESSAGE(WM_RECV_UDP_DATA,OnRecvUdpData)
END_MESSAGE_MAP()


// CansleUDPDlg 消息处理程序

BOOL CansleUDPDlg::OnInitDialog()
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

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CansleUDPDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CansleUDPDlg::OnPaint()
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
HCURSOR CansleUDPDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//打开UDP
void CansleUDPDlg::OnBnClickedBtnOpen()
{
   UpdateData(TRUE);
   m_CEUdp.m_OnUdpRecv = OnUdpCERecv;
   m_CEUdp.m_OnUdpError = OnUdpCEError;
   DWORD nResult = m_CEUdp.Open(this,m_LocalPort,m_RemoteHost.GetBuffer(m_RemoteHost.GetLength()),m_RemotePort);
   if (nResult <=0) 
   {
	   AfxMessageBox(_T("打开端口失败"));
   }
}

//关闭UDP
void CansleUDPDlg::OnBnClickedBtnClose()
{
  m_CEUdp.Close();	
}

//发送数据
void CansleUDPDlg::OnBnClickedBtnSend()
{
	char * buf  =NULL;  //定义发送缓冲区
	DWORD dwBufLen = 0;   //定义发送缓冲区长度
	CString strSend = L"";

	//得到发送输入框
	CEdit *pEdtSendMsg = (CEdit*)GetDlgItem(IDC_EDT_SEND);
	ASSERT(pEdtSendMsg != NULL);

	//得到待发送的字符串
	pEdtSendMsg->GetWindowTextW(strSend);

	//将待发送的字符串转换成单字节，进行发送
	buf = new char[strSend.GetLength()*2+1];
	ZeroMemory(buf,strSend.GetLength()*2+1);
	//转换成单字节进行发送	
	WideCharToMultiByte(CP_ACP,WC_COMPOSITECHECK,strSend.GetBuffer(strSend.GetLength())
	   ,strSend.GetLength(),buf,strSend.GetLength()*2,NULL,NULL);

	dwBufLen = strlen(buf) + 1;

	//发送数据
    m_CEUdp.SendData(buf,dwBufLen);	

	//释放内存
	delete[] buf;
	buf = NULL;
}

//UDP数据接收回调函数
void CALLBACK CansleUDPDlg::OnUdpCERecv(void * pOwner,char* buf,DWORD dwBufLen,sockaddr * addr)
{
	BYTE *pRecvBuf = NULL; //接收缓冲区
	//得到父对象指针
	CansleUDPDlg* pThis = (CansleUDPDlg*)pOwner;
	//将接收的缓冲区拷贝到pRecvBuf种
	pRecvBuf = new BYTE[dwBufLen];
	CopyMemory(pRecvBuf,buf,dwBufLen);

	//发送异步消息，表示收到串口数据，消息处理完，应释放内存
	pThis->PostMessage(WM_RECV_UDP_DATA,WPARAM(pRecvBuf),dwBufLen);
} 

//UDP报错回调函数
void CALLBACK CansleUDPDlg::OnUdpCEError(void * pOwner,int nErrorCode)
{

}


// UDP接收数据处理函数
LONG CansleUDPDlg::OnRecvUdpData(WPARAM wParam,LPARAM lParam)
{
	CString strOldRecv = L"";
	CString strRecv = L"";
	//串口接收到的BUF
	CHAR *pBuf = (CHAR*)wParam;
	//串口接收到的BUF长度
	DWORD dwBufLen = lParam;
	
	//接收框
	CEdit *pEdtRecvMsg = (CEdit*)GetDlgItem(IDC_EDT_RECV);
	ASSERT(pEdtRecvMsg != NULL);

	//得到接收框中的历史文本
	pEdtRecvMsg->GetWindowTextW(strOldRecv);
	
	//
    strRecv = CString(pBuf);
	//将新接收到的文本添加到接收框中
	strOldRecv = strOldRecv + strRecv +L"\r\n";
	pEdtRecvMsg->SetWindowTextW(strOldRecv);

	//释放内存
	delete[] pBuf;
	pBuf = NULL;
	return 0;
}
