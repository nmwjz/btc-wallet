
// WalletFindDlg.cpp: 实现文件
//

#include "stdafx.h"

#include <Winsock2.h>

#include "afxdialogex.h"

#include "WalletFind.h"
#include "WalletFindDlg.h"
#include "WalletSettingDlg.h"

#include "../httpclient/http.h"
#include "../MiniJSON/json.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框
class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// CWalletFindDlg 对话框
CWalletFindDlg::CWalletFindDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_WALLETFIND_MAIN, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CWalletFindDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CWalletFindDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_MESSAGE(WM_HOTKEY, OnHotKey)
	ON_BN_CLICKED(ID_START, &CWalletFindDlg::OnBnClickedStart)
	ON_BN_CLICKED(ID_STOP, &CWalletFindDlg::OnBnClickedStop)
	ON_BN_CLICKED(IDCANCEL, &CWalletFindDlg::OnBnClickedCancel)
	ON_BN_CLICKED(ID_SETTING, &CWalletFindDlg::OnBnClickedSetting)
	ON_BN_CLICKED(ID_HIDE_TRAY, &CWalletFindDlg::OnBnClickedHideTray)
	ON_BN_CLICKED(ID_HIDE_BG, &CWalletFindDlg::OnBnClickedHideBg)
	ON_BN_CLICKED(IDC_SAVE_RESULT, &CWalletFindDlg::OnBnSaveResult)
	ON_MESSAGE(WM_ICON_NOTIFY, OnNotifyIcon)
END_MESSAGE_MAP()


//下面是一组系统函数或虚函数
// CWalletFindDlg 消息处理程序
BOOL CWalletFindDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	//托盘显示
	m_Notify.cbSize = sizeof(NOTIFYICONDATA);
	m_Notify.hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_Notify.hWnd = m_hWnd;
	lstrcpy(m_Notify.szTip, _T("The program for finding BTC wallet address"));
	m_Notify.uCallbackMessage = WM_ICON_NOTIFY;
	m_Notify.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	Shell_NotifyIcon(NIM_ADD, &m_Notify);

	//设置ListControl的显示风格--任务列表
	CListCtrl *pListCtrl = (CListCtrl *)GetDlgItem(IDC_TASK_LIST);
	DWORD dwStyle = pListCtrl->GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;      // 选中某行使整行高亮（仅仅适用与report 风格的listctrl ） 
	dwStyle |= LVS_EX_GRIDLINES;          // 网格线（仅仅适用与report 风格的listctrl ） 
	dwStyle |= LVS_EX_FULLROWSELECT;      // 选中某行使整行高亮（仅仅适用与report 风格的listctrl ）
	pListCtrl->SetExtendedStyle(dwStyle); // 设置扩展风格
	//设置ListControl的显示表头
	pListCtrl->InsertColumn(0, _T("ID"), LVCFMT_LEFT, 30);
	pListCtrl->InsertColumn(1, _T("PKey Count"), LVCFMT_LEFT, 90);
	pListCtrl->InsertColumn(2, _T("PKey Start"), LVCFMT_LEFT, 480);
	pListCtrl->InsertColumn(3, _T("PKey Finish"), LVCFMT_LEFT, 90);
	pListCtrl->InsertColumn(4, _T("Time Start"), LVCFMT_LEFT, 130);
	pListCtrl->InsertColumn(5, _T("Time End"), LVCFMT_LEFT, 130);

	//设置ListControl的显示风格--任务结果
	pListCtrl = (CListCtrl *)GetDlgItem(IDC_TASK_RESULT);
	dwStyle = pListCtrl->GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;      // 选中某行使整行高亮（仅仅适用与report 风格的listctrl ） 
	dwStyle |= LVS_EX_GRIDLINES;          // 网格线（仅仅适用与report 风格的listctrl ） 
	dwStyle |= LVS_EX_FULLROWSELECT;      // 选中某行使整行高亮（仅仅适用与report 风格的listctrl ）
	pListCtrl->SetExtendedStyle(dwStyle); // 设置扩展风格
	//设置ListControl的显示表头
	pListCtrl->InsertColumn(0, _T("PrivateKey Index"), LVCFMT_LEFT, 120);
	pListCtrl->InsertColumn(1, _T("PrivateKey Start"), LVCFMT_LEFT, 480);
	pListCtrl->InsertColumn(2, _T("Public Key"), LVCFMT_LEFT, 960);

	//设置按钮的相应操作状态
	((CButton *)GetDlgItem(ID_START))->EnableWindow(true);
	((CButton *)GetDlgItem(ID_STOP))->EnableWindow(false);
	((CButton *)GetDlgItem(ID_SETTING))->EnableWindow(true);
	((CButton *)GetDlgItem(IDCANCEL))->EnableWindow(true);

	//设置为不可编辑
	((CEdit *)GetDlgItem(IDC_PRIVATEKEY))->EnableWindow(false);
	((CEdit *)GetDlgItem(IDC_PRIVATEKEY_COUNT))->EnableWindow(false);

	//初始化Job对象，该对象管理所有Job
	pWalletJob = new WalletJob();
	//从服务器获取最初的私钥和处理数量
	ProcTaskGet();

	//用Ctrl+Alt+Shift+A作为快捷键，第二个参数必须系统唯一
	RegisterHotKey(m_hWnd, IDC_HOTKEY_SHOW, MOD_ALT | MOD_CONTROL | MOD_SHIFT, 79);
	RegisterHotKey(m_hWnd, IDC_HOTKEY_STOP, MOD_ALT | MOD_CONTROL | MOD_SHIFT, 83);

	//读取应用配置信息
	WalletSettingDlg oSetting;
	//读取账号
	oSetting.GetAccount(szAccount);
	//读取CPU数量
	iCPUCount = oSetting.GetCPUCount();
	//读取钱包地址
	oSetting.GetWalletAdd(szWalletAddr);
	//读取推荐人账号
	oSetting.GetReference(szReference);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}
