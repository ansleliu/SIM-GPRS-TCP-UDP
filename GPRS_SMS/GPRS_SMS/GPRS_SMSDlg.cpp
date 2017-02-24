
// GPRS_SMSDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "GPRS_SMS.h"
#include "GPRS_SMSDlg.h"

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


// CGPRS_SMSDlg 对话框




CGPRS_SMSDlg::CGPRS_SMSDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGPRS_SMSDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CGPRS_SMSDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CGPRS_SMSDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_SET_MSGCENTR_TELCODE, &CGPRS_SMSDlg::OnBnClickedSetMsgcentrTelcode)
	ON_BN_CLICKED(IDC_SENDMSG, &CGPRS_SMSDlg::OnBnClickedSendmsg)
END_MESSAGE_MAP()


// CGPRS_SMSDlg 消息处理程序

BOOL CGPRS_SMSDlg::OnInitDialog()
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

	ShowWindow(SW_MINIMIZE);

	// TODO: 在此添加额外的初始化代码
	SetDlgItemText(IDC_SENDMSG_TELCODE, _T("18710846798"));		
	SetDlgItemText(IDC_EDIT_SENDMSG, _T("ansleliu"));
	BOOL ret = m_GPRS.GPRS_Init(_T("COM4:"), 115200, (DWORD)this);
	if (ret == FALSE)
	{
		MessageBox(_T("GPRS初始化失败, 请检查是否安装正确."));
	}	
	SetDlgItemText(IDC_MSGCENTR_TELCODE, _T("+8613800290500"));
	m_GPRS.GPRS_DeleteShortMsg(1);		/* 删除第 1 条短信*/
	SetTimer(1, 1000, NULL);			/* 每 1 秒读取一次短信 */

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CGPRS_SMSDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CGPRS_SMSDlg::OnPaint()
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
HCURSOR CGPRS_SMSDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CGPRS_SMSDlg::OnTimer(UINT nIDEvent) 
{
	BOOL ret;
	CString strTelCode, strMsg;
	ret = m_GPRS.GPRS_ReadShortMessage(1, &strTelCode, &strMsg);// 读取第 0 条短信
	if (ret == TRUE)
	{
		for (int i = 0; i < strMsg.GetLength(); i++)
		{	
			if ((char)strMsg.GetAt(i) == '\n')	//有过行字符 
			if ((char)strMsg.GetAt(i - 1) != '\r')	// 但没有回车字符 
			{
				strMsg.Insert(i, '\r');//插入回车符
			}
		}
		SetDlgItemText(IDC_RECVMSG_TELCODE, strTelCode);//显示电话号码 
		SetDlgItemText(IDC_DISP_RECVMSG, strMsg);	// 显示短信内容
		m_GPRS.GPRS_DeleteShortMsg(1);	// 删除短信
	}
	
	SetTimer(1, 1000, NULL);	//每 1 秒读取一次短信 
	CDialog::OnTimer(nIDEvent);
}
void CGPRS_SMSDlg::OnBnClickedSetMsgcentrTelcode()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strCode;
	GetDlgItemText(IDC_MSGCENTR_TELCODE, strCode);
	BOOL ret = m_GPRS.GPRS_SetShortMSGCenterTel(strCode);	//设置短信中号码 
	if (ret == TRUE)
		MessageBox(_T("设置短信中心号码成功."));
	else
		MessageBox(_T("设置短信中心号码失败."));
}

void CGPRS_SMSDlg::OnBnClickedSendmsg()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strTelCode, strMsg;

	GetDlgItemText(IDC_SENDMSG_TELCODE, strTelCode);//获取发送短信电话号码及内容	
	GetDlgItemText(IDC_EDIT_SENDMSG, strMsg);	

	if ((strTelCode == "") || (strMsg == ""))
	{	//判断输入内容是否正确 
		MessageBox(_T("电话号码或短信内容不能为空."));
		return;
	}

	BOOL ret = m_GPRS.GPRS_SendShortMessage(strTelCode, strMsg);//发送短信
	if (ret == TRUE)
		MessageBox(_T("短信发送成功."));
	else
		MessageBox(_T("短信发送失败."));
}
