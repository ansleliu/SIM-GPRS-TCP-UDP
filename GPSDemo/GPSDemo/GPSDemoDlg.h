// GPSDemoDlg.h : 头文件
//

#pragma once
#include "gps.h"
// CGPSDemoDlg 对话框
class CGPSDemoDlg : public CDialog
{
// 构造
public:
	CGPSDemoDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_GPSDEMO_DIALOG };


	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();

	// 显示GPS串口接收的数据
	afx_msg LONG OnRecvSerialData(WPARAM wParam,LPARAM lParam);
	// 显示GPS状态变化
	afx_msg LONG OnGpsStateChange(WPARAM wParam,LPARAM lParam);
	// 显示GPS有效数据
	afx_msg LONG OnRecvValidGps(WPARAM wParam,LPARAM lParam);
	DECLARE_MESSAGE_MAP()
private:
	CGPS *m_pGps;  //GPS类对象
public:
	afx_msg void OnBnClickedBtnOpen();
	afx_msg void OnBnClickedBtnClose();
	afx_msg void OnBnClickedBtnClear();
};
