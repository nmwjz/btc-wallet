#pragma once

class WalletResultItem
{
public:
	WalletResultItem()
	{
	};
	~WalletResultItem()
	{
	};

	unsigned char * getPrivateKey()
	{
		return szPrivateKey;
	}
	unsigned char * getPublicKey()
	{
		return szPublicKey;
	}
	unsigned int getPrivateOffset()
	{
		return iPrivateOffset;
	}
	unsigned int getJobID()
	{
		return iJobID;
	}
	unsigned int getTaskID()
	{
		return iTaskID;
	}

	void setPrivateKey(unsigned char *pKey)
	{
		memcpy(szPrivateKey, pKey, 32);
	}
	void setPublicKey(unsigned char *pKey)
	{
		memcpy(szPrivateKey, pKey, 64);
	}
	void setPrivateOffset(unsigned int iOffset)
	{
		iPrivateOffset = iOffset;
	}
	void setJobID(unsigned int iID)
	{
		iJobID = iID;
	}
	void setTaskID(unsigned int iID)
	{
		iTaskID = iID;
	}

	void setState(unsigned int iState)
	{
		this->iState = iState;
	};
	unsigned int getState()
	{
		return iState;
	};
private:
	unsigned char szPrivateKey[32] = { 0 };
	unsigned char szPublicKey[64]  = { 0 };
	unsigned int iPrivateOffset = 0;
	unsigned int iJobID = 0;
	unsigned int iTaskID = 0;

	unsigned int iState = 0;    //处理状态：0，未处理；1，已处理
};