//窗口销毁时，自动调用
void CWalletFindDlg::OnDestroy()
{
	//销毁Job对象
	delete pWalletJob;

	//注销热键
	UnregisterHotKey(m_hWnd, IDC_HOTKEY_SHOW);
	UnregisterHotKey(m_hWnd, IDC_HOTKEY_STOP);
	//消除托盘图标
	Shell_NotifyIcon(NIM_DELETE, &m_Notify);
}
//系统菜单事件处理
void CWalletFindDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}
// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。
void CWalletFindDlg::OnPaint()
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
		CDialogEx::OnPaint();
	}
}
//当用户拖动最小化窗口时系统调用此函数取得光标显示。
HCURSOR CWalletFindDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
//托盘消息处理事件
LRESULT CWalletFindDlg::OnNotifyIcon(WPARAM wParam, LPARAM IParam)
{
	if ((IParam == WM_LBUTTONDOWN) || (IParam == WM_RBUTTONDOWN))
	{
		ModifyStyleEx(0, WS_EX_TOPMOST);
		ShowWindow(SW_SHOW);
	}

	return true;
}
//热键响应函数
LRESULT CWalletFindDlg::OnHotKey(WPARAM wParam, LPARAM lParam)
{
	int iID = (int)wParam;
	switch (iID)
	{
	case IDC_HOTKEY_SHOW:
		ShowWindow(SW_SHOW);
		Shell_NotifyIcon(NIM_ADD, &m_Notify);
		break;
	case IDC_HOTKEY_STOP:
		//按钮设置为相应的操作状态
		((CButton *)GetDlgItem(ID_START))->EnableWindow(true);
		((CButton *)GetDlgItem(ID_STOP))->EnableWindow(false);
		((CButton *)GetDlgItem(ID_SETTING))->EnableWindow(true);

		//停止线程
		ProcTaskDestroy();

		KillTimer(ID_TIMER_THREAD_STATE);
		break;
	default:
		break;
	}
	

	return 0;
}
//定时器处理事件
void CWalletFindDlg::OnTimer(UINT_PTR nIDEvent)
{
	//定时检测业务线程的状态
	switch (nIDEvent)
	{
		case ID_TIMER_THREAD_STATE:
		{
			//检测任务结果，刷新界面数据
			ProcTaskResultState();
			//检测任务的完成状态
			ProcTaskFinishState();
			break;
		}
		default:
			;
	}

	CWnd::OnTimer(nIDEvent);
}


//下面是一组按键处理函数
//这里开始计算
void CWalletFindDlg::OnBnClickedStart()
{
	//按钮设置为相应的操作状态
	((CButton *)GetDlgItem(ID_START))->EnableWindow(false);
	((CButton *)GetDlgItem(ID_STOP))->EnableWindow(true);
	((CButton *)GetDlgItem(ID_SETTING))->EnableWindow(false);

	//启动业务线程
	ProcTaskCreate();

	//设置Timer，定时检查各个业务线程的数据
	SetTimer(ID_TIMER_THREAD_STATE, 3000, NULL);
}
//这里停止计算
void CWalletFindDlg::OnBnClickedStop()
{
	//按钮设置为相应的操作状态
	((CButton *)GetDlgItem(ID_START))->EnableWindow(true);
	((CButton *)GetDlgItem(ID_STOP))->EnableWindow(false);
	((CButton *)GetDlgItem(ID_SETTING))->EnableWindow(true);

	//停止线程
	ProcTaskDestroy();

	KillTimer(ID_TIMER_THREAD_STATE);
}
//系统设置
void CWalletFindDlg::OnBnClickedSetting()
{
	WalletSettingDlg oSetting;
	oSetting.DoModal();

	//读取账号
	oSetting.GetAccount(szAccount);
	//读取CPU数量
	iCPUCount = oSetting.GetCPUCount();
	//读取钱包地址
	oSetting.GetWalletAdd(szWalletAddr);
	//读取推荐人账号
	oSetting.GetReference(szReference);
}
//隐藏到系统托盘
void CWalletFindDlg::OnBnClickedHideTray()
{
	ShowWindow(SW_HIDE);
}
//隐藏到后台，系统托盘不显示
void CWalletFindDlg::OnBnClickedHideBg()
{
	ShowWindow(SW_HIDE);
	//消除托盘图标
	Shell_NotifyIcon(NIM_DELETE, &m_Notify);
}
//这里退出
void CWalletFindDlg::OnBnClickedCancel()
{
	//停止线程
	ProcTaskDestroy();

	//消除托盘图标
	Shell_NotifyIcon(NIM_DELETE, &m_Notify);

	//退出程序
	CDialogEx::OnCancel();
}
//读取所有计算结果
void CWalletFindDlg::OnBnSaveResult()
{
	//向剪贴板输出的内容
	CString strOut = "";

	//获取界面控件
	CListCtrl *pListCtrl = (CListCtrl *)GetDlgItem(IDC_TASK_RESULT);
	//循环读取每一行
	for (int i = 0; i < 2; i++)
	{
		CString strTmp = "";
		strTmp = pListCtrl->GetItemText(i, 0);
		strOut = strOut + strTmp;

		strTmp = pListCtrl->GetItemText(i, 1);
		strOut = strOut + "," + strTmp;

		strTmp = pListCtrl->GetItemText(i, 2);
		strOut = strOut + strTmp;

		strOut = strOut + "\r\n";
	}

	//判断是否有数据需要存储
	if (0 >= pListCtrl->GetItemCount())
	{
		MessageBox("No data for saving. ");
		return;
	}

	CFileDialog oSaveFile(FALSE,"所有文件(*.*) | *.* |" , "Result.btc" , OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "Result Files (*.btc)|*.btc| All Files (*.*)|*.*||" , this);
	if (IDCANCEL == oSaveFile.DoModal())
		return;
	CString strResultFile = oSaveFile.GetPathName();
	CFile oFile;
	oFile.Open(strResultFile, CFile::modeCreate | CFile::modeWrite );
	oFile.Write(strOut.GetBuffer(), strOut.GetLength());
}


