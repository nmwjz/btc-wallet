
// WalletDataTestDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "WalletDataTest.h"
#include "WalletDataTestDlg.h"
#include "afxdialogex.h"

#include "ecmult_gen_impl.h"
#include "sha256.h"
#include "rmd160.h"
#include "Base58.h"

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


// CWalletDataTestDlg 对话框



CWalletDataTestDlg::CWalletDataTestDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_WALLETDATATEST_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CWalletDataTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CWalletDataTestDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_BROWSE_WALLETFILE, &CWalletDataTestDlg::OnBnClickedBtnBrowseWalletfile)
	ON_BN_CLICKED(IDC_BTN_INSERT_TESTDATA, &CWalletDataTestDlg::OnBnClickedBtnInsertTestdata)
END_MESSAGE_MAP()


// CWalletDataTestDlg 消息处理程序

BOOL CWalletDataTestDlg::OnInitDialog()
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CWalletDataTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CWalletDataTestDlg::OnPaint()
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

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CWalletDataTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


//将字符串的私钥转化为16进制
void TransferPrivate2Hex(unsigned char * pPrivate, char * szPrivate)
{
	//逐位转化
	for (int i = 0; i < 32; i++)
	{
		//计算高位
		int iHigh = 0;    //高位
		if (('a' <= *szPrivate) && (*szPrivate <= 'f'))
			iHigh = *szPrivate - 'a' + 0xa;
		if (('A' <= *szPrivate) && (*szPrivate <= 'F'))
			iHigh = *szPrivate - 'A' + 0xa;
		if (('0' <= *szPrivate) && (*szPrivate <= '9'))
			iHigh = *szPrivate - '0';

		//计算低位
		int iLow = 0;    //低位
		szPrivate++;
		if (('a' <= *szPrivate) && (*szPrivate <= 'f'))
			iLow = *szPrivate - 'a' + 0xa;
		if (('A' <= *szPrivate) && (*szPrivate <= 'F'))
			iLow = *szPrivate - 'A' + 0xa;
		if (('0' <= *szPrivate) && (*szPrivate <= '9'))
			iLow = *szPrivate - '0';

		//目标
		*pPrivate = iHigh * 16 + iLow;

		//计算下一个
		szPrivate++;
		pPrivate++;
	}
}
//从文件读取20字节
int ReadFile(FILE *fp, unsigned char * pBuff)
{
	int iSum = 0;
	while (true)
	{
		int iLen = (int)fread(pBuff, 1, 20 - iSum, fp);
		if (0 >= iLen)
			break;

		iSum = iSum + iLen;
		if (iSum >= 20)
			break;
	}
	return iSum;
}
//将钱包地址插入目标文件
void InsertIntoFile(char * pFilePathName, unsigned char * pWalletAddress)
{
	unsigned char * pWallets = NULL;
	unsigned char * pBuff = (unsigned char *)malloc(40);
	unsigned char * pTmp;

	//打开文件
	FILE *fp = fopen(pFilePathName, "rb+");

	//取得文件大小
	fseek(fp, 0, SEEK_END);
	long iFileSize = ftell(fp);

	//分配控件
	pWallets = (unsigned char *)malloc(iFileSize + 40);
	memset(pWallets, 0, iFileSize + 40);
	//移动到文件开头
	fseek(fp, 0, SEEK_SET);
	pTmp = pWallets;

	bool bFlag = false;
	//循环处理每一个钱包
	while (1)
	{
		memset(pBuff, 0, 40);
		//读取一个钱包
		//size_t iRead = fread(pBuff, 20, 1, fp);
		int iRead = ReadFile(fp, pBuff);
		if (20 > iRead)
			break;

		//比较钱包
		if ((memcmp(pWalletAddress, pBuff, 20) <= 0) && (!bFlag))
		{
			//插入一个钱包
			memcpy(pTmp, pWalletAddress, 20);

			//下移一个钱包位置
			pTmp = pTmp + 20;

			//发现标志设为true
			bFlag = true;
		}

		//将原有的钱包数据Copy到目标
		memcpy(pTmp, pBuff, 20);
		//下移一个钱包位置
		pTmp = pTmp + 20;
	}

	//移动到文件开头
	fseek(fp, 0, SEEK_SET);
	//移动到数据开头
	pTmp = pWallets;

	//将文件内容写入文件
	for (int i = 0; i < iFileSize / 20 + 1; i++)
	{
		//每次写一个钱包数据
		fwrite(pTmp, 20, 1, fp);
		//下移一个钱包位置
		pTmp = pTmp + 20;
	}
	fclose(fp);

	free(pBuff);
	free(pWallets);
}

