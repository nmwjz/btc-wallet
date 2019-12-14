// WalletSettingDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "WalletFind.h"
#include "WalletSettingDlg.h"
#include "afxdialogex.h"


// WalletSettingDlg 对话框

IMPLEMENT_DYNAMIC(WalletSettingDlg, CDialogEx)

WalletSettingDlg::WalletSettingDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_WALLETFIND_SETTING, pParent)
{
	//获取当前路径，用于读取配置文件
	char szPathFile[512] = { 0 };
	GetCurrentDirectory(256, szPathFile);
	char * pPathFile = strcat(szPathFile, "\\all-wallet.ini");

	GetPrivateProfileString(_T("BaseInfo"), _T("Account"), "", szAccount, 64, szPathFile); //读取账号
	iCPUCount = GetPrivateProfileInt(_T("BaseInfo"), _T("CPUCount"), 4, szPathFile); //读取可以使用的CPU数量
	GetPrivateProfileString(_T("BaseInfo"), _T("WalletAddr"), "", szWalletAddr, 64, szPathFile); //读取钱包地址
	GetPrivateProfileString(_T("BaseInfo"), _T("Reference"), "", szReference, 64, szPathFile); //读取推荐人账号
}

WalletSettingDlg::~WalletSettingDlg()
{
}

void WalletSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BOOL WalletSettingDlg::OnInitDialog()
{
	char szTmpCPU[16] = { 0 };
	sprintf(szTmpCPU, "%d", iCPUCount);

	//设置账号
	((CEdit *)GetDlgItem(IDC_SETTING_ACCOUNT))->SetWindowTextA(szAccount);

	//设置比特钱包地址
	((CEdit *)GetDlgItem(IDC_SETTING_WALLETADDR))->SetWindowTextA(szWalletAddr);

	//设置CPU数量
	((CEdit *)GetDlgItem(IDC_SETTING_CPUCOUNT))->SetWindowTextA(szTmpCPU);

	//设置推荐人账号
	((CEdit *)GetDlgItem(IDC_SETTING_REFERENCE))->SetWindowTextA(szReference);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

BEGIN_MESSAGE_MAP(WalletSettingDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &WalletSettingDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_SETTING_AUTOGEN, &WalletSettingDlg::OnBnClickedSettingAutogen)
END_MESSAGE_MAP()


// WalletSettingDlg 消息处理程序


//设置完成，存储配置信息
void WalletSettingDlg::OnBnClickedOk()
{
	//获取当前路径，用于读取配置文件
	char szPathFile[512] = { 0 };
	GetCurrentDirectory(256, szPathFile);
	char * pPathFile = strcat(szPathFile, "\\all-wallet.ini");

	char szTmp[128] = { 0 };

	//获取比特钱包地址
	((CEdit *)GetDlgItem(IDC_SETTING_ACCOUNT))->GetWindowTextA(szTmp, 128);
	//将比特钱包地址写入文件
	WritePrivateProfileString(_T("BaseInfo"), _T("Account"), szTmp, szPathFile);

	//获取比特钱包地址
	((CEdit *)GetDlgItem(IDC_SETTING_WALLETADDR))->GetWindowTextA(szTmp,128);
	//将比特钱包地址写入文件
	WritePrivateProfileString(_T("BaseInfo"), _T("WalletAddr"), szTmp, szPathFile);

	//获取推荐人账号
	((CEdit *)GetDlgItem(IDC_SETTING_REFERENCE))->GetWindowTextA(szTmp, 128);
	//将推荐人账号写入文件
	WritePrivateProfileString(_T("BaseInfo"), _T("Reference"), szTmp, szPathFile);

	//获取CPU数量
	((CEdit *)GetDlgItem(IDC_SETTING_CPUCOUNT))->GetWindowTextA(szTmp, 128);
	//将CPU数量写入文件
	WritePrivateProfileString(_T("BaseInfo"), _T("CPUCount"), szTmp, szPathFile);

	//读取最新配置信息
	iCPUCount = GetPrivateProfileInt(_T("BaseInfo"), _T("CPUCount"), 4, szPathFile); //读取可以使用的CPU数量
	GetPrivateProfileString(_T("BaseInfo"), _T("WalletAddr"), "", szWalletAddr, 128, szPathFile); //读取钱包地址
	GetPrivateProfileString(_T("BaseInfo"), _T("Reference"), "", szReference, 128, szPathFile); //读取推荐人账号

	CDialog::OnOK();
}
//自动生成钱包地址
void WalletSettingDlg::OnBnClickedSettingAutogen()
{
	// TODO: 在此添加控件通知处理程序代码
}

//读取账号
unsigned int WalletSettingDlg::GetAccount(char *pAccount)
{
	strcpy(pAccount, szAccount);
	return 0;
}
//读取可以使用的CPU数量
unsigned int WalletSettingDlg::GetCPUCount()
{
	return iCPUCount;
}
//读取钱包地址
unsigned int WalletSettingDlg::GetWalletAdd(char *pWallet)
{
	strcpy(pWallet, szWalletAddr);
	return 0;
}
//读取推荐人账号
unsigned int WalletSettingDlg::GetReference(char *pWallet)
{
	strcpy(pWallet, szReference);
	return 0;
}
