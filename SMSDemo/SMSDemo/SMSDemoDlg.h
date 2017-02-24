
// SMSDemoDlg.h : 头文件
//

#pragma once
#include"CESeries.h"
#include"Encode.h"
#include "afxwin.h"

typedef struct {
	int len;
	char data[16384];
} SM_BUFF;

// CSMSDemoDlg 对话框
class CSMSDemoDlg : public CDialog
{
// 构造
public:
	CSMSDemoDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_SMSDEMO_DIALOG };
	CButton m_sendmsgs;
	CButton m_readmsg;
	CListBox m_list;
	CButton m_open;
	CButton m_close;
	CButton m_qc;
	CButton m_initmsg;
	CButton m_initgprs;
	CButton m_gd;
	CButton m_bt;
	CButton m_bh;
	CString num_to;
	CString num_from;
	CString m_num;
	CString m_nums;
	CString m_hnum;
	CString m_message;
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;
    BOOL ReadMessage();//读取短信
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBnClickedOpenport();
	afx_msg void OnBnClickedBclose();
	afx_msg void OnBnClickedInitgprs();
	afx_msg void OnBnClickedInitsms();
	afx_msg void OnBnClickedBrest();
	afx_msg void OnBnClickedBexit();
	afx_msg void OnBnClickedSendmesg();
	afx_msg void OnBnClickedReadmesg();
	afx_msg void OnBnClickedBbh();
	afx_msg void OnBnClickedBqc();
	afx_msg void OnBnClickedBbt();
	afx_msg void OnBnClickedBgd();
	afx_msg void OnBnClickedB1();
	afx_msg void OnBnClickedB2();
	afx_msg void OnBnClickedB3();
	afx_msg void OnBnClickedB4();
	afx_msg void OnBnClickedB5();
	afx_msg void OnBnClickedB6();
	afx_msg void OnBnClickedB7();
	afx_msg void OnBnClickedB8();
	afx_msg void OnBnClickedB9();
	afx_msg void OnBnClickedBf1();
	afx_msg void OnBnClickedB0();
	afx_msg void OnBnClickedBf2();
	afx_msg void OnLbnDblclkList1();
	afx_msg void OnLbnSelchangeList1();
	afx_msg void GetNum(CString str);
	DECLARE_MESSAGE_MAP()

public:
	BOOL waitok();
//	typedef void (CALLBACK* ONSERIESREAD)(CWnd* pPortOwner,BYTE* buf,int bufLen);
//	ONSERIESREAD m_OnSeriesRead; 
	CWnd* pPortOwner;//窗口类指针
	CEncode encode;			//编码类指针
	CCESeries serial;   //串口类变量
	BOOL sendmsg;         //发送判定
	BOOL m_bInitGSM;        //GPRS状态
	BOOL m_bReadMsg;        //短信接收状态、
	BOOL bSendATCmd;		//短信发送状态
	CString strNum;          //读取缓冲区数据
	CString m_strMsg;   //短信内容
	CListCtrl m_ListMsg;   //短信指针
	char pdu[512];          //PDU字符
	void Reset(void);
	void EnableBtn(bool bvalue);
	
private:
    int FindMsgPos(int *posnum, int *numlen, int *posmsg, int *msglen,CString str);//回调函数
	HANDLE m_hReadCloseEvent;
	static void CALLBACK m_OnSeriesReads(void* pPortOwner,BYTE* buf,DWORD bufLen); 
};
