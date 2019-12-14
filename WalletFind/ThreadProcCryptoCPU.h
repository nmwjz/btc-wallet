#pragma once

#include <thread>
#include <list>
#include <mutex>

#include "WalletResultItem.h"
#include "WalletResult.h"
#include "WalletTask.h"

#include "../CryptoC/find-wallet-bin.h"
#include "../CryptoC/ecmult_gen_impl.h"
#include "../CryptoC/rmd160.h"
#include "../CryptoC/sha256.h"

//业务处理子线程
//用于计算和验证一组私钥
class ThreadProcCryptoCPU
{
public:
	ThreadProcCryptoCPU()
	{
		//初始化椭圆曲线
		pContext = (secp256k1_ecmult_gen_context*)malloc(sizeof(secp256k1_ecmult_gen_context));
		secp256k1_ecmult_gen_context_build(pContext);

		//返回结果对象
		pWalletResult = new WalletResult();
		//计算任务对象
		pWalletTask = new WalletTask;
	};
	~ThreadProcCryptoCPU()
	{
		free(pContext);

		//释放结果对象
		delete pWalletResult;
		//释放任务对象
		delete pWalletTask;
	};

	//启动线程
	void start()
	{
		//设置开始标志
		iFlagExit = 1;


		//启动处理线程
		oMainWorkThd = std::thread(&ThreadProcCryptoCPU::run, this);
		oMainWorkThd.detach();
	};
	//停止线程
	void stop()
	{
		//设置退出标志
		iFlagExit = 0;
	};

	//设置钱包数据
	//参数：
	//  1、pWalletData钱包数据；
	//  2、iWalletCount钱包数量
	void setWalletData(unsigned char *pWalletData, unsigned int iWalletCount)
	{
		this->pWalletData  = pWalletData;
		this->iWalletCount = iWalletCount;

		//设置钱包数据
		wallet_bin_init(pWalletData, iWalletCount);
	};
	//添加业务线程的运行Data，添加任务
	//参数：
	//  1、JobID；
	//  2、任务ID；
	//  3、开始私钥；
	//  4、每线程的计算量；
	void addTaskData(unsigned int iJobID, unsigned int iTaskID, unsigned char *szPrivateKey, unsigned int iCalcCount)
	{
		WalletTaskItem * pTaskItem = new WalletTaskItem();
		pTaskItem->setJobID(iJobID);
		pTaskItem->setTaskID(iTaskID);
		pTaskItem->setPrivateStart(szPrivateKey);
		pTaskItem->setPrivateCount(iCalcCount);

		pWalletTask->pushItem(pTaskItem);
	};

	//判断线程是否在运行
	bool isRun()
	{
		return (1 == iFlagExit);
	};

	//获取结果列表
	WalletResult * getResult()
	{
		return pWalletResult;
	};
	//获取任务
	WalletTask * getTask()
	{
		return pWalletTask;
	};

