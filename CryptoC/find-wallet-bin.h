#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

	//初始化数组 By 已排序的文件
	void wallet_bin_init(unsigned char * pWalletData, unsigned int iWalletCount);

	//查找目标数据
	int wallet_bin_find(unsigned char * pWallet, unsigned int iIndexStart, unsigned int iIndexEnd);

#ifdef __cplusplus
}
#endif