
// SMSDemoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SMSDemo.h"
#include "SMSDemoDlg.h"
#include <atlconv.h>
#include <afx.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define GSM_7BIT		0
#define GSM_8BIT		4
#define GSM_UCS22		8

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


// CSMSDemoDlg 对话框




CSMSDemoDlg::CSMSDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSMSDemoDlg::IDD, pParent)
	, num_to(_T(""))
	, num_from(_T("接收区"))
	, m_num(_T("+8613800290500"))
	, m_nums(_T("+8618710846259"))
	, m_hnum(_T("0"))
	, m_message(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSMSDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SENDMESG, m_sendmsgs);
	DDX_Control(pDX, IDC_READMESG, m_readmsg);
	DDX_Control(pDX, IDC_LIST1, m_list);
	DDX_Control(pDX, IDC_OPENPORT, m_open);
	DDX_Control(pDX, IDC_BCLOSE, m_close);
	DDX_Control(pDX, IDC_BQC, m_qc);
	DDX_Control(pDX, IDC_INITSMS, m_initmsg);
	DDX_Control(pDX, IDC_INITGPRS, m_initgprs);
	DDX_Control(pDX, IDC_BGD, m_gd);
	DDX_Control(pDX, IDC_BBT, m_bt);
	DDX_Control(pDX, IDC_BBH, m_bh);
	DDX_Text(pDX, IDC_EDIT1, num_to);
	DDX_Text(pDX, IDC_EDIT2, num_from);
	DDX_Text(pDX, IDC_EDIT3, m_num);
	DDX_Text(pDX, IDC_EDIT4, m_nums);
	DDX_Text(pDX, IDC_EDIT5, m_hnum);
	DDX_Text(pDX, IDC_MESSAGE, m_message);
}

BEGIN_MESSAGE_MAP(CSMSDemoDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_OPENPORT, &CSMSDemoDlg::OnBnClickedOpenport)
	ON_BN_CLICKED(IDC_BCLOSE, &CSMSDemoDlg::OnBnClickedBclose)
	ON_BN_CLICKED(IDC_INITGPRS, &CSMSDemoDlg::OnBnClickedInitgprs)
	ON_BN_CLICKED(IDC_INITSMS, &CSMSDemoDlg::OnBnClickedInitsms)
	ON_BN_CLICKED(IDC_BREST, &CSMSDemoDlg::OnBnClickedBrest)
	ON_BN_CLICKED(IDC_BEXIT, &CSMSDemoDlg::OnBnClickedBexit)
	ON_BN_CLICKED(IDC_SENDMESG, &CSMSDemoDlg::OnBnClickedSendmesg)
	ON_BN_CLICKED(IDC_READMESG, &CSMSDemoDlg::OnBnClickedReadmesg)
	ON_BN_CLICKED(IDC_BBH, &CSMSDemoDlg::OnBnClickedBbh)
	ON_BN_CLICKED(IDC_BQC, &CSMSDemoDlg::OnBnClickedBqc)
	ON_BN_CLICKED(IDC_BBT, &CSMSDemoDlg::OnBnClickedBbt)
	ON_BN_CLICKED(IDC_BGD, &CSMSDemoDlg::OnBnClickedBgd)
	ON_BN_CLICKED(IDC_B1, &CSMSDemoDlg::OnBnClickedB1)
	ON_BN_CLICKED(IDC_B2, &CSMSDemoDlg::OnBnClickedB2)
	ON_BN_CLICKED(IDC_B3, &CSMSDemoDlg::OnBnClickedB3)
	ON_BN_CLICKED(IDC_B4, &CSMSDemoDlg::OnBnClickedB4)
	ON_BN_CLICKED(IDC_B5, &CSMSDemoDlg::OnBnClickedB5)
	ON_BN_CLICKED(IDC_B6, &CSMSDemoDlg::OnBnClickedB6)
	ON_BN_CLICKED(IDC_B7, &CSMSDemoDlg::OnBnClickedB7)
	ON_BN_CLICKED(IDC_B8, &CSMSDemoDlg::OnBnClickedB8)
	ON_BN_CLICKED(IDC_B9, &CSMSDemoDlg::OnBnClickedB9)
	ON_BN_CLICKED(IDC_BF1, &CSMSDemoDlg::OnBnClickedBf1)
	ON_BN_CLICKED(IDC_B0, &CSMSDemoDlg::OnBnClickedB0)
	ON_BN_CLICKED(IDC_BF2, &CSMSDemoDlg::OnBnClickedBf2)
	ON_LBN_DBLCLK(IDC_LIST1, &CSMSDemoDlg::OnLbnDblclkList1)
	ON_LBN_SELCHANGE(IDC_LIST1, &CSMSDemoDlg::OnLbnSelchangeList1)