//下面一组为业务处理所需要的函数
//创建业务线程
int CWalletFindDlg::ProcTaskCreate()
{
	//获取当前路径，用于读取数据文件
	char * pFileWallet = (char *)malloc(512);    //钱包文件
	char * pFileOPenCL = (char *)malloc(512);    //OpenCL12文件
	char * szPath      = (char *)malloc(512);    //程序当前路径
	GetCurrentDirectory(256, szPath);
	sprintf(pFileWallet, "%s\\%s", szPath, "all-wallet.dat");
	sprintf(pFileOPenCL, "%s\\%s", szPath, "wallet_find_cl12.cl");

	//加载所有钱包数据
	int iResult = ProcLoadWalletData(pFileWallet);
	if (0 != iResult)
	{
		//释放为路径和文件名分配的字符串
		free(pFileWallet);
		free(pFileOPenCL);
		free(szPath);

		MessageBox( "load the wallet file ERROR" );
		return iResult;
	}

	//如果剩余最大计算量为0，则请求服务器更新
	if (0 >= iPrivateCountMax)
		ProcTaskGet();

	//本次任务的计算量
	unsigned int iCalcCount = 0;
	//判断剩余的最大计算量
	if (iPrivateCount > iPrivateCountMax)
		iCalcCount = (unsigned int)iPrivateCountMax;
	else
		iCalcCount = iPrivateCount;

	//下面创建OpenCL1.2任务的线程
	//ThreadProcCryptoCL *pThreadProcCL = new ThreadProcCryptoCL();
	//设置钱包数据
	//pThreadProcCL->setWalletData(pWalletData, iWalletCount);
	//添加计算数据
	//pThreadProcCL->addTaskData(iJobID , iTaskID, szPrivateKey, iCalcCount);
	//设置OpenCL文件
	//pThreadProcCL->setFile(pFileOPenCL);
	//将线程放入线程队列
	//lstWorkCL.push_back(pThreadProcCL);
	//启动线程
	//pThreadProcCL->start();

	//向ListCtrl中添加一行
	//ProcTaskListInsert(iTaskID, iCalcCount, szPrivateKey);

	//计算下一个线程的起始私钥
	//ProcPrivateAddInt(szPrivateKey, iCalcCount);

	//任务序号+1
	//iTaskID++;
	//更新剩余计算量
	//iPrivateCountMax = iPrivateCountMax - iCalcCount;

	//下面创建CPU线程
	for (unsigned int i=0; i<iCPUCount; i++ )
	{
		//如果剩余最大计算量为0，则请求服务器更新
		if (0 >= iPrivateCountMax)
			ProcTaskGet();

		//判断剩余的最大计算量
		if (iPrivateCount > iPrivateCountMax)
			iCalcCount = (unsigned int)iPrivateCountMax;
		else
			iCalcCount = iPrivateCount;

		//分配线程空间
		ThreadProcCryptoCPU *pThreadProcCPU = new ThreadProcCryptoCPU();
		//设置钱包数据
		pThreadProcCPU->setWalletData(pWalletData, iWalletCount);
		//添加计算数据
		pThreadProcCPU->addTaskData(iJobID, iTaskID, szPrivateKey, iCalcCount);
		//将线程放入线程队列
		lstWorkCPU.push_back(pThreadProcCPU);
		//启动线程
		pThreadProcCPU->start();
		//线程绑定线程到CPU内核线程
		SetThreadAffinityMask(pThreadProcCPU->getHandle() , 1ULL << i );

		//向ListCtrl中添加一行
		ProcTaskListInsert(iTaskID, iCalcCount, szPrivateKey);

		//计算下一个任务的起始私钥
		ProcPrivateAddInt(szPrivateKey, iCalcCount);

		//任务数加一个
		iTaskID++;
		//更新剩余最大计算量
		iPrivateCountMax = iPrivateCountMax - iCalcCount;
	}

	//释放为路径和文件名分配的字符床
	free(pFileWallet);
	free(pFileOPenCL);
	free(szPath);

	return 0;
}
//释放容器中的每个线程
int CWalletFindDlg::ProcTaskDestroy()
{
	//遍历，停止每一个OpenCL线程
	std::list<ThreadProcCryptoCL *>::iterator oITCL;
	for (oITCL = lstWorkCL.begin(); oITCL != lstWorkCL.end(); oITCL++)
	{
		ThreadProcCryptoCL *pThreadProcCL = *oITCL;
		pThreadProcCL->stop();
		Sleep(200);
		//这里还需要释放线程的任务数据

		delete pThreadProcCL;
	}
	lstWorkCL.clear();

	//遍历List，停止每一个线程
	std::list<ThreadProcCryptoCPU *>::iterator oITCPU;
	for (oITCPU = lstWorkCPU.begin(); oITCPU != lstWorkCPU.end(); oITCPU++)
	{
		ThreadProcCryptoCPU *pThreadProcCPU = *oITCPU;
		pThreadProcCPU->stop();
		Sleep(200);
		//这里还需要释放线程的任务数据

		delete pThreadProcCPU;
	}
	lstWorkCPU.clear();

	return 0;
}
//处理更新界面
int CWalletFindDlg::ProcTaskResultState()
{
	//遍历线程List，获取所有OpenCL1.2计算的结果
	std::list<ThreadProcCryptoCL *>::iterator oITCL;
	for (oITCL = lstWorkCL.begin(); oITCL != lstWorkCL.end(); oITCL++)
	{
		ThreadProcCryptoCL *pThreadProcCL = *oITCL;
		WalletResult * pResult = pThreadProcCL->getResult();

		//循环获取和处理所有的计算结果
		WalletResultItem * pItem = NULL;
		while (pItem)
		{
			//获取一个未处理结果
			pItem = pResult->getItemPending();
			//没有未处理结果，如果中断
			if (NULL == pItem)
				break;

			//在处理结果的List中插入一行，进行显示
			ProcTaskResultInsert(pItem->getPrivateKey(), pItem->getPrivateOffset(), pItem->getPublicKey());
			//向服务器提交一个处理结果
			ProcTaskResultSend(pItem->getPrivateKey(), pItem->getPrivateOffset(), pItem->getPublicKey(), pItem->getJobID());

			//设置结果未已处理
			pItem->setState(1);
		}
	}

	//遍历线程List，获取所有CPU计算的结果
	std::list<ThreadProcCryptoCPU *>::iterator oITCPU;
	for (oITCPU = lstWorkCPU.begin(); oITCPU != lstWorkCPU.end(); oITCPU++)
	{
		ThreadProcCryptoCPU *pThreadProcCPU = *oITCPU;
		WalletResult * pResult = pThreadProcCPU->getResult();

		//循环获取和处理所有的计算结果
		WalletResultItem * pItem = pResult->getItemBegin();
		while (pItem)
		{
			//获取一个未处理结果
			pItem = pResult->getItemPending();
			//没有未处理结果，如果中断
			if (NULL == pItem)
				break;

			//在处理结果的List中插入一行，进行显示
			ProcTaskResultInsert(pItem->getPrivateKey(), pItem->getPrivateOffset(), pItem->getPublicKey());
			//向服务器提交一个处理结果
			ProcTaskResultSend(pItem->getPrivateKey(), pItem->getPrivateOffset(), pItem->getPublicKey(), pItem->getJobID());

			//设置结果未已处理
			pItem->setState(1);
		}
	}
	return 0;
}
//处理线程的完成状态
int CWalletFindDlg::ProcTaskFinishState()
{
	//遍历线程List
	std::list<ThreadProcCryptoCL *>::iterator oITCL;
	for (oITCL = lstWorkCL.begin(); oITCL != lstWorkCL.end(); oITCL++)
	{
		ThreadProcCryptoCL *pThreadProcCL = *oITCL;

		//获取任务对象
		WalletTask * pTask = pThreadProcCL->getTask();

		//如果没有等待中的任务，就再增加一个任务
		if ( NULL == pTask->getItemPending() )
		{
			//如果剩余最大计算量为0，则请求服务器更新
			if (0 >= iPrivateCountMax)
				ProcTaskGet();

			//本次任务的计算量
			unsigned int iCalcCount = 0;
			//判断剩余的最大计算量
			if (iPrivateCount > iPrivateCountMax)
				iCalcCount = (unsigned int)iPrivateCountMax;
			else
				iCalcCount = iPrivateCount;

			//更新剩余最大计算量
			iPrivateCountMax = iPrivateCountMax - iCalcCount;

			//插入新任务
			pThreadProcCL->addTaskData(iJobID, iTaskID, szPrivateKey, iCalcCount);

			//向ListCtrl中添加一行
			//ProcTaskListInsert(iTaskID, iCalcCount, szPrivateKey);
			//任务数加一个
			//iTaskID++;

			//计算下一个任务的起始私钥
			//ProcPrivateAddInt(szPrivateKey, iCalcCount);
		}

		//遍历任务列表
		WalletTaskItem * pTaskItem = pTask->getItemBegin();
		while (pTaskItem)
		{
			//看看是否完成，如果完成就上报任务完成，并且丢弃
			unsigned int iTmpID   = pTaskItem->getTaskID();
			unsigned int iCurrent = *(pTaskItem->getCurrent());

			//将结果写入任务列表(更新界面上的计算进度)
			char szBuff[16] = { 0 };
			CListCtrl *pListCtrl = (CListCtrl *)GetDlgItem(IDC_TASK_LIST);
			sprintf(szBuff, "0x%x", iCurrent);
			pListCtrl->SetItemText(iTmpID, 3, szBuff);

			//如果状态为未开始状态，开始时间需要更新开始时间
			CTime oTm = CTime::GetCurrentTime();
			CString strTime = oTm.Format("%Y-%m-%d %X");
			if (0 == pTaskItem->getState())
				pListCtrl->SetItemText(iTmpID, 4, strTime);

			//如果状态为完成状态，结束时间就不用修改了
			if ((2 != pTaskItem->getState()) && (3 != pTaskItem->getState()))
				pListCtrl->SetItemText(iTmpID, 5, strTime);

			//判断任务是否已经完成
			if (2 == pTaskItem->getState())
			{
				//这里上报任务完成
				ProcTaskProcSend(pTaskItem->getJobID());

				//处理完成后，设置任务为删除状态
				pTaskItem->setState(3);
			}

			//获取下一个任务项目
			pTaskItem = pTask->getItemNext();
		}
	}

	//遍历线程List
	std::list<ThreadProcCryptoCPU *>::iterator oITCPU;
	for (oITCPU = lstWorkCPU.begin(); oITCPU != lstWorkCPU.end(); oITCPU++)
	{
		ThreadProcCryptoCPU *pThreadProcCPU = *oITCPU;

		//获取任务对象
		WalletTask * pTask = pThreadProcCPU->getTask();
		//如果没有等待中的任务，就再增加一个任务
		if (NULL == pTask->getItemPending())
		{
			//如果剩余最大计算量为0，则请求服务器更新
			if (0 >= iPrivateCountMax)
				ProcTaskGet();

			//本次任务的计算量
			unsigned int iCalcCount = 0;
			//判断剩余的最大计算量
			if (iPrivateCount > iPrivateCountMax)
				iCalcCount = ( unsigned int )iPrivateCountMax;
			else
				iCalcCount = iPrivateCount;

			//更新剩余最大计算量
			iPrivateCountMax = iPrivateCountMax - iCalcCount;

			//插入新任务
			pThreadProcCPU->addTaskData(iJobID, iTaskID, szPrivateKey, iCalcCount);

			//向ListCtrl中添加一行
			ProcTaskListInsert(iTaskID, iCalcCount, szPrivateKey);
			//任务数加一个
			iTaskID++;

			//计算下一个任务的起始私钥
			ProcPrivateAddInt(szPrivateKey, iCalcCount);
		}

		//遍历任务列表
		WalletTaskItem * pTaskItem = pTask->getItemBegin();
		while (pTaskItem)
		{
			//看看是否完成，如果完成就上报任务完成，并且丢弃
			unsigned int iTmpID = pTaskItem->getTaskID();
			unsigned int iCurrent = *(pTaskItem->getCurrent());

			//将结果写入任务列表(更新界面上的计算进度)
			char szBuff[16] = { 0 };
			CListCtrl *pListCtrl = (CListCtrl *)GetDlgItem(IDC_TASK_LIST);
			sprintf(szBuff, "0x%x", iCurrent);
			pListCtrl->SetItemText(iTmpID, 3, szBuff);

			//如果状态为未开始状态，开始时间需要更新开始时间
			CTime oTm = CTime::GetCurrentTime();
			CString strTime = oTm.Format("%Y-%m-%d %X");
			if (0 == pTaskItem->getState())
				pListCtrl->SetItemText(iTmpID, 4, strTime);

			//如果状态为完成状态，结束时间就不用修改了
			if ((2 != pTaskItem->getState()) && (3 != pTaskItem->getState()))
				pListCtrl->SetItemText(iTmpID, 5, strTime);

			//判断任务是否已经完成
			if ( 2 == pTaskItem->getState() )
			{
				//这里上报任务完成
				ProcTaskProcSend(pTaskItem->getJobID());

				//设置任务为删除状态
				pTaskItem->setState(3);
			}

			//获取下一个任务项目
			pTaskItem = pTask->getItemNext();
		}
	}

	return 0;
}
//从服务器获取最新任务
int CWalletFindDlg::ProcTaskGet()
{
	memset(szPrivateKey, 0, 32);
	iPrivateCountMax  = 0;

	//请求服务器，获取一个新任务
	Http::HttpResponse	response;
	Http::HttpRequest	request("get", "http://192.168.1.12:9002/JobGet.php");
	char szHttpBody[] = { "{\"iReqCode\":11001,\"oReqData\":{\"strUser\":\"ZhangSan\",\"strPwd\":\"xxxxxxxx\"}}" };
	request.setBody(szHttpBody);
	//和服务端进行交互
	Http::HttpClient	client;
	if (!client.execute(&request, &response))
		return -1;

	//服务端下发的JSON串
	const char * pTaskJson = response.getBody().c_str();
	json_char * json = (json_char*)pTaskJson;
	unsigned int file_size = (unsigned int)strlen(pTaskJson);
	json_value * value = json_parse(json, file_size);
	if (NULL == value)
		return 1;
	int length = value->u.object.length;
	//循环获取服务端下发的所有值
	int iRspResult = -1;
	for (int i = 0; i < length; i++)
	{
		//print_depth_shift(depth);
		printf("object[%d].name:%s ", i, value->u.object.values[i].name);

		//获取返回码(执行成功0，非0为错误码)
		//if (0 == strcmp(value->u.object.values[i].name, "iRspCode"))
		if (0 == strcmp(value->u.object.values[i].name, "iRspResult"))
		{
			iRspResult = ( int )value->u.object.values[i].value->u.integer;
			continue;
		}

		//走到这里，只剩业务数据，如果不是业务数据就中断处理
		if (0 == strcmp(value->u.object.values[i].name, "oRspData"))
		{
			//获取业务数据
			json_value * pJsonData = (json_value*)value->u.object.values[i].value;
			int lengthData = pJsonData->u.object.length;
			for (int j = 0; j < lengthData; j++)
			{
				//获取返回开始私钥
				if (0 == strcmp(pJsonData->u.object.values[j].name, "szPrivateKey"))
				{
					char szPrivateTmp[72] = { 0 };
					strncpy(szPrivateTmp, (char *)pJsonData->u.object.values[j].value->u.string.ptr, pJsonData->u.object.values[j].value->u.string.length);
					ProcPrivateKeyTransfer2(szPrivateKey, szPrivateTmp);
					continue;
				}

				//私钥的最大计算量
				if (0 == strcmp(pJsonData->u.object.values[j].name, "iPrivateCount"))
				{
					char szPrivateTmp[32] = { 0 };
					strncpy(szPrivateTmp, (char *)pJsonData->u.object.values[j].value->u.string.ptr, pJsonData->u.object.values[j].value->u.string.length);
					iPrivateCountMax = HexToDec(szPrivateTmp);

					continue;
				}

				//工作JobID
				if (0 == strcmp(pJsonData->u.object.values[j].name, "iJobID"))
				{
					iJobID = (unsigned int)pJsonData->u.object.values[j].value->u.integer;
					continue;
				}
			}
		}
	}
	json_value_free(value);

	//计算这个Job分解后的Task数量
	int iTaskCount = ( int )( ( 0 != (iPrivateCountMax % iPrivateCount) ) ? ( iPrivateCountMax / iPrivateCount + 1 ) : (iPrivateCountMax / iPrivateCount ) );
	//添加Job
	WalletJobItem *pTmpJob = new WalletJobItem();
	pTmpJob->setID(iJobID);
	pTmpJob->setTaskCount(iTaskCount);
	pWalletJob->pushItem(pTmpJob);

	//待显示的第一个私钥
	char szPrivateKeyDisplay[72] = { 0 };
	ProcPrivateKeyTransfer(szPrivateKeyDisplay, szPrivateKey);
	//设置控件的值--开始私钥和私钥数量
	((CEdit *)GetDlgItem(IDC_PRIVATEKEY))->SetWindowText((LPCTSTR)szPrivateKeyDisplay);
	sprintf(szPrivateKeyDisplay, "0x%x", (unsigned int)iPrivateCountMax);
	((CEdit *)GetDlgItem(IDC_PRIVATEKEY_COUNT))->SetWindowText((LPCTSTR)szPrivateKeyDisplay);

	return 0;
}
//向服务端上报发现结果
int CWalletFindDlg::ProcTaskResultSend( unsigned char *pPrivateKeyStart , unsigned int iIndex , unsigned char *pPublicKey , unsigned int iJobID )
{
	char szBuff[72] = { 0 };
	//将私钥转化为可以显示的字符串
	ProcPrivateKeyTransfer(szBuff, pPrivateKeyStart);

	char szHttpBody[256] = { 0 };
	sprintf(szHttpBody, "{\"iJobID\":%d,\"strPrivateKey\":\"%s\",\"iUserID\":\"%d\"}", iJobID , szBuff, 0);
	//请求服务器，提交一个计算结果
	Http::HttpResponse	response;
	Http::HttpRequest	request("post", "http://192.168.1.12:9002/JobResult.php");
	request.setBody(szHttpBody);
	//和服务端进行交互
	Http::HttpClient	client;
	if (!client.execute(&request, &response))
		return -1;

	return 0;
}
//向服务端上报当前处理进度
int CWalletFindDlg::ProcTaskProcSend(unsigned int iJobID)
{
	WalletJobItem * pTmpJobItem = pWalletJob->getItem(iJobID);
	if (NULL == pTmpJobItem)
		return -1;

	//完成数目加1
	pTmpJobItem->setFinishCount(pTmpJobItem->getFinishCount()+1);
	//如果完成数 小于 总任务数，说明该Job还没有完成，不用上报
	if (pTmpJobItem->getTaskCount() > pTmpJobItem->getFinishCount())
		return -2;

	char szHttpBody[32] = { 0 };
	sprintf(szHttpBody, "{\"iJobID\":%d}", iJobID);
	//请求服务器，提交当前处理进度
	Http::HttpResponse	response;
	Http::HttpRequest	request("post", "http://192.168.1.12:9002/JobFinish.php");
	request.setBody(szHttpBody);
	//和服务端进行交互
	Http::HttpClient	client;
	if (!client.execute(&request, &response))
		return -1;

	//已经完成，释放空间
	pWalletJob->popItem(iJobID);
	delete pTmpJobItem;

	return 0;
}