	//返回线程的Handle
	std::thread::native_handle_type getHandle()
	{
		return oMainWorkThd.native_handle();
	};

private:
	//产生开头的公钥
	unsigned int genPublicKeyStart(unsigned char *pPrivateKey, unsigned char *pPublicKey, unsigned int iThreadCount, unsigned int iCalcCount, unsigned char *pPrivateStart)
	{
		//使用私钥，计算公钥为每个GPU线程计算公钥
		for (unsigned int i = 0; i < iThreadCount; i++)
		{
			//目标私钥
			ProcPrivateAddInt(pPrivateKey, pPrivateStart, iCalcCount * i);

			//目标私钥对应的公钥
			secp256k1_ecmult_gen(pContext, pPublicKey, pPrivateKey);

			//目标私钥和公钥的存储位置
			pPrivateKey = pPrivateKey + 32;
			pPublicKey  = pPublicKey  + 66;
		}
		return 0;
	};
	//产生其他的公钥
	unsigned int genPublicKeyOther(unsigned char *pPrivateKey, unsigned char *pPublicKey, unsigned int iThreadCount, unsigned int iCalcCount, unsigned char * pOutput, unsigned int &iResultCount, unsigned int iJob, unsigned int iTask, unsigned int *iCurrent)
	{
		unsigned char * szPublicKey  = pPublicKey;
		//===================================//
		uint32_t iPublic0 = ((szPublicKey[31] << 24) & 0xFF000000) | ((szPublicKey[30] << 16) & 0x00FF0000) | ((szPublicKey[29] << 8) & 0x0000FF00) | (szPublicKey[28] & 0xFF);
		uint32_t iPublic1 = ((szPublicKey[27] << 24) & 0xFF000000) | ((szPublicKey[26] << 16) & 0x00FF0000) | ((szPublicKey[25] << 8) & 0x0000FF00) | (szPublicKey[24] & 0xFF);
		uint32_t iPublic2 = ((szPublicKey[23] << 24) & 0xFF000000) | ((szPublicKey[22] << 16) & 0x00FF0000) | ((szPublicKey[21] << 8) & 0x0000FF00) | (szPublicKey[20] & 0xFF);
		uint32_t iPublic3 = ((szPublicKey[19] << 24) & 0xFF000000) | ((szPublicKey[18] << 16) & 0x00FF0000) | ((szPublicKey[17] << 8) & 0x0000FF00) | (szPublicKey[16] & 0xFF);
		uint32_t iPublic4 = ((szPublicKey[15] << 24) & 0xFF000000) | ((szPublicKey[14] << 16) & 0x00FF0000) | ((szPublicKey[13] << 8) & 0x0000FF00) | (szPublicKey[12] & 0xFF);
		uint32_t iPublic5 = ((szPublicKey[11] << 24) & 0xFF000000) | ((szPublicKey[10] << 16) & 0x00FF0000) | ((szPublicKey[ 9] << 8) & 0x0000FF00) | (szPublicKey[ 8] & 0xFF);
		uint32_t iPublic6 = ((szPublicKey[ 7] << 24) & 0xFF000000) | ((szPublicKey[ 6] << 16) & 0x00FF0000) | ((szPublicKey[ 5] << 8) & 0x0000FF00) | (szPublicKey[ 4] & 0xFF);
		uint32_t iPublic7 = ((szPublicKey[ 3] << 24) & 0xFF000000) | ((szPublicKey[ 2] << 16) & 0x00FF0000) | ((szPublicKey[ 1] << 8) & 0x0000FF00) | (szPublicKey[ 0] & 0xFF);
		uint32_t iPublic8 = ((szPublicKey[63] << 24) & 0xFF000000) | ((szPublicKey[62] << 16) & 0x00FF0000) | ((szPublicKey[61] << 8) & 0x0000FF00) | (szPublicKey[60] & 0xFF);
		uint32_t iPublic9 = ((szPublicKey[59] << 24) & 0xFF000000) | ((szPublicKey[58] << 16) & 0x00FF0000) | ((szPublicKey[57] << 8) & 0x0000FF00) | (szPublicKey[56] & 0xFF);
		uint32_t iPublica = ((szPublicKey[55] << 24) & 0xFF000000) | ((szPublicKey[54] << 16) & 0x00FF0000) | ((szPublicKey[53] << 8) & 0x0000FF00) | (szPublicKey[52] & 0xFF);
		uint32_t iPublicb = ((szPublicKey[51] << 24) & 0xFF000000) | ((szPublicKey[50] << 16) & 0x00FF0000) | ((szPublicKey[49] << 8) & 0x0000FF00) | (szPublicKey[48] & 0xFF);
		uint32_t iPublicc = ((szPublicKey[47] << 24) & 0xFF000000) | ((szPublicKey[46] << 16) & 0x00FF0000) | ((szPublicKey[45] << 8) & 0x0000FF00) | (szPublicKey[44] & 0xFF);
		uint32_t iPublicd = ((szPublicKey[43] << 24) & 0xFF000000) | ((szPublicKey[42] << 16) & 0x00FF0000) | ((szPublicKey[41] << 8) & 0x0000FF00) | (szPublicKey[40] & 0xFF);
		uint32_t iPublice = ((szPublicKey[39] << 24) & 0xFF000000) | ((szPublicKey[38] << 16) & 0x00FF0000) | ((szPublicKey[37] << 8) & 0x0000FF00) | (szPublicKey[36] & 0xFF);
		uint32_t iPublicf = ((szPublicKey[35] << 24) & 0xFF000000) | ((szPublicKey[34] << 16) & 0x00FF0000) | ((szPublicKey[33] << 8) & 0x0000FF00) | (szPublicKey[32] & 0xFF);
		//公钥
		secp256k1_gej oPublicKey;
		//获取第一个公钥
		secp256k1_ge secp256k1_Public = SECP256K1_GE_CONST(iPublic0, iPublic1, iPublic2, iPublic3, iPublic4, iPublic5, iPublic6, iPublic7, iPublic8, iPublic9, iPublica, iPublicb, iPublicc, iPublicd, iPublice, iPublicf);
		secp256k1_gej_set_ge(&oPublicKey, &secp256k1_Public);

		//用于SHA256加密的公钥字符串
		unsigned char szPublicForSha256[66] = { 0 };

		//sha256的输出
		unsigned char szSHA256[32] = { 0 };
		//RMD160的输出
		unsigned char szRMD160[20] = { 0 };

		unsigned int *i = iCurrent;
		//循环获取每一个公钥
		for (; (*i) < iCalcCount; (*i)++)
		{
			if (!iFlagExit)
				break;

			//将目标公钥转化为字符串
			secp256k1_ge ge;
			secp256k1_ge_set_gej(&ge, &oPublicKey);
			//公钥字符串
			//secp256k1_ge_to_storage((secp256k1_ge_storage *)(szPublicForSha256 + 1), &ge);
			secp256k1_ge_to_buff( szPublicForSha256 + 1, &ge);
			//开头固定为04，标识非压缩形式
			szPublicForSha256[0] = 0x04;

			memset(szSHA256, 0, 32);
			//计算公钥的SHA265
			sha256(szPublicForSha256, 65, szSHA256);

			//计算RMD160
			rmd160(szRMD160, szSHA256);

			//查找计算结果是否OK
			int bResult = wallet_bin_find(szRMD160, 0, iWalletCount);
			if (0 == bResult)
			//if( ( 0 == ( (*i) % 0x100 ) ) && (iResultCount < 10 ) )//测试用
			{
				//找到填写的位置
				unsigned char szDstPrivate[32] = { 0 };
				//在原有私钥的基础上，添加偏移值，然后写入目标地址
				ProcPrivateAddInt(szDstPrivate, pPrivateKey, *i);

				memcpy(pOutput, szDstPrivate, 32);
				pOutput = pOutput + 32;

				iResultCount++;
			}

			//使用当前公钥，计算下一个公钥
			secp256k1_gej_add_ge(&oPublicKey, &oPublicKey, &secp256k1_ge_const_g);
		}

		return 0;
	}
	//计算私钥 和 32位整数相加
	int ProcPrivateAddInt(unsigned char *pPrivateDest, unsigned char * szPrivateKey, unsigned int iNum)
	{
		unsigned char iFirst = iNum % 0x100; iNum = iNum / 0x100;
		unsigned char iSecond = iNum % 0x100; iNum = iNum / 0x100;
		unsigned char iThird = iNum % 0x100; iNum = iNum / 0x100;
		unsigned char iFourth = iNum % 0x100; iNum = iNum / 0x100;

		//先把全部数据放到目标空间
		memcpy(pPrivateDest, szPrivateKey, 32);

		//前28位原样不动
		int iTmp = szPrivateKey[31] + iFirst; pPrivateDest[31] = szPrivateKey[31] + iFirst;
		if (iTmp >= 0x100)
		{
			iTmp = szPrivateKey[30] + iSecond + 1; pPrivateDest[30] = szPrivateKey[30] + iSecond + 1;
		}
		else
		{
			iTmp = szPrivateKey[30] + iSecond; pPrivateDest[30] = szPrivateKey[30] + iSecond;
		}

		if (iTmp >= 0x100)
		{
			iTmp = szPrivateKey[29] + iThird + 1; pPrivateDest[29] = szPrivateKey[29] + iThird + 1;
		}
		else
		{
			iTmp = szPrivateKey[29] + iThird; pPrivateDest[29] = szPrivateKey[29] + iThird;
		}

		if (iTmp >= 0x100)
		{
			iTmp = szPrivateKey[28] + iFourth + 1; pPrivateDest[28] = szPrivateKey[28] + iFourth + 1;
		}
		else
		{
			iTmp = szPrivateKey[28] + iFourth; pPrivateDest[28] = szPrivateKey[28] + iFourth;
		}

		if (iTmp >= 0x100)
		{
			iTmp = szPrivateKey[27] + 1; szPrivateKey[27] = szPrivateKey[27] + 1;
		}

		if (iTmp >= 0x100)
		{
			iTmp = szPrivateKey[26] + 1; szPrivateKey[26] = szPrivateKey[26] + 1;
		}
		return iTmp;
	};

private:
	//释放每一个已经设为删除的任务
	static void ReleaseFinish(WalletTask * pWalletTask)
	{
		//任务对象
		WalletTaskItem * pTaskItem = NULL;
		//获取第一个Item
		pTaskItem = pWalletTask->getItemBegin();

		//循环释放每一个已经设为删除的任务
		while (true)
		{
			//如果找不到，就跳出去
			if (NULL == pTaskItem)
			{
				break;
			}

			//如果已经被设为删除状态，就删除掉
			if (3 == pTaskItem->getState())
			{
				//从任务清单删除
				pWalletTask->popItem(pTaskItem);
				//释放空间
				delete pTaskItem;
				pTaskItem = NULL;
			}

			//获取下一个任务
			pTaskItem = pWalletTask->getItemNext();
		}
	}
	//线程主函数
	static void run(ThreadProcCryptoCPU * pThread)
	{
		if (NULL == pThread)
			return;

		//任务对象
		WalletTask * pWalletTask = pThread->pWalletTask;
		WalletTaskItem * pTaskItem = NULL;

		while (pThread->iFlagExit)
		{
			//查找和释放已经删除的任务
			ReleaseFinish(pWalletTask);

			//寻找未处理的任务Item
			pTaskItem = pWalletTask->getItemPending();
			//如果找不到，就等一下再找
			if (!pTaskItem)
			{
				Sleep(100);
				continue;
			}

			//设置为正在处理状态
			pTaskItem->setState(1);

			//将任务交给空闲任务处理器
			pThread->proc_task(pTaskItem);

			//设置为完成状态
			pTaskItem->setState(2);
		}
		//退出状态
		pThread->iFlagExit = 0;
	};
	void proc_task(WalletTaskItem * pTaskItem)
	{
		//当前处理位置
		unsigned int * iCurrent = pTaskItem->getCurrent();

		//开始私钥
		unsigned char szPrivateStart[33] = { 0 };
		memcpy(szPrivateStart, pTaskItem->getPrivateStart(), 32);
		//JobID
		unsigned int iJobID = pTaskItem->getJobID();
		//TaskID
		unsigned int iTaskID = pTaskItem->getTaskID();
		//私钥数量
		unsigned int iPrivateCount = pTaskItem->getPrivateCount();

		//处理每个阶段返回的结果
		unsigned int iResult = 0;

		//这里的线程数量
		unsigned int iThreadCount = 1;
		//计算私钥和公钥的空间 //CPU线程，内部并行只有1个
		unsigned char * pPrivateKey = (unsigned char *)malloc(32);
		unsigned char * pPublicKey  = (unsigned char *)malloc(66);

		//返回结果以及返回结果数量，每个线程最大128个返回
		unsigned int iResultCount = 0;
		unsigned char * pOutput = (unsigned char *)malloc(32 * iThreadCount * 128);

		//计算开始公钥数组
		iResult = genPublicKeyStart(pPrivateKey, pPublicKey, iThreadCount, iPrivateCount, szPrivateStart);

		//结果数量为0
		iResultCount = 0;

		//调用GPU处理剩余的计算任务
		iResult = genPublicKeyOther(pPrivateKey, pPublicKey, iThreadCount, iPrivateCount, pOutput, iResultCount, iJobID, iTaskID, iCurrent);

		//读取每个返回结果
		for (unsigned int i = 0; i < iResultCount; i++)
		{
			WalletResultItem *pItem = new WalletResultItem();
			pItem->setPrivateKey(pOutput + i * 32);
			pItem->setJobID(iJobID);
			pItem->setTaskID(iTaskID);
			pWalletResult->pushItem(pItem);
		}

		//释放空间
		free(pPrivateKey);
		free(pPublicKey);
		free(pOutput);
	};

private:
	//线程运行和退出标志: 0,退出；1,运行
	unsigned int iFlagExit   = 0;

	//主线程句柄
	std::thread oMainWorkThd;
	//计算结果
	WalletResult * pWalletResult = NULL;
	//计算任务
	WalletTask * pWalletTask = NULL;

	//钱包文件内容
	unsigned char *pWalletData = NULL;
	//钱包数据中钱包的数量
	unsigned int iWalletCount  = 0;

	//椭圆曲线计算对象
	secp256k1_ecmult_gen_context * pContext = NULL;
};
