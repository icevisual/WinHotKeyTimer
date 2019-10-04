
#pragma once

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



#define SCENTREALM_DEBUG

#ifdef SCENTREALM_DEBUG

#ifndef  DEBUG_LOG

#define USING_DEBUG_LOG_VARS static CHAR DEBUG_LOG_CTime[64] = { 0 };static CHAR DEBUG_LOG_MSG[640] = { 0 };
#define DEBUG_LOG(fmt, ...) sprintf_s(DEBUG_LOG_MSG,640, fmt, __VA_ARGS__) ;GetHMS_CHAR(DEBUG_LOG_CTime); fprintf(stdout, "[%s] %s",DEBUG_LOG_CTime , DEBUG_LOG_MSG)
#define DEBUG_LOGN(fmt, ...) sprintf_s(DEBUG_LOG_MSG,640, fmt, __VA_ARGS__) ;GetHMS_CHAR(DEBUG_LOG_CTime); fprintf(stdout, "[%s] %s\n",DEBUG_LOG_CTime , DEBUG_LOG_MSG)

// #define DEBUG_LOG(fmt, ...) _stprintf_s(g_MSG, TEXT(fmt), __VA_ARGS__) ;GetHMS(g_CunnentTime); _ftprintf(stdout, TEXT("[%s] %s"),g_CunnentTime , g_MSG)

#endif // ! DEBUG_LOG

#else
#define DEBUG_LOG(fmt, ...) 

#endif



int MakeDIR(string dir);

INT GetMilliSecondOfDay();

VOID GetHMS_CHAR(CHAR * output);

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

DWORD char2Wchar(const char * sBuf, int sBufSize, WCHAR * output);

VOID ShowLastErrorMsg(LPTSTR lpszFunction);



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




VOID LeftClick();
VOID LeftClick_Hold();
VOID LeftClick_Release();

VOID SimulateMouseClick(int x, int y);
VOID SimulateMouseClick_Hold(int x, int y);
