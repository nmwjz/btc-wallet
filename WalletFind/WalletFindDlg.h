#pragma once

#include "WalletJob.h"

#include "ThreadProcCryptoCPU.h"
#include "ThreadProcCryptoCL.h"
//#include "ThreadProcCryptoCL20.h"

#define WM_ICON_NOTIFY WM_USER + 1

// CWalletFindDlg 对话框
class CWalletFindDlg : public CDialogEx
{
// 构造
public:
	CWalletFindDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_WALLETFIND_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);                     //定时器处理函数
	afx_msg LRESULT OnNotifyIcon(WPARAM wparan, LPARAM lparam);  //托盘事件处理函数
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	//鼠标界面的点击事件
	afx_msg void OnBnClickedStart();
	afx_msg void OnBnClickedStop();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedSetting();
	afx_msg void OnBnSaveResult();
	afx_msg void OnBnClickedHideTray();
	afx_msg void OnBnClickedHideBg();
	afx_msg LRESULT OnHotKey(WPARAM wParam, LPARAM lParam);

private:
	//创建业务处理线程
	int ProcTaskCreate();
	//销毁业务处理线程
	int ProcTaskDestroy();
	//检测线程的结果状态
	int ProcTaskResultState();
	//检测线程的完成状态
	int ProcTaskFinishState();
	//处理获取最新任务
	int ProcTaskGet();
	//向服务端上报发现结果
	int ProcTaskResultSend(unsigned char *pPrivateKeyStart, unsigned int iIndex, unsigned char *pPublicKey, unsigned int iJobID);
	//向服务端上报当前处理进度
	int ProcTaskProcSend(unsigned int iJobID);
	//计算私钥 和 整数相加
	int ProcPrivateAddInt( unsigned char * szPrivateKey , unsigned int iNum );
	//向ListCtrl中插入行数--任务列表
	int ProcTaskListInsert(unsigned int iTaskID, unsigned int iPrivateCount, unsigned char * szPrivateKey);
	//将私钥串转化为可以显示的字符串
	int ProcPrivateKeyTransfer(char * szPrivateKeyDisplay, unsigned char * szPrivateKey);
	//将私钥串转化为二进制的字符串
	int ProcPrivateKeyTransfer2(unsigned char * szPrivateKey, char * szPrivateKeyDisplay);
	//向ListCtrl中插入行数--任务结果
	int ProcTaskResultInsert(unsigned char * szPrivateKey, unsigned int iCPUID, unsigned char * szPublicKey);

	//加载系统中所有的钱包
	int ProcLoadWalletData(const char * pWalletFile);
	unsigned long long HexToDec(char * pHex);

	//系统托盘对象
	NOTIFYICONDATA m_Notify;

	//开始的私钥(从服务器获取)
	unsigned char szPrivateKey[32] = { 0 };
	//私钥的最大计算量(从服务器获取)
	unsigned long long iPrivateCountMax = 0x0;
	//工作JobID(从服务器获取)
	unsigned int iJobID = 0x0;

	//每个线程的计算量(本地设置量，或默认量)
	unsigned int iPrivateCount = 0x20000000;
	//用户账号
	char szAccount[64] = { 0 };
	//需要使用CPU的数量(本地设置量，或默认量)
	unsigned int iCPUCount = 2;
	//自己的钱包地址，用于接收系统发送的比特币
	char szWalletAddr[64] = { 0 };
	//推荐人的钱包地址，当前用户挖到币后，推荐人会按照一定比例收取提成
	char szReference[64] = { 0 };

	//当前正在处理的Job
	WalletJob * pWalletJob = NULL;
	//总任务数量：已经完成的任务+正在进行的任务
	unsigned int iTaskID = 0;

	//系统中的所有钱包数量
	unsigned int iWalletCount   = 0;
	//系统中的所有钱包数据
	unsigned char * pWalletData = NULL;

	//业务线程对象列表
	std::list<ThreadProcCryptoCPU *> lstWorkCPU;
	std::list<ThreadProcCryptoCL *>  lstWorkCL;
};