END_MESSAGE_MAP()


// CSMSDemoDlg 消息处理程序

BOOL CSMSDemoDlg::OnInitDialog()
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
	m_bt.EnableWindow(false);
	m_bh.EnableWindow(false);
	m_gd.EnableWindow(false);
	m_close.EnableWindow(false);
	m_initgprs.EnableWindow(false);
	m_qc.EnableWindow(true);
	m_initmsg.EnableWindow(false);
	m_sendmsgs.EnableWindow(false);
	pPortOwner=this;
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CSMSDemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CSMSDemoDlg::OnPaint()
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
HCURSOR CSMSDemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CSMSDemoDlg::OnBnClickedOpenport()
{
	// TODO: 在此添加控件通知处理程序代码
	BOOL m_Open=serial.OpenPort(this,4,115200);//初始化端口
	if(!m_Open)	
	{
		MessageBox(L"串口初始化失败!",L"系统信息",MB_OK);
		return;
	}
	MessageBox(L"串口打开成功！",L"提示框");
	serial.m_OnSeriesRead = m_OnSeriesReads;//读线程回调函数
	m_close.EnableWindow(true);
	m_initgprs.EnableWindow(true);
	m_qc.EnableWindow(true);
//	m_open.EnableWindow(false);
}

void CSMSDemoDlg::OnBnClickedBclose()
{
	// TODO: 在此添加控件通知处理程序代码
	serial.ClosePort();	
	m_open.EnableWindow(true);
	m_initgprs.EnableWindow(false);
	m_qc.EnableWindow(false);
}

void CSMSDemoDlg::OnBnClickedInitgprs()
{
	// TODO: 在此添加控件通知处理程序代码
//	CString at="AT\r\n";  
//	m_bInitGSM=serial.WriteChar(at);
	char * buf  =NULL;  //定义发送缓冲区
	DWORD dwBufLen = 0;   //定义发送缓冲区长度
	CString strSend = L"AT\r\n";
	//将待发送的字符串转换成单字节，进行发送
	buf = new char[strSend.GetLength()*2+1];
	ZeroMemory(buf,strSend.GetLength()*2+1);
	//转换成单字节进行发送	
	WideCharToMultiByte(CP_ACP,WC_COMPOSITECHECK,strSend.GetBuffer(strSend.GetLength())
	   ,strSend.GetLength(),buf,strSend.GetLength()*2,NULL,NULL);
	dwBufLen = strlen(buf) + 1;
	//发送字符串
	m_bInitGSM=serial.WriteSyncPort((BYTE*)buf,dwBufLen);
	if(m_bInitGSM==false)
	{
		MessageBox(L"modem无法连接!",L"系统信息",MB_OK);
		serial.ClosePort();
		delete[] buf;
		buf = NULL;
		return;
	}
	//释放内存
	delete[] buf;
	buf = NULL;
	m_bh.EnableWindow(true);
	m_initmsg.EnableWindow(true);
}

void CSMSDemoDlg::OnBnClickedInitsms()
{
	// TODO: 在此添加控件通知处理程序代码
	CString ATE=L"ATE0\r\n";                          //请求建立连接
	CString CMGF=L"AT+CMGF=0\r\n";						//选择短消息发送格式，为0是PDU，为1为TEXT格式
	CString CNMI=L"AT+CNMI=2,1,0,0,0\r\n";               //显示新收到的短信
	serial.WriteChar(ATE);
	serial.WriteChar(CMGF);
	serial.WriteChar(CNMI);
	m_sendmsgs.EnableWindow(true);
}