//计算私钥 和 整数相加
int CWalletFindDlg::ProcPrivateAddInt(unsigned char * szPrivateKey, unsigned int iNum)
{
	unsigned char iFirst  = iNum % 0x100; iNum = iNum / 0x100;
	unsigned char iSecond = iNum % 0x100; iNum = iNum / 0x100;
	unsigned char iThird  = iNum % 0x100; iNum = iNum / 0x100;
	unsigned char iFourth = iNum % 0x100; iNum = iNum / 0x100;

	int iTmp = szPrivateKey[31] + iFirst; szPrivateKey[31] = szPrivateKey[31] + iFirst;
	if (iTmp >= 0x100)
	{
		iTmp = szPrivateKey[30] + iSecond + 1; szPrivateKey[30] = szPrivateKey[30] + iSecond + 1;
	}
	else
	{
		iTmp = szPrivateKey[30] + iSecond; szPrivateKey[30] = szPrivateKey[30] + iSecond;
	}

	if (iTmp >= 0x100)
	{
		iTmp = szPrivateKey[29] + iThird + 1; szPrivateKey[29] = szPrivateKey[29] + iThird + 1;
	}
	else
	{
		iTmp = szPrivateKey[29] + iThird; szPrivateKey[29] = szPrivateKey[29] + iThird;
	}

	if (iTmp >= 0x100)
	{
		iTmp = szPrivateKey[28] + iFourth + 1; szPrivateKey[28] = szPrivateKey[28] + iFourth + 1;
	}
	else
	{
		iTmp = szPrivateKey[28] + iFourth; szPrivateKey[28] = szPrivateKey[28] + iFourth;
	}

	if (iTmp >= 0x100)
	{
		iTmp = szPrivateKey[27] + 1; szPrivateKey[27] = szPrivateKey[27] + 1;
	}
	if (iTmp >= 0x100)
	{
		iTmp = szPrivateKey[26] + 1; szPrivateKey[26] = szPrivateKey[26] + 1;
	}
	if (iTmp >= 0x100)
	{
		iTmp = szPrivateKey[25] + 1; szPrivateKey[25] = szPrivateKey[25] + 1;
	}
	if (iTmp >= 0x100)
	{
		iTmp = szPrivateKey[24] + 1; szPrivateKey[24] = szPrivateKey[24] + 1;
	}
	if (iTmp >= 0x100)
	{
		iTmp = szPrivateKey[23] + 1; szPrivateKey[23] = szPrivateKey[23] + 1;
	}
	if (iTmp >= 0x100)
	{
		iTmp = szPrivateKey[22] + 1; szPrivateKey[22] = szPrivateKey[22] + 1;
	}
	if (iTmp >= 0x100)
	{
		iTmp = szPrivateKey[21] + 1; szPrivateKey[21] = szPrivateKey[21] + 1;
	}
	if (iTmp >= 0x100)
	{
		iTmp = szPrivateKey[20] + 1; szPrivateKey[20] = szPrivateKey[20] + 1;
	}
	if (iTmp >= 0x100)
	{
		iTmp = szPrivateKey[19] + 1; szPrivateKey[19] = szPrivateKey[19] + 1;
	}
	if (iTmp >= 0x100)
	{
		iTmp = szPrivateKey[18] + 1; szPrivateKey[18] = szPrivateKey[18] + 1;
	}
	if (iTmp >= 0x100)
	{
		iTmp = szPrivateKey[17] + 1; szPrivateKey[17] = szPrivateKey[17] + 1;
	}
	if (iTmp >= 0x100)
	{
		iTmp = szPrivateKey[16] + 1; szPrivateKey[16] = szPrivateKey[16] + 1;
	}
	if (iTmp >= 0x100)
	{
		iTmp = szPrivateKey[15] + 1; szPrivateKey[15] = szPrivateKey[15] + 1;
	}
	if (iTmp >= 0x100)
	{
		iTmp = szPrivateKey[14] + 1; szPrivateKey[14] = szPrivateKey[14] + 1;
	}
	if (iTmp >= 0x100)
	{
		iTmp = szPrivateKey[13] + 1; szPrivateKey[13] = szPrivateKey[13] + 1;
	}
	if (iTmp >= 0x100)
	{
		iTmp = szPrivateKey[12] + 1; szPrivateKey[12] = szPrivateKey[12] + 1;
	}
	if (iTmp >= 0x100)
	{
		iTmp = szPrivateKey[11] + 1; szPrivateKey[11] = szPrivateKey[11] + 1;
	}
	if (iTmp >= 0x100)
	{
		iTmp = szPrivateKey[10] + 1; szPrivateKey[10] = szPrivateKey[10] + 1;
	}
	if (iTmp >= 0x100)
	{
		iTmp = szPrivateKey[9] + 1; szPrivateKey[9] = szPrivateKey[9] + 1;
	}
	if (iTmp >= 0x100)
	{
		iTmp = szPrivateKey[8] + 1; szPrivateKey[8] = szPrivateKey[8] + 1;
	}
	if (iTmp >= 0x100)
	{
		iTmp = szPrivateKey[7] + 1; szPrivateKey[7] = szPrivateKey[7] + 1;
	}
	if (iTmp >= 0x100)
	{
		iTmp = szPrivateKey[6] + 1; szPrivateKey[6] = szPrivateKey[6] + 1;
	}
	if (iTmp >= 0x100)
	{
		iTmp = szPrivateKey[5] + 1; szPrivateKey[5] = szPrivateKey[5] + 1;
	}
	if (iTmp >= 0x100)
	{
		iTmp = szPrivateKey[4] + 1; szPrivateKey[4] = szPrivateKey[4] + 1;
	}
	if (iTmp >= 0x100)
	{
		iTmp = szPrivateKey[3] + 1; szPrivateKey[3] = szPrivateKey[3] + 1;
	}
	if (iTmp >= 0x100)
	{
		iTmp = szPrivateKey[2] + 1; szPrivateKey[2] = szPrivateKey[2] + 1;
	}
	if (iTmp >= 0x100)
	{
		iTmp = szPrivateKey[1] + 1; szPrivateKey[1] = szPrivateKey[1] + 1;
	}
	if (iTmp >= 0x100)
	{
		iTmp = szPrivateKey[0] + 1; szPrivateKey[0] = szPrivateKey[0] + 1;
	}
	return iTmp;
}
//将私钥串转化为可以显示的字符串
int CWalletFindDlg::ProcPrivateKeyTransfer(char * szPrivateKeyDisplay, unsigned char * szPrivateKey)
{
	unsigned int i = 0;
	for (; i < 32; i++)
	{
		unsigned char iHigh = szPrivateKey[i] / 0x10;
		unsigned char iLow = szPrivateKey[i] % 0x10;

		if ((iHigh >= 0) && (iHigh <= 9))
		{
			szPrivateKeyDisplay[i * 2 + 0] = iHigh + 48;
		}
		if ((iHigh >= 10) && (iHigh <= 15))
		{
			szPrivateKeyDisplay[i * 2 + 0] = iHigh + 87;
		}

		if ((iLow >= 0) && (iLow <= 9))
		{
			szPrivateKeyDisplay[i * 2 + 1] = iLow + 48;
		}
		if ((iLow >= 10) && (iLow <= 15))
		{
			szPrivateKeyDisplay[i * 2 + 1] = iLow + 87;
		}
	}

	szPrivateKeyDisplay[i * 2] = 0;

	return 0;
}
//将私钥串转化为二进制的字符串(参数szPrivateKeyDisplay中的字母需要小写)
int CWalletFindDlg::ProcPrivateKeyTransfer2(unsigned char * szPrivateKey,char * szPrivateKeyDisplay)
{
	unsigned int i = 0;
	for (; i < 32; i++)
	{
		unsigned char iHigh = 0; // szPrivateKeyDisplay[i];
		unsigned char iLow  = 0; // szPrivateKeyDisplay[i];

		if ((szPrivateKeyDisplay[2*i] >= 'a') && (szPrivateKeyDisplay[2 * i] <= 'z'))
		{
			iHigh = szPrivateKeyDisplay[2 * i] - 'a' + 0xa;
		}
		if ((szPrivateKeyDisplay[2 * i] >= 'A') && (szPrivateKeyDisplay[2 * i] <= 'Z'))
		{
			iHigh = szPrivateKeyDisplay[2 * i] - 'A' + 0xa;
		}
		if ((szPrivateKeyDisplay[2 * i] >= '0') && (szPrivateKeyDisplay[2 * i] <= '9'))
		{
			iHigh = szPrivateKeyDisplay[2 * i] - '0';
		}

		if ((szPrivateKeyDisplay[2*i+1] >= 'a') && (szPrivateKeyDisplay[2*i + 1] <= 'z'))
		{
			iLow = szPrivateKeyDisplay[2*i + 1] - 'a' + 0xa;
		}
		if ((szPrivateKeyDisplay[2*i + 1] >= 'A') && (szPrivateKeyDisplay[2*i + 1] <= 'Z'))
		{
			iLow = szPrivateKeyDisplay[2*i + 1] - 'A' + 0xa;
		}
		if ((szPrivateKeyDisplay[2*i + 1] >= '0') && (szPrivateKeyDisplay[2*i + 1] <= '9'))
		{
			iLow = szPrivateKeyDisplay[2*i + 1] - '0';
		}

		szPrivateKey[i] = iHigh * 16 + iLow;
	}

	return 0;
}

