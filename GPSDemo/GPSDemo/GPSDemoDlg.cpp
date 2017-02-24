// GPSDemoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "GPSDemo.h"
#include "GPSDemoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
//定义GPS串口号
#define GPS_COM_NO 8
// CGPSDemoDlg 对话框

CGPSDemoDlg::CGPSDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGPSDemoDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pGps = NULL;  //GPS类对象 
}

void CGPSDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CGPSDemoDlg, CDialog)

	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_GPS_RECV_BUF,OnRecvSerialData)
	ON_MESSAGE(WM_GPS_STATE_CHANGE_MESSAGE,OnGpsStateChange)
	ON_MESSAGE(WM_GPS_RECV_VALID_LONGLAT,OnRecvValidGps)
	ON_BN_CLICKED(IDC_BTN_OPEN, &CGPSDemoDlg::OnBnClickedBtnOpen)
	ON_BN_CLICKED(IDC_BTN_CLOSE, &CGPSDemoDlg::OnBnClickedBtnClose)
	ON_BN_CLICKED(IDC_BTN_CLEAR, &CGPSDemoDlg::OnBnClickedBtnClear)
END_MESSAGE_MAP()


// CGPSDemoDlg 消息处理程序

BOOL CGPSDemoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}


// 显示GPS串口接收的数据
LONG CGPSDemoDlg::OnRecvSerialData(WPARAM wParam,LPARAM lParam)
{
	//得到原始数据
	char * aRecv = (char *)wParam;
	
	CString strRecv = L"";
	CString strRecv1 = L"";
	//
	CEdit *pEdtRecv = (CEdit*)GetDlgItem(IDC_EDT_RECV);

	//得到接收框原来的文本
	pEdtRecv->GetWindowTextW(strRecv1);
	strRecv = CString(aRecv);
	//追加新接收到的文本
	pEdtRecv->SetWindowTextW(strRecv1+strRecv);
	
	//释放内存
	delete[] aRecv;
	aRecv = NULL;
	return 0;

}

// 显示GPS状态变化
LONG CGPSDemoDlg::OnGpsStateChange(WPARAM wParam,LPARAM lParam)
{
	//GPS状态
	GPSDEV_STATE gpsStatus = (GPSDEV_STATE)wParam;
	CString strHint = L"";
	//
	CStatic *pLblStatus = (CStatic*)GetDlgItem(IDC_LBL_GPS_STATUS);

	switch (gpsStatus)
	{
		//GPS有效数据
	case GPS_VALID_DATA:
		{
			strHint = L"数据有效";
		}
		break;
		//GPS无效数据
	case GPS_INVALID_DATA:
		{
			strHint = L"数据无效";
		}
		break;
		//GPS串口设备未打开
	case GPS_DEV_NOTOPENED:
		{
			strHint = L"串口未打开";
		}
		break;
		//GPS串口设备打开
	case GPS_DEV_OPENED:
		{
			strHint = L"串口打开";
		}
		break;
		//GPS无数据
	case GPS_NODATA:
		{
			strHint = L"无数据";
		}
		break;
	default:
		break;
	}

	//设置GPS状态标题
	pLblStatus->SetWindowTextW(strHint);
	return 0;
}

// 显示GPS有效数据
LONG CGPSDemoDlg::OnRecvValidGps(WPARAM wParam,LPARAM lParam)
{
	//得到GPS数据
	GPSData *pGps = (GPSData*)wParam;

	CEdit *pEdtLong = (CEdit*)GetDlgItem(IDC_EDT_LONG);
	CEdit *pEdtLat = (CEdit*)GetDlgItem(IDC_EDT_LAT);

	CString strLong = CString(pGps->longitude);  //经度
	CString strLat = CString(pGps->latitude);	 //纬度
 
	//显示经纬度信息
	pEdtLong->SetWindowTextW(strLong);
	pEdtLat->SetWindowTextW(strLat);

	//释放内存
	delete pGps;
	pGps = NULL;
	return 0;
}
//打开
void CGPSDemoDlg::OnBnClickedBtnOpen()
{
	CString strTmp;
	//先检查是否释放
	if (m_pGps != NULL)
	{
		m_pGps->Close();
		delete m_pGps;
		m_pGps = NULL;
	}
	//创建GPS逻辑类
	m_pGps = new CGPS();
	//
	if (m_pGps->Open(this,GPS_COM_NO,115200))
	{
		AfxMessageBox(L"GPS设备打开成功");
	}
	else
	{
		AfxMessageBox(L"GPS 设备打开失败");
		return ;
	}
}

//关闭
void CGPSDemoDlg::OnBnClickedBtnClose()
{
	//释放GPS对象
	if (m_pGps != NULL)
	{
		m_pGps->Close();
		delete m_pGps;
		m_pGps = NULL;
	}
}
//清除
void CGPSDemoDlg::OnBnClickedBtnClear()
{
	//
	CEdit *pEdtRecv = (CEdit*)GetDlgItem(IDC_EDT_RECV);

	pEdtRecv->SetWindowTextW(L"");

}