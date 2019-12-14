#pragma once

#include "stdafx.h"
#include "WalletFind.h"

#define JSON_INVALID 0
#define JSON_INT     1
#define JSON_STRING  2
#define JSON_OBJ     3  //暂不支持
#define JSON_ARRAY   4  //暂不支持

class JsonObj
{
public:
	JsonObj(char * pObjName, int iObjData)
	{
		iType = JSON_INT;

		pName = (char *)malloc(strlen(pObjName));
		strcpy(pName, pObjName);

		iData = iObjData;
	}
	JsonObj(char * pObjName, char * pObjData)
	{
		iType = JSON_STRING;

		pName = (char *)malloc(strlen(pObjName));
		strcpy(pName, pObjName);

		pData = (char *)malloc(strlen(pObjData));
		strcpy(pData, pObjData);
	}

	//从字符串分析JSON对象
	static JsonObj * ParseFromString(char *pSrc)
	{
		//去掉前后的空格、Tab、{、}、回车、换行
		pSrc = strtrim(pSrc);

		while (true)
		{
			//查找冒号
			char *pColon = strstr(pSrc, ":");
			//查找逗号
			char *pComma = strstr(pSrc, ",");
		}
	}

	~JsonObj()
	{
		free(pName);

		if (pData)
			free(pData);
	}
private:
	static char * strtrim(char *s)
	{
		char *p = s;
		char *q = s;
		while ((*p == ' ') || (*p == '\t') || (*p == '\r') || (*p == '\n') ) ++p;
		while (*q++ = *p++);
		q -= 2;
		while ((*q == ' ') || (*q == '\t') || (*q == '\r') || (*q == '\n') ) --q;
		*(q + 1) = '\0';
		return s;
	}

private:
	int iType    = JSON_INVALID;
	char * pName = NULL;
	int iData    = 0;
	char * pData = NULL;
	std::list<void *> lstData;

	void * pObjData;  //暂不支持
	void * pArrayData;  //暂不支持
};

