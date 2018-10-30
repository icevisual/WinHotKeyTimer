// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <strsafe.h>
#include "MD5.h"
#include "windows.h"
#include "time.h"
#include "math.h"
#include "FileConfig.h"
#include <iostream>  
#include<vector>
using namespace std;
// TODO:  在此处引用程序需要的其他头文件

using std::string;
using std::vector;

INT GetMilliSecondOfDay();

VOID GetHMS(TCHAR * output);

VOID LocalRegisterHotKey(_In_opt_ HWND hWnd, _In_ int id, _In_ UINT fsModifiers, _In_ UINT vk);

VOID TrimNewLine(CHAR * string, INT length);

// 简单模式匹配，匹配  dd:dd:dd,ddd 形式
BOOLEAN SimpleRegexMatch(CHAR * target, INT offset, INT length, CHAR * regex, INT rLength, INT * out);

// 将 dd:dd:dd,ddd 转换成 毫秒
INT ConvertHMSF2Int(CHAR * target, INT offset);

// 获取 气味 ID ，（1 ~ +00 ）,字符串包含 #!#
INT GetSmellID(CHAR * taregt, INT length);

// 获取 srt 文件中 时间段（毫秒）
BOOLEAN GetSrtTimePeriod(CHAR * string, INT length, INT  * StartEnd);


UINT64 GetTimestamp();
// 将毫秒转为 HMSF 格式的时间字符串
VOID MilliSecond2HMSF(INT ms, TCHAR * msString);

DWORD char2Wchar(char * sBuf, int sBufSize, WCHAR * output);

VOID ShowLastErrorMsg(LPTSTR lpszFunction);

VOID InitScentrealmFunctions(HINSTANCE g_ScentRealm_DLL, struct ScentrealmRuntime * runtime);

INT RandomInt(INT min, INT max);
BOOL DoubleEqual(DOUBLE a, DOUBLE b);
// 截屏
HBITMAP ScreenCapture(LPCSTR filename, WORD BitCount, LPRECT lpRect);
// 模拟键盘输入，低四位分别 代表 是否 Shift、Ctrl、Win、Alt

VOID SimulateKeyArrayInput(WORD  Keys[], CHAR Count);

WORD ConvertChar2KeyWordAndSimulate(string str);

INT IndexOf(CHAR *data, INT length, CHAR chr);

INT ListFilesWithExt_NDP(const string& folder_path, vector<string> &ResultVector, string Ext);

int DOMatch(string img_name, string templ_name);

string FileDigest(const string &file);

typedef int(*pScentrealmInit)();
typedef int(*pScentrealmPlaySmell)(int smell, int duration);
typedef int(*pScentrealmStopPlay)();
typedef int(*pScentrealmGetConnectStatus)();
typedef int(*pScentrealmClose)();

struct ScentrealmRuntime
{
	unsigned char DllState;  // is dll imported & function initialized
	unsigned long LastErrorCode;   // dll error code
	unsigned char ConnectState;    // connect status ,0 device connected，1 device unconnected，2 controller unconnected

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