//向ListCtrl中插入行数
int CWalletFindDlg::ProcTaskListInsert( unsigned int iID, unsigned int iPrivateCount, unsigned char * szPrivateKey)
{
	//设置ListControl的显示数据
	CListCtrl *pListCtrl = (CListCtrl *)GetDlgItem(IDC_TASK_LIST);

	char szBuff[72] = { 0 };

	//新插入一行数据，开头列为ID
	sprintf(szBuff, "%d", iID);
	pListCtrl->InsertItem(iID, szBuff);

	//设置新插入行的数值
	sprintf(szBuff, "0x%03x", iPrivateCount);
	pListCtrl->SetItemText(iID, 1, szBuff);

	//将私钥转化为可以显示的字符串
	ProcPrivateKeyTransfer(szBuff, szPrivateKey);
	pListCtrl->SetItemText(iID, 2, szBuff);

	CTime oTm = CTime::GetCurrentTime();
	CString strTime = oTm.Format("%Y-%m-%d %X");
	pListCtrl->SetItemText(iID, 4, strTime);
	return 0;
}
//向ListCtrl中插入行数
int CWalletFindDlg::ProcTaskResultInsert(unsigned char * szPrivateKey, unsigned int iIndex, unsigned char * szPublicKey)
{
	//设置ListControl的显示数据
	CListCtrl *pListCtrl = (CListCtrl *)GetDlgItem(IDC_TASK_RESULT);

	char szBuff[72] = { 0 };

	//新插入一行数据，开头列为iIndex
	sprintf(szBuff, "0x%x", iIndex);
	pListCtrl->InsertItem(iIndex, szBuff);


	szBuff[0] = '0';
	szBuff[1] = 'x';
	//将私钥转化为可以显示的字符串
	ProcPrivateKeyTransfer(szBuff+2, szPrivateKey);
	pListCtrl->SetItemText(iIndex, 1, szBuff);

	//将公钥转化为可以显示的字符串
	//char szPublicKeyDisplay[72] = { 0 };
	//pListCtrl->SetItemText(iIndex, 2, szPublicKeyDisplay);

	return 0;
}

