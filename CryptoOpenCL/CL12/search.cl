

int MEMCMP2(unsigned char * pWalletKey, unsigned char * pWallet, unsigned int iLength )
{
	for( unsigned int i=0; i<iLength; i++ )
	{
		if( *pWalletKey > *pWallet )
			return 1;
		else if( *pWalletKey < *pWallet )
			return -1;
		else
		{
			pWalletKey ++;
			pWallet ++;
			continue;
		}
	}

	return 0;
}

//查找钱包
int wallet_bin_find(unsigned char * pWallet, unsigned char * pWalletAll, unsigned int iCount)
{
	unsigned int iLow  = 0;             //数组最小索引值
	unsigned int iHigh = iCount - 1;    //数组最大索引值

	unsigned int iMiddle = 0;
	while( iLow<=iHigh )
	{
		//printf("-->%d--%d--\r\n" , iLow , iHigh);
		iMiddle = ( iLow + iHigh ) / 2;
		if( 0 == MEMCMP2(pWallet, pWalletAll+20*iMiddle, 20) )
			return 0;
		else if( 0 < MEMCMP2(pWallet, pWalletAll+20*iMiddle, 20) )
			iLow = iMiddle + 1;
		else if( 0 > MEMCMP2(pWallet, pWalletAll+20*iMiddle, 20) )
			iHigh = iMiddle - 1;
	}

	return -1; //iLow>iHigh的情况，这种情况下key的值大于arr中最大的元素值或者key的值小于arr中最小的元素值
}