void CSMSDemoDlg::OnBnClickedBrest()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	serial.m_OnSeriesRead=m_OnSeriesReads;
	m_open.EnableWindow(true);
	m_bt.EnableWindow(false);
	m_bh.EnableWindow(false);
	m_gd.EnableWindow(false);
	m_close.EnableWindow(false);
	m_initgprs.EnableWindow(false);
	m_qc.EnableWindow(false);
	m_initmsg.EnableWindow(false);
	Reset();	
	UpdateData(false);
}

/*复位函数*/
void CSMSDemoDlg::Reset()
{
	serial.ClosePort();
	sendmsg = FALSE;
	m_bInitGSM=FALSE;
	m_bReadMsg=FALSE;
	m_strMsg="";
	num_to="";
	num_from="接收区";
	UpdateData(false);
}
void CSMSDemoDlg::OnBnClickedBexit()
{
	// TODO: 在此添加控件通知处理程序代码
	if(AfxMessageBox(L"退出系统？", MB_YESNO) == IDYES)
	{
		MessageBox(L"退出系统？",L"提示框",MB_OK|MB_ICONASTERISK);
		SetEvent(m_hReadCloseEvent);
		Reset();
		serial.ClosePort();
		Sleep(500);
		AfxGetMainWnd()->PostMessage(WM_QUIT);
		CDialog::OnCancel();
	}
}

