
// GPRS_SMSDlg.h : 头文件
//

#pragma once
#include "GPRS.h"
// CGPRS_SMSDlg 对话框
class CGPRS_SMSDlg : public CDialog
{
// 构造
public:
	CGPRS_SMSDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_GPRS_SMS_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	CGPRS m_GPRS;
public:
	afx_msg void OnBnClickedSetMsgcentrTelcode();
	afx_msg void OnBnClickedSendmsg();
};
