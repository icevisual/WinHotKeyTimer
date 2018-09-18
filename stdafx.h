// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <strsafe.h>

#include "windows.h"
#include "time.h"
#include "math.h"
#include "FileConfig.h"
#include <iostream>  
using namespace std;
// TODO:  �ڴ˴����ó�����Ҫ������ͷ�ļ�

INT GetMilliSecondOfDay();

VOID GetHMS(TCHAR * output);

VOID LocalRegisterHotKey(_In_opt_ HWND hWnd, _In_ int id, _In_ UINT fsModifiers, _In_ UINT vk);

VOID TrimNewLine(CHAR * string, INT length);

// ��ģʽƥ�䣬ƥ��  dd:dd:dd,ddd ��ʽ
BOOLEAN SimpleRegexMatch(CHAR * target, INT offset, INT length, CHAR * regex, INT rLength, INT * out);

// �� dd:dd:dd,ddd ת���� ����
INT ConvertHMSF2Int(CHAR * target, INT offset);

// ��ȡ ��ζ ID ����1 ~ +00 ��,�ַ������� #!#
INT GetSmellID(CHAR * taregt, INT length);

// ��ȡ srt �ļ��� ʱ��Σ����룩
BOOLEAN GetSrtTimePeriod(CHAR * string, INT length, INT  * StartEnd);

// ������תΪ HMSF ��ʽ��ʱ���ַ���
VOID MilliSecond2HMSF(INT ms, TCHAR * msString);

DWORD char2Wchar(char * sBuf, int sBufSize, WCHAR * output);

VOID ShowLastErrorMsg(LPTSTR lpszFunction);

VOID InitScentrealmFunctions(HINSTANCE g_ScentRealm_DLL, struct ScentrealmRuntime * runtime);

INT RandomInt(INT min, INT max);
BOOL DoubleEqual(DOUBLE a, DOUBLE b);
// ����
HBITMAP ScreenCapture(LPCSTR filename, WORD BitCount, LPRECT lpRect);
// ģ��������룬����λ�ֱ� ���� �Ƿ� Shift��Ctrl��Win��Alt

VOID SimulateKeyArrayInput(WORD  Keys[], CHAR Count);

WORD ConvertChar2KeyWordAndSimulate(string str);

INT IndexOf(CHAR *data, INT length, CHAR chr);


int DOMatch(string img_name, string templ_name);


typedef int(*pScentrealmInit)();
typedef int(*pScentrealmPlaySmell)(int smell, int duration);
typedef int(*pScentrealmStopPlay)();
typedef int(*pScentrealmGetConnectStatus)();
typedef int(*pScentrealmClose)();

struct ScentrealmRuntime
{
	unsigned char DllState;  // is dll imported & function initialized
	unsigned long LastErrorCode;   // dll error code
	unsigned char ConnectState;    // connect status ,0 device connected��1 device unconnected��2 controller unconnected

	bool isReconnecting;

	int(*pInit)();
	int(*pPlaySmell)(int smell, int duration);
	int(*pStopPlay)();
	int(*pGetConnectStatus)();
	int(*pClose)();
};

#define CS_UNKNOWN 99
#define CS_DEV_CONNECTED 0
#define CS_DEV_UNCONNECTED 1
#define CS_CTR_UNCONNECTED 2
#define DS_UNLOAD 0XA0
#define DS_LOADED 0XFF
#define DS_LOADOV 0X01