//浏览钱包文件
void CWalletDataTestDlg::OnBnClickedBtnBrowseWalletfile()
{
	CString strExtName;
	CString strDefaultName;
	DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	CString strFilter;

	strExtName = "All Files (*.*)";
	strDefaultName = "*";
	strFilter = "All Files (*.*)";

	//获取输入文件的路径和名称
	CString strDstFile;
	CFileDialog *pOpenBTCDataFile = new CFileDialog(true, strExtName, strDefaultName, dwFlags, strFilter, this);
	if (IDOK == pOpenBTCDataFile->DoModal())
		strDstFile = pOpenBTCDataFile->GetPathName();
	else
		strDstFile = "";
	delete pOpenBTCDataFile;

	//设置控件的显示值
	CEdit *pWalletFile = (CEdit *)GetDlgItem(IDC_TXT_WALLETFILE);
	pWalletFile->SetWindowText(strDstFile);
}
//向钱包文件插入一个测试钱包地址
void CWalletDataTestDlg::OnBnClickedBtnInsertTestdata()
{
	CString strWalletFile;
	//获取控件的显示值
	CEdit *pWalletFile = (CEdit *)GetDlgItem(IDC_TXT_WALLETFILE);
	pWalletFile->GetWindowText(strWalletFile);
	if (strWalletFile.IsEmpty())
	{
		CString strOut;
		strOut = "请选择要插入的目标文件";
		MessageBox(strOut);
		return;
	}

	CString strPrivateKey;
	//获取控件的显示值
	CEdit *pPrivate = (CEdit *)GetDlgItem(IDC_TXT_TEST_PRIVATEKEY);
	pPrivate->GetWindowText(strPrivateKey);
	if (strPrivateKey.IsEmpty())
	{
		CString strOut;
		strOut = "请输入私钥";
		MessageBox(strOut);
		return;
	}
	if (strPrivateKey.GetLength() < 64)
	{
		CString strOut;
		strOut = "私钥长度不对";
		MessageBox(strOut);
		return;
	}

	//分配私钥空间
	unsigned char * pPrivateKey = (unsigned char *)malloc(33);
	memset(pPrivateKey, 0, 33);
	//unsigned char pPrivateKey[160] = { 0xfa,0x55,0x4d,0x43,0xee,0x35,0x97,0x81,0x24,0x04,0x95,0xe3,0x5e,0xe1,0x0e,0xf7,0x97,0x53,0xc4,0xe0,0xbe,0x64,0xdb,0x74,0x3b,0x49,0x51,0xff,0xdd,0x70,0x15,0xcb };
	//转化和获取私钥
	TransferPrivate2Hex(pPrivateKey, (char *)strPrivateKey.GetBuffer());

	//分配公钥空间
	unsigned char * pPublicKey = (unsigned char *)malloc(72);
	memset(pPublicKey, 0, 72);
	*pPublicKey = 0x04;
	unsigned char * pPublicTmp = pPublicKey + 1;

	//计算公钥
	secp256k1_ecmult_gen_context * pContext = (secp256k1_ecmult_gen_context*)malloc(sizeof(secp256k1_ecmult_gen_context));
	secp256k1_ecmult_gen_context_build(pContext);
	secp256k1_ecmult_gen(pContext, pPublicTmp, pPrivateKey);
	free(pContext);

	//将公钥转化为需要的顺序
	{
		//调整顺序
		unsigned char cTmp = 0;
		cTmp = pPublicTmp[0]; pPublicTmp[0] = pPublicTmp[31]; pPublicTmp[31] = cTmp;
		cTmp = pPublicTmp[1]; pPublicTmp[1] = pPublicTmp[30]; pPublicTmp[30] = cTmp;
		cTmp = pPublicTmp[2]; pPublicTmp[2] = pPublicTmp[29]; pPublicTmp[29] = cTmp;
		cTmp = pPublicTmp[3]; pPublicTmp[3] = pPublicTmp[28]; pPublicTmp[28] = cTmp;
		cTmp = pPublicTmp[4]; pPublicTmp[4] = pPublicTmp[27]; pPublicTmp[27] = cTmp;
		cTmp = pPublicTmp[5]; pPublicTmp[5] = pPublicTmp[26]; pPublicTmp[26] = cTmp;
		cTmp = pPublicTmp[6]; pPublicTmp[6] = pPublicTmp[25]; pPublicTmp[25] = cTmp;
		cTmp = pPublicTmp[7]; pPublicTmp[7] = pPublicTmp[24]; pPublicTmp[24] = cTmp;
		cTmp = pPublicTmp[8]; pPublicTmp[8] = pPublicTmp[23]; pPublicTmp[23] = cTmp;
		cTmp = pPublicTmp[9]; pPublicTmp[9] = pPublicTmp[22]; pPublicTmp[22] = cTmp;
		cTmp = pPublicTmp[10]; pPublicTmp[10] = pPublicTmp[21]; pPublicTmp[21] = cTmp;
		cTmp = pPublicTmp[11]; pPublicTmp[11] = pPublicTmp[20]; pPublicTmp[20] = cTmp;
		cTmp = pPublicTmp[12]; pPublicTmp[12] = pPublicTmp[19]; pPublicTmp[19] = cTmp;
		cTmp = pPublicTmp[13]; pPublicTmp[13] = pPublicTmp[18]; pPublicTmp[18] = cTmp;
		cTmp = pPublicTmp[14]; pPublicTmp[14] = pPublicTmp[17]; pPublicTmp[17] = cTmp;
		cTmp = pPublicTmp[15]; pPublicTmp[15] = pPublicTmp[16]; pPublicTmp[16] = cTmp;

		cTmp = pPublicTmp[32]; pPublicTmp[32] = pPublicTmp[63]; pPublicTmp[63] = cTmp;
		cTmp = pPublicTmp[33]; pPublicTmp[33] = pPublicTmp[62]; pPublicTmp[62] = cTmp;
		cTmp = pPublicTmp[34]; pPublicTmp[34] = pPublicTmp[61]; pPublicTmp[61] = cTmp;
		cTmp = pPublicTmp[35]; pPublicTmp[35] = pPublicTmp[60]; pPublicTmp[60] = cTmp;
		cTmp = pPublicTmp[36]; pPublicTmp[36] = pPublicTmp[59]; pPublicTmp[59] = cTmp;
		cTmp = pPublicTmp[37]; pPublicTmp[37] = pPublicTmp[58]; pPublicTmp[58] = cTmp;
		cTmp = pPublicTmp[38]; pPublicTmp[38] = pPublicTmp[57]; pPublicTmp[57] = cTmp;
		cTmp = pPublicTmp[39]; pPublicTmp[39] = pPublicTmp[56]; pPublicTmp[56] = cTmp;
		cTmp = pPublicTmp[40]; pPublicTmp[40] = pPublicTmp[55]; pPublicTmp[55] = cTmp;
		cTmp = pPublicTmp[41]; pPublicTmp[41] = pPublicTmp[54]; pPublicTmp[54] = cTmp;
		cTmp = pPublicTmp[42]; pPublicTmp[42] = pPublicTmp[53]; pPublicTmp[53] = cTmp;
		cTmp = pPublicTmp[43]; pPublicTmp[43] = pPublicTmp[52]; pPublicTmp[52] = cTmp;
		cTmp = pPublicTmp[44]; pPublicTmp[44] = pPublicTmp[51]; pPublicTmp[51] = cTmp;
		cTmp = pPublicTmp[45]; pPublicTmp[45] = pPublicTmp[50]; pPublicTmp[50] = cTmp;
		cTmp = pPublicTmp[46]; pPublicTmp[46] = pPublicTmp[49]; pPublicTmp[49] = cTmp;
		cTmp = pPublicTmp[47]; pPublicTmp[47] = pPublicTmp[48]; pPublicTmp[48] = cTmp;
	}

	//HASH值存放空间
	unsigned char * pHash = (unsigned char *)malloc(128);
	memset(pHash, 0, 128);
	//计算SHA256
	sha256(pPublicKey, 65, pHash);

	//RPMD160值存放空间
	unsigned char * pRMD160 = (unsigned char *)malloc(32);
	memset(pRMD160, 0, 32);
	unsigned char * pRPMDTmp = pRMD160 + 1;
	//计算RPMD160
	rmd160(pRPMDTmp, pHash);

	//将rmd160结果插入目标文件
	InsertIntoFile((char *)strWalletFile.GetBuffer(), pRPMDTmp);

	//计算两次SHA256
	unsigned char szTmp[36] = { 0 };
	sha256(pRMD160, 21, pHash);
	memcpy(szTmp, pHash, 32);
	sha256(szTmp, 32, pHash);

	char *pWalletAddress = (char *)malloc(72);
	size_t iLength = 72;
	//拼接产生钱包的串
	memcpy(pRMD160 + 21, pHash, 4);
	//产生钱包地址
	b58enc(pWalletAddress, &iLength, pRMD160, 25);

	//放入显示控件
	CEdit *pWalletAddr = (CEdit *)GetDlgItem(IDC_TXT_TEST_WALLETADDRESS);
	pWalletAddr->SetWindowText(pWalletAddress);

	//释放空间
	free(pPublicKey);
	free(pPrivateKey);
	free(pHash);
	free(pRMD160);
	free(pWalletAddress);
}