/*串口数据回调函数*/
void CALLBACK CSMSDemoDlg::m_OnSeriesReads(void* pPortOwner,BYTE* buf,DWORD bufLen)
{	
	CSMSDemoDlg *pDlg = (CSMSDemoDlg *)pPortOwner;
	pDlg->UpdateData(TRUE);
	pDlg->strNum=_T("");
	LPTSTR pStr = pDlg->strNum.GetBuffer(bufLen);
	MultiByteToWideChar(CP_ACP, 0, (char *)buf, bufLen, pStr, bufLen);//转换为Unicode编码
	pDlg->strNum.ReleaseBuffer();
	if ((pDlg->strNum.Find(_T("RING")) >= 0) || 
		(pDlg->strNum.Find(_T("ring")) >= 0))
	{													//有电话打进来 		
//		pDlg->GetNum(pDlg->strNum);
		pDlg->m_bt.EnableWindow(true);
	}
	else
	if ((pDlg->strNum.Find(_T("NO CARRIER")) >= 0) || 
	(pDlg->strNum.Find(_T("no carrier")) >= 0))				
	{													// 来电后, 对方挂机 
		CString st=L"AT+CHUP\r\n";
		pDlg->serial.WriteChar(st);
		pDlg->MessageBox(L"对方已挂机，正在发送挂机命令",L"系统信息");

	}
	else
	if ((pDlg->strNum.Find(_T("ERROR"), 0) >= 0) ||
		(pDlg->strNum.Find(_T("error"), 0) >= 0))
	{													// 响应中有"ERROR" 
		pDlg->MessageBox(L"动作失败",L"系统信息");	
	}
	if ((pDlg->strNum.Find(_T("OK"), 0) >= 0) ||
		(pDlg->strNum.Find(_T("ok"), 0) >= 0))
	{	
	//	pDlg->MessageBox(L"找到OK",L"系统信息");
	// 响应中有"OK" 
	}
	else
	if((pDlg->strNum.Find(_T("+CMGL:")),0)||(pDlg->strNum.Find(_T("+cmgl:")),0))
	{
			int posnum, numlen, posmsg, msglen;	
			int pos=pDlg->FindMsgPos(&posnum, &numlen, &posmsg, &msglen,pDlg->strNum);
	}
	else
	if((pDlg->strNum.Find(_T("+CMS ERROR")),0)||(pDlg->strNum.Find(_T("+cms error")),0))
	{
		pDlg->MessageBox(L"读短信错误",L"系统信息");
	}
	else
	if((pDlg->strNum.Find(_T("CMTI")),0)||(pDlg->strNum.Find(_T("cmti")),0))
	{
		pDlg->MessageBox(L"有新短消息",L"系统信息");
		int i=pDlg->strNum.GetLength();
//		char cmd[16];
		int index=(int)pDlg->strNum[i];
		if(AfxMessageBox(L"是否读取此消息？", MB_YESNO) == IDYES)
		{
			CString cmd;
//			sprintf(cmd,format, index);	// 生成命令
			cmd.Format(_T("AT+CMGR=%d\r"),index);
			pDlg->serial.WriteChar(cmd);
			Sleep(100);
		}
	}
	else
	if(pDlg->strNum.Find(_T("CMGR"), 0))
	{
	//在这里读消息
		SM_PARAM *pMsg = NULL;
		char* ptr;
		SM_BUFF *pBuff = NULL;
		CString str=L"短信ID:";
		CString str1=L"短信来自:";
		ptr=pBuff->data;
		while((ptr = strstr(ptr,"+CMGL:")) != NULL)
		{
			ptr += 6;		// 跳过"+CMGL:", 定位到序号
			sscanf(ptr, "%d", &pMsg->index);	// 读取序号
//			str+=&pMsg->index;
			ptr = strstr(ptr, "\r\n");	// 找下一行
			if (ptr != NULL)
			{
				str+=str1;
				ptr += 2;		// 跳过"\r\n", 定位到PDU		
				pDlg->encode.gsmDecodePdu(ptr, pMsg);	// PDU串解码
//				str+=&pMsg->TPA;
				str+=" 短信发送时间:";
//				str+=&pMsg->TP_SCTS;
			//	m_list.AddString(str);
//				m_message="";
//				m_message+=str;
			}
		}

	}
	else
	if((pDlg->strNum.Find(_T("CMGL")),0)||(pDlg->strNum.Find(_T("CMGL")),0))
	{
		SM_PARAM *pMsg;
		char* ptr;
		SM_BUFF *pBuff;
		CString str=L"短信ID:";
		CString str1=L"短信来自:";
		ptr=pBuff->data;
		while((ptr = strstr(ptr, "+CMGL:")) != NULL)
		{
			ptr += 6;		// 跳过"+CMGL:", 定位到序号
			sscanf(ptr, "%d", &pMsg->index);	// 读取序号
//			str+=&pMsg->index;
			ptr = strstr(ptr, "\r\n");	// 找下一行
			if (ptr != NULL)
			{
				str+=str1;
				ptr += 2;		// 跳过"\r\n", 定位到PDU		
				pDlg->encode.gsmDecodePdu(ptr, pMsg);	// PDU串解码
//				str+=&pMsg->TPA;
				str+=L" 短信发送时间:";
//				str+=&pMsg->TP_SCTS;
//				m_list.AddString(str);
			}
		}
	}
	else
	{
		for(DWORD i=0;i<bufLen;i++)
			pDlg->num_from.Format(L"%s %c",pDlg->num_from,buf[i]);
	}
	pDlg->UpdateData(FALSE);
	delete[] buf;
}

/*文本模式下读取短消息*/
BOOL CSMSDemoDlg::ReadMessage()
{
	CString strTel,Msg;
	int posnum, numlen, posmsg, msglen;	
	int pos = FindMsgPos(&posnum, &numlen, &posmsg, &msglen,strNum);	// 寻找短信位置
	if (pos > 0)
	{
		strTel=strNum.Mid(posnum, numlen);   //发送方手机号
		Msg = strNum.Mid(posmsg, msglen);				// 取出短信内容 
	}
	num_from="来电号码:";
	num_from+=strTel;
	num_from+="\n来电信息:";
	num_from+=Msg;
	return TRUE;
}
/*文本模式下寻找短信位置*/
int CSMSDemoDlg::FindMsgPos(int *posnum, int *numlen, int *posmsg, int *msglen,CString strs)
{
	// 短信开始位置
	int posStart = strs.Find(_T("+CMGR:"), 0);					
	if (posStart < 0)
		return -1;

	// 电话号码开始位置
	*posnum = strs.Find(_T("\"REC READ\",\""), posStart);	
	if (*posnum < 0)
		return -1;

	*posnum = *posnum + wcslen(_T("\"REC READ\",\""));			

	// 电话号码结束位置
	int posnumend = strs.Find(_T("\""), *posnum + 1);
	if (posnumend < 0)
		return -1;

	// 电话号码长度
	*numlen = posnumend - *posnum;				
	
	// 短信开始位置
	*posmsg = strs.Find(_T("\r\n"), posStart);				
	if (*posmsg < 0)
		return -1;

	*posmsg = *posmsg + wcslen(_T("\r\n"));	
	
	// 短信结束位置
	int posmsgend = strs.Find(_T("\r\n\r\nOK"), *posmsg);
	*msglen = posmsgend - *posmsg;	
	ReadMessage();
	return 1;
}

