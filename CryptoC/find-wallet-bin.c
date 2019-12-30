#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

//所有钱包地址
unsigned char *szWallet = NULL;
//数据实际存放量
unsigned int iSumBinCount = 0;

//初始化数组 By 排序后的文件
void wallet_bin_init(unsigned char * pWalletData, unsigned int iWalletCount)
{
	szWallet     = pWalletData;
	iSumBinCount = iWalletCount;
}

//查找钱包
int wallet_bin_find(unsigned char * pWallet, unsigned int iIndexStart, unsigned int iIndexEnd)
{
	//如果查找范围非常小，那么直接暴力处理
	if ((iIndexEnd - iIndexStart) < 4)
	{
		for (unsigned int i = iIndexStart; i <= iIndexEnd; i++)
			if (0 == memcmp(pWallet, szWallet + (i * 20), 20))
				return 0;
		return 1;
	}

	//中间位置
	unsigned int iMiddle = (iIndexStart + iIndexEnd) / 2;
	//根据情况递归处理
	int iResult = memcmp(pWallet, szWallet + (iMiddle * 20), 20);
	if (0 == iResult)
		return 0;
	else if (0 > iResult)
		return wallet_bin_find(pWallet, iIndexStart, iMiddle);
	else
		return wallet_bin_find(pWallet, iMiddle, iIndexEnd);
}
