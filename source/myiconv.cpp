//=======================================================================================
// �ַ�������ת��
//
// ��GNU��lib-iconvת�������⣬���Ծ�������Щ���룬����ժ���ģ����Ų���лл���ߡ�
//
// ԭ���ߣ�ccrun(����) info@ccrun.com
// ����ת�� C++Builder �о� - http://www.ccrun.com/article/go.asp?i=634&d=04g63p
//
// ����ԭ�������� C++ Builder �ģ���ֻ�ǰ����޸ĳ��� C ���롣
//=======================================================================================

#include <windows.h>
#include <tchar.h>

//=======================================================================================
// ������תGBK
//=======================================================================================

void BIG52GBK(char *szBuf)
{
	int nStrLen;
	BOOL bValue = FALSE;
	int nReturn;
	wchar_t *pws = NULL;

	if (!strcmp(szBuf, ""))
		return;
	nStrLen = strlen(szBuf);
	pws = (wchar_t *) malloc(sizeof(wchar_t) * (nStrLen + 1));	//new wchar_t[nStrLen + 1];
	nReturn = MultiByteToWideChar(950, 0, szBuf, nStrLen, pws, nStrLen + 1);
	nReturn = WideCharToMultiByte(936, 0, pws, nReturn, szBuf, nStrLen + 1, "?", &bValue);
	szBuf[nReturn] = 0;
	free(pws);
}


//=======================================================================================
// GBKת������
// ���A���񹲺͇� --> ���ؤH���@�M��
//=======================================================================================

void GBK2BIG5(char *szBuf)
{
	int nStrLen;
	wchar_t *pws;
	BOOL bValue = FALSE;

	if (!strcmp(szBuf, ""))
		return;
	nStrLen = strlen(szBuf);
	pws = (wchar_t *) malloc(sizeof(wchar_t) * (nStrLen + 1));	//new wchar_t[nStrLen + 1];
	MultiByteToWideChar(936, 0, szBuf, nStrLen, pws, nStrLen + 1);
	WideCharToMultiByte(950, 0, pws, nStrLen, szBuf, nStrLen + 1, "?", &bValue);
	szBuf[nStrLen] = 0;
	free(pws);
}


//=======================================================================================
// ��Ǹ�������ʾ�����ˣ�Ϊ�˷�ֹ�������ε�ת���ߣ�ֻ���ڴ���Щ��Ϣ��
// ���ߣ�ccrun(����) info@ccrun.com
// ����ת�� C++Builder �о� - http://www.ccrun.com/article/go.asp?i=634&d=04g63p
//---------------------------------------------------------------------------
// GB2312��תGBK��
// ����ת�� C++Builder�о� - http://www.ccrun.com/article.asp?i=634&d=04g63p
// �л����񹲺͹� --> ���A���񹲺͇�
//=======================================================================================

void GB2GBK(char *szBuf)
{
	DWORD wLCID;
	int nStrLen;
	char *pcBuf;
	int nReturn;

	if (!strcmp(szBuf, ""))
		return;
	nStrLen = strlen(szBuf);
	wLCID = MAKELCID(MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED), SORT_CHINESE_PRC);
	nReturn = LCMapString(wLCID, LCMAP_TRADITIONAL_CHINESE, szBuf, nStrLen, NULL, 0);
	if (!nReturn)
		return;
	pcBuf = (char *) malloc(nReturn + 1);	//new char[nReturn + 1];
	wLCID = MAKELCID(MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED), SORT_CHINESE_PRC);
	LCMapString(wLCID, LCMAP_TRADITIONAL_CHINESE, szBuf, nReturn, pcBuf, nReturn + 1);
	strncpy(szBuf, pcBuf, nReturn);

	free(pcBuf);
}


//=======================================================================================
// GBK��תGB2312��
// ���A���񹲺͇� --> �л����񹲺͹�
//=======================================================================================

void GBK2GB(char *szBuf)
{
	int nStrLen;
	int nReturn;
	char *pcBuf;
	DWORD wLCID;

	if (!strcmp(szBuf, ""))
		return;
	nStrLen = strlen(szBuf);
	wLCID = MAKELCID(MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED), SORT_CHINESE_BIG5);
	nReturn = LCMapString(wLCID, LCMAP_SIMPLIFIED_CHINESE, szBuf, nStrLen, NULL, 0);
	if (!nReturn)
		return;
	pcBuf = (char *) malloc(nReturn + 1);	// new char[nReturn + 1];
	wLCID = MAKELCID(MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED), SORT_CHINESE_BIG5);
	LCMapString(wLCID, LCMAP_SIMPLIFIED_CHINESE, szBuf, nReturn, pcBuf, nReturn + 1);
	strncpy(szBuf, pcBuf, nReturn);
	free(pcBuf);				//  delete []pcBuf;
}



void BIG52GB(char *s)
{
	BIG52GBK(s);
	GBK2GB(s);
}

void GB2BIG5(char *s)
{
	GB2GBK(s);
	GBK2BIG5(s);
}