BOOL CSMSDemoDlg::waitok()
{
	return false;
}
void CSMSDemoDlg::GetNum(CString str)
{
	CString strTum=L"";
	int pos1 =str.Find(_T("+CLIP: \""), 0);
	if (pos1 < 0)
		MessageBox(L"找不到来电号码",L"系统信息");	
	else
		pos1 = pos1 + wcslen(_T("+CLIP: \""));
	int pos2 =str.Find(_T("\""), pos1);
	if (pos2 < 0)
		MessageBox(L"找不到来电号码",L"系统信息");
	strTum = str.Mid(pos1, pos2-pos1);				  /* 取得电话号码 */
	num_from=strTum;
	UpdateData(false);
//	return strNum;
}
void CSMSDemoDlg::OnBnClickedSendmesg()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	CString strSmsc=m_num;
	CString strNumber=m_nums;
	CString strContent=m_message;
	if(AfxMessageBox(L"确定发送吗？", MB_YESNO) == IDYES)	  //IDYES   The Yes button was selected.IDYES=6
	{
		SM_PARAM SmParam;
		memset(&SmParam, 0, sizeof(SM_PARAM));  //void int size_t(count) 指针 放置的位置 几个字符

		// 去掉号码前的"+"
		if(strSmsc[0] == '+')  strSmsc = strSmsc.Mid(1);   //从第一个开始提取，而不是第0个
		if(strNumber[0] == '+')  strNumber = strNumber.Mid(1);
		CString strtmp=_T("86");


		// 在号码前加"86"
		if(strSmsc.Left(2) != "86")     //参数：nCount 要从这个CString对象中提取的字符数为2。 
		{
			strtmp+=strSmsc;//strSmsc = "86" + strSmsc;
			strSmsc=strtmp;
		}
		strtmp=_T("86");
		if(strNumber.Left(2) != "86") 
		{
			strtmp+=strNumber;
			strNumber=strtmp;
		}

		// 填充短消息结构
		USES_CONVERSION;          //避免编译错误
		char* pStr=W2A(strSmsc);  //
		strcpy(SmParam.SCA, pStr);//strSmsc);   短消息服务中心号码(SMSC地址)
		
		pStr=W2A(strNumber);
		strcpy(SmParam.TPA, pStr);//strNumber);   目标号码或回复号码(TP-DA或TP-RA)
		
		pStr=W2A(strContent);
		strcpy(SmParam.TP_UD, pStr);        //  原始用户信息(编码前或解码后的TP-UD)
		SmParam.TP_PID=0;                 //  用户信息协议标识(TP-PID)

		SmParam.TP_DCS=GSM_UCS22;         //  用户信息编码方式(TP-DCS)

		int nPduLength;		// PDU串长度
		unsigned char nSmscLength;	// SMSC串长度
//		char cmd[16];		// 命令串

		nPduLength=encode.gsmEncodePdu(&SmParam,pdu);
		strcat(pdu, "\x01a");

		encode.gsmString2Bytes(pdu, &nSmscLength, 2);	// 取PDU串中的SMSC信息长度
		nSmscLength++;		// 加上长度字节本身
		// 命令中的长度，不包括SMSC信息长度，以数据字节计
//		sprintf(cmd, "AT+CMGS=%d\r", nPduLength / 2 - nSmscLength);	// 生成命令
		CString cmd;
		cmd.Format(_T("AT+CMGS=%d\r"),nPduLength / 2 - nSmscLength);
		serial.WriteChar(cmd);
	}
}

