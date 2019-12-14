#pragma once


// WalletSettingDlg 对话框

class WalletSettingDlg : public CDialogEx
{
	DECLARE_DYNAMIC(WalletSettingDlg)

public:
	WalletSettingDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~WalletSettingDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_WALLETFIND_SETTING };
#endif

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedSettingAutogen();

	unsigned int GetAccount(char *pAccount);
	unsigned int GetCPUCount();
	unsigned int GetWalletAdd(char *pWallet);
	unsigned int GetReference(char *pWallet);

private:
	char szAccount[64] = { 0 };
	unsigned int iCPUCount = 4;
	char szWalletAddr[64] = { 0 };
	char szReference[64] = { 0 };
};
