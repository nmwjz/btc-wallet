#pragma once

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <list>
#include "CL/cl.h"

#include "device_info.h"

class wallet_find_opencl
{
public:
	wallet_find_opencl();
	~wallet_find_opencl();

	//调用顺序0
	//功能：设置钱包数据信息
	//参数；
	//	1、pWalletData钱包数据
	//	2、iWalletCount钱包数量
	//成功返回0，失败返回对应错误码
	unsigned int load_wallet(unsigned char *pWalletData, unsigned int iCount);

	//调用顺序0
	//功能：加载CL文件到内存
	//参数：
	//  1、pFileOpenCL12 OpenCL1.2的源文件
	//  2、pFileOpenCL20 OpenCL2.0的源文件
	//成功返回0，失败返回对应错误码
	unsigned int load_source(char *pFileOpenCL12, char *pFileOpenCL20);

	//调用顺序0
	//功能：获取设备ID，并且创建设备上下文
	//无参数；
	//成功返回0，失败返回对应错误码
	unsigned int find_platform();

	//调用顺序1，在方法load_wallet，load_source，find_platform之后
	//功能：分配应用执行的Buff，创建应用执行的参数
	//参数：
	//	无
	//成功返回0，失败返回对应错误码
	unsigned int create_buffer();

	//调用顺序2，在方法load_wallet，load_source，find_platform，create_buffer之后
	//功能：将Host数据加载到Kernel
	//	1、pPublicKey公钥数组，存放每一个开始的公钥
	//	2、pPrivateKey私钥数组，存放每一个开始的私钥
	//	3、iCalcCount每个线程的计算量
	//成功返回0，失败返回对应错误码
	unsigned int load_buffer(unsigned char * pPublicKey, unsigned char * pPrivateKey, unsigned int iCalcCount);

	//调用顺序3，在方法load_wallet，load_source，find_platform，create_buffer，load_buffer之后
	//功能：计算目标加密数据
	//参数：
	//	1、pOutput输出数据
	//	2、iOutputCount输出数据的数量
	//成功返回0，失败返回对应错误码
	unsigned int crypt(unsigned char * pOutput, unsigned int &iOutputCount);

	//调用顺序2，在方法find_platform之后即可
	//返回Work_Item的数量，即并发线程的数量
	unsigned int get_thread();

	//调用顺序4，在方法create_buffer，crypt之后
	//功能：清除战场，清除缓存
	void clean_buff();

	//调用顺序5，在方法create_buffer，crypt，clean_buff之后
	//功能：清除战场，清除所有OpenCL对象
	void clean_context();

private:
	cl_uint iPlatformCount           = 0;       //平台数量
	cl_platform_id *pPlatformID      = NULL;    //平台ID列表
	std::list<device_info *> lstDeviceInfo;     //所有设备信息列表

	unsigned int iWalletCount        = 0;       //存放钱包的数量
	unsigned char *pAllWalletContext = NULL;    //存放所有的钱包信息
	char *pSourceOpenCL12            = NULL;    //存放OPenCL1.2脚本文件的串
	char *pSourceOpenCL20            = NULL;    //存放OPenCL2.0脚本文件的串

	//secp256k1输入输出缓冲
	cl_mem buffer_public_key;      //公钥
	cl_mem buffer_private_key;     //私钥
	cl_mem buffer_calc_count;      //每个线程的计算量
	cl_mem buffer_wallet_data;     //所有钱包的数据Buff
	cl_mem buffer_wallet_count;    //所有钱包的数量
	cl_mem buffer_output;          //输出结果数组
	cl_mem buffer_output_count;    //输出结果数量

private:
	void setJobID(unsigned int iID)
	{
		iJobID = iID;
	}
	void setTaskID(unsigned int iID)
	{
		iTaskID = iID;
	}
	void setPrivateCount(unsigned int iCount)
	{
		iPrivateCount = iCount;
	}
	unsigned int getJobID()
	{
		return iJobID;
	}
	unsigned int getTaskID()
	{
		return iTaskID;
	}
	unsigned int getPrivateCount()
	{
		return iPrivateCount;
	}
	bool isFinish()
	{
		return true;
	}

	//功能：读取OpenCL文件
	//参数：
	//  1、fp被fopen打开的文件句柄
	//  2、pSourceContext存放文件的目标Buff
	//  3、iLen文件长度或Buff长度
	//成功返回0，失败返回对应错误码
	unsigned int readFile(FILE *fp, unsigned char * pSourceContext, unsigned int iLen);

	//功能：获取平台下所有设备，并且在每个设备ID的基础上创建上下文
	//参数：
	//  1、iPlatformID平台ID
	//成功返回0，失败返回对应错误码
	unsigned int find_device(cl_platform_id iPlatformID);

	//功能：分配应用执行的Buff，创建应用执行的参数
	//参数：
	//  1、oContext设备的上下文
	//  2、oKernel设备上创建的Kernel
	//成功返回0，失败返回对应错误码
	unsigned int create_buffer_inner(cl_context oContext, cl_kernel oKernel);

	//功能：将Host数据加载到Queue
	//	1、oQueue命令队列
	//	2、pPublicKey公钥数组，存放每一个开始的公钥
	//	3、pPrivateKey私钥数组，存放每一个开始的私钥
	//  4、iCalcCount每个线程的计算量
	//成功返回0，失败返回对应错误码
	unsigned int load_buffer_inner(cl_command_queue oQueue, unsigned char * pPublicKey, unsigned char * pPrivateKey, unsigned int iCalcCount);

	//功能：计算目标加密数据
	//参数：
	//  1、oCommandQueue命令队列
	//  2、oKernel内核
	//	3、pOutput输出数据
	//	4、iOutputCount输出数据的数量
	//成功返回0，失败返回对应错误码
	unsigned int crypt_inner(cl_command_queue oCommandQueue, cl_kernel oKernel, unsigned char * pOutput, unsigned int &iOutputCount);
private:
	unsigned int iJobID = 0;
	unsigned int iTaskID = 0;
	unsigned int iPrivateCount = 0;
};