void CSMSDemoDlg::OnBnClickedReadmesg()
{
	// TODO: 在此添加控件通知处理程序代码
	CString CMGL=L"AT+CMGL=\"ALL\"\r\n";
	serial.WriteChar(CMGL);
}

void CSMSDemoDlg::OnBnClickedBbh()
{
	// TODO: 在此添加控件通知处理程序代码
	BOOL ret;
	CString num=L"ATD ";
	num+=num_to;
	num+=L";\r\n";
	ret=serial.WriteChar(num);
	if(ret==false)
	{
		MessageBox(L"拨号失败",L"提示框");
		return;
	}
	MessageBox(L"拨号成功",L"提示框");
	m_gd.EnableWindow(true);
}

void CSMSDemoDlg::OnBnClickedBqc()
{
	// TODO: 在此添加控件通知处理程序代码
	num_to=L"";
	UpdateData(false);
}

void CSMSDemoDlg::OnBnClickedBbt()
{
	// TODO: 在此添加控件通知处理程序代码
	BOOL ret;
	CString ata=L"ATA\r\n";
	ret=serial.WriteChar(ata);
	if(ret==false)
	{
		MessageBox(L"接听失败",L"提示框");
		return;
	}
	m_gd.EnableWindow(true);
}

void CSMSDemoDlg::OnBnClickedBgd()
{
	// TODO: 在此添加控件通知处理程序代码
	BOOL ret;
	CString ath=L"ATH\r\n";
	ret=serial.WriteChar(ath);
	if(ret==false)
	{
		MessageBox(L"挂断失败",L"提示框");
		return;	
	}	
	MessageBox(L"挂断成功",L"提示框");	
	m_gd.EnableWindow(false);
	return;
}

void CSMSDemoDlg::OnBnClickedB1()
{
	// TODO: 在此添加控件通知处理程序代码
	num_to=num_to+L"1";	
	UpdateData(false);	
}

void CSMSDemoDlg::OnBnClickedB2()
{
	// TODO: 在此添加控件通知处理程序代码
	num_to=num_to+L"2";	
	UpdateData(false);	
}

void CSMSDemoDlg::OnBnClickedB3()
{
	// TODO: 在此添加控件通知处理程序代码
	num_to=num_to+L"3";	
	UpdateData(false);	
}

void CSMSDemoDlg::OnBnClickedB4()
{
	// TODO: 在此添加控件通知处理程序代码
	num_to=num_to+L"4";	
	UpdateData(false);	
}

void CSMSDemoDlg::OnBnClickedB5()
{
	// TODO: 在此添加控件通知处理程序代码
	num_to=num_to+L"5";	
	UpdateData(false);	
}

void CSMSDemoDlg::OnBnClickedB6()
{
	// TODO: 在此添加控件通知处理程序代码
	num_to=num_to+L"6";	
	UpdateData(false);	
}

void CSMSDemoDlg::OnBnClickedB7()
{
	// TODO: 在此添加控件通知处理程序代码
	num_to=num_to+L"7";	
	UpdateData(false);	
}

void CSMSDemoDlg::OnBnClickedB8()
{
	// TODO: 在此添加控件通知处理程序代码
	num_to=num_to+L"8";	
	UpdateData(false);	
}

void CSMSDemoDlg::OnBnClickedB9()
{
	// TODO: 在此添加控件通知处理程序代码
	num_to=num_to+L"9";	
	UpdateData(false);	
}

void CSMSDemoDlg::OnBnClickedBf1()
{
	// TODO: 在此添加控件通知处理程序代码
	num_to=num_to+L"*";	
	UpdateData(false);	
}

void CSMSDemoDlg::OnBnClickedB0()
{
	// TODO: 在此添加控件通知处理程序代码
	num_to=num_to+L"0";	
	UpdateData(false);	
}

void CSMSDemoDlg::OnBnClickedBf2()
{
	// TODO: 在此添加控件通知处理程序代码
	num_to=num_to+L"#";	
	UpdateData(false);	
}

void CSMSDemoDlg::OnLbnDblclkList1()
{
	// TODO: 在此添加控件通知处理程序代码
}

void CSMSDemoDlg::OnLbnSelchangeList1()
{
	// TODO: 在此添加控件通知处理程序代码
}