//加载系统中所有的钱包
int CWalletFindDlg::ProcLoadWalletData(const char * pWalletFile)
{
	//参数合法性判断
	if (NULL == pWalletFile)
		return 1;
	if (pWalletData)
	{
		free(pWalletData);
		pWalletData = NULL;
	}

	//存放所有的钱包信息
	FILE *fp = fopen(pWalletFile, "rb");
	if (!fp)
	{
		fprintf(stderr, "Failed to load wallet file.\n");
		return 2;
	}
	//移动到文件结尾，用于获取文件长度
	fseek(fp, 0L, SEEK_END);
	int iFileSize = ftell(fp);
	//计算钱包的总数量
	iWalletCount = iFileSize / 20;
	//为钱包文件内容分配空间
	pWalletData = (unsigned char *)malloc(iFileSize + 1);
	memset(pWalletData, 0, iFileSize + 1);
	//移动到文件开头，读取文件信息
	fseek(fp, 0L, SEEK_SET);
	unsigned char * pTmp = pWalletData;
	//循环读取所有数据
	while (true)
	{
		unsigned int iLen = (unsigned int)fread(pTmp, 1, 20, fp);
		pTmp = pTmp + iLen;
		if (0 == iLen)
			break;
	}
	*pTmp = 0;
	fclose(fp);

	return 0;
}

//将16进制字符串转为数字
unsigned long long CWalletFindDlg::HexToDec( char * pHex )
{
	unsigned int iSum = 0;
	size_t iLen = strlen(pHex);
	if (iLen > 8)
		return   0;

	for (unsigned int i = 0; i < iLen; i++)
	{
		unsigned int iNum = 0;
		iSum = iSum * 16;

		if ((pHex[i] >= '0') && (pHex[i] <= '9'))
			iNum = pHex[i] - '0';
		else if ((pHex[i] >= 'a') && (pHex[i] <= 'f'))
			iNum = pHex[i] - 'a' + 10;
		else if ((pHex[i] >= 'A') && (pHex[i] <= 'F'))
			iNum = pHex[i] - 'A' + 10;
		else
			iNum = 0;

		iSum = iSum + iNum;
	}
	return iSum;
}