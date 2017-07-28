#include "stdafx.h"


INT GetMilliSecondOfDay()
{
	SYSTEMTIME sys;
	GetLocalTime(&sys);
	//printf("%4d-%02d-%02d %02d:%02d:%02d.%03d\n", sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds);
	return sys.wHour * 3600000 + sys.wMinute * 60000 + sys.wSecond * 1000 + sys.wMilliseconds;
}

VOID GetHMS(TCHAR * output)
{
	SYSTEMTIME sys;
	GetLocalTime(&sys);
	wsprintf(output, _T("%02d:%02d:%02d.%03d"), sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds);
}


VOID LocalRegisterHotKey(_In_opt_ HWND hWnd, _In_ int id, _In_ UINT fsModifiers, _In_ UINT vk) {
	if (RegisterHotKey(hWnd, id, fsModifiers, vk)) //0x42 is 'b'
	{
	///	_tprintf(_T("Hotkey '%c' registered \n"), vk);
	}
	else {
		ShowLastErrorMsg(L"RegisterHotKey");
		system("Pause");
		ExitProcess(2);
	}
}

VOID Trim(CHAR * string, INT length) {

	while (length > 0 && string[length - 1] == '\n')
	{
		string[length - 1] = 0;
		length--;
	}
}


BOOLEAN SimpleRegexMatch(CHAR * target, INT offset, INT length, CHAR * regex, INT rLength, INT * out) {
	// 00:00:10,000 --> 00:00:11,000
	CHAR Regex[] = "dd:dd:dd,ddd";
	INT ri = 0;
	for (int i = offset; i < length; i++)
	{
		if (regex[ri] == 'd') {
			if (target[i] >= '0' && target[i] <= '9') {
				if (ri == 0) out[0] = i;
				ri++;
				if (ri >= rLength) {
					out[1] = i;
					return TRUE;
				}
			}
			else {
				ri = 0;
			}
		}
		else
		{
			if (target[i] == regex[ri]) {
				if (ri == 0) out[0] = i;
				ri++;
				if (ri >= rLength) {
					out[1] = i;
					return TRUE;
				}
			}
			else {
				ri = 0;
			}
		}
	}
	return FALSE;
}


// 将 dd:dd:dd,ddd 转换成 毫秒
INT ConvertHMSF2Int(CHAR * target, INT offset) {
	// "dd:dd:dd,ddd"

	INT h = (target[offset] - '0') * 10 + target[offset + 1] - '0';
	INT m = (target[offset + 3] - '0') * 10 + target[offset + 4] - '0';
	INT s = (target[offset + 6] - '0') * 10 + target[offset + 7] - '0';
	INT f = (target[offset + 9] - '0') * 100 + (target[offset + 10] - '0') * 10 + target[offset + 11] - '0';
	return h * 3600000 + m * 60000 + s * 1000 + f;
}



// 获取 气味 ID ，（1 ~ +00 ）,字符串包含 #!#
INT GetSmellID(CHAR * taregt, INT length) {
	CHAR delimiter[] = "#!#";
	INT out[2] = { 0 };
	INT s = 0;
	if (SimpleRegexMatch(taregt, 0, length, delimiter, strlen(delimiter), out)) {
		for (int i = out[1] + 1; i < length; i++)
		{
			if (taregt[i] >= '0' && taregt[i] <= '9') {
				s = s * 10 + (taregt[i] - '0');
			}
		}
	}
	return s;
}

BOOLEAN GetSrtTimePeriod(CHAR * string, INT length, INT  * StartEnd) {
	CHAR Regex[] = "dd:dd:dd,ddd";
	INT rLength = strlen(Regex);
	INT out[2] = { 0 };
	if (SimpleRegexMatch(string, 0, length, Regex, rLength, out)) {
		INT millSecond = ConvertHMSF2Int(string, out[0]);
		//printf("Match1 %d %d millSecond = %d\n",out[0],out[1], millSecond);
		StartEnd[0] = millSecond;
		if (SimpleRegexMatch(string, out[1] + 1, length, Regex, rLength, out)) {
			millSecond = ConvertHMSF2Int(string, out[0]);
			//printf("Match2 %d %d millSecond = %d\n", out[0], out[1], millSecond);
			StartEnd[1] = millSecond;
			return TRUE;
		}
	}
	return FALSE;
}


VOID MilliSecond2HMSF(INT ms, TCHAR * msString) {
	INT f = ms % 1000;
	ms = ms / 1000;
	INT s = ms % 60;
	ms = ms / 60;
	INT m = ms % 60;
	ms = ms / 60;
	INT h = ms % 60;
	wsprintf(msString, _T("%02d:%02d:%02d,%03d"), h, m, s, f);
}


DWORD char2Wchar(char * sBuf, int sBufSize, WCHAR * output)
{
	DWORD dBufSize = MultiByteToWideChar(CP_ACP, 0, sBuf, sBufSize, NULL, 0);
	WCHAR * dBuf = new WCHAR[dBufSize + 1];
	wmemset(dBuf, 0, dBufSize);
	int nRet = MultiByteToWideChar(CP_ACP, 0, sBuf, sBufSize, dBuf, dBufSize);
	dBuf[dBufSize] = '\0';
	if (nRet <= 0)
		return  GetLastError();
	else
		wcscpy_s(output, dBufSize + 1, dBuf);
	delete(dBuf);
	return 0;
}


VOID ShowLastErrorMsg(LPTSTR lpszFunction)
{
	// Retrieve the system error message for the last-error code

	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;
	DWORD dw = GetLastError();

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0, NULL);

	// Display the error message and exit the process

	lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
		(lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
	StringCchPrintf((LPTSTR)lpDisplayBuf,
		LocalSize(lpDisplayBuf) / sizeof(TCHAR),
		TEXT("%s failed with error %d: %s"),
		lpszFunction, dw, lpMsgBuf);
	MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);
}



VOID InitScentrealmFunctions(HINSTANCE g_ScentRealm_DLL,struct ScentrealmRuntime * runtime) {
	static int is_inited = 0;
	if (is_inited) return;

	g_ScentRealm_DLL = LoadLibrary(L"scentrealm-client.dll");
	if (g_ScentRealm_DLL)
	{
		runtime->DllState = DS_LOADED;

		runtime->pInit			= (pScentrealmInit)GetProcAddress(g_ScentRealm_DLL, "ScentrealmInit");
		runtime->pPlaySmell	= (pScentrealmPlaySmell)GetProcAddress(g_ScentRealm_DLL, "ScentrealmPlaySmell");
		runtime->pStopPlay		= (pScentrealmStopPlay)GetProcAddress(g_ScentRealm_DLL, "ScentrealmStopPlay");
		runtime->pGetConnectStatus		= (pScentrealmGetConnectStatus)GetProcAddress(g_ScentRealm_DLL, "ScentrealmGetConnectStatus");
		runtime->pClose		= (pScentrealmClose)GetProcAddress(g_ScentRealm_DLL, "ScentrealmClose");

		if (!runtime->pInit)
		{
			_tprintf(_T("pScentrealmInit Load Failed \n"));
			runtime->DllState ^= 0x01;
		}
		if (!runtime->pPlaySmell)
		{
			_tprintf(_T("pScentrealmPlaySmell Load Failed \n"));
			runtime->DllState ^= 0x02;
		}
		if (!runtime->pStopPlay)
		{
			_tprintf(_T("pScentrealmStopPlay Load Failed \n"));
			runtime->DllState ^= 0x04;
		}
		if (!runtime->pGetConnectStatus)
		{
			_tprintf(_T("pScentrealmGetConnectStatus Load Failed \n"));
			runtime->DllState ^= 0x08;
		}
		if (!runtime->pClose)
		{
			_tprintf(_T("pScentrealmClose Load Failed \n"));
			runtime->DllState ^= 0x10;
		}
		is_inited = 1;
		if (runtime->DllState == DS_LOADED) {
			runtime->DllState = DS_LOADOV;
			int init_ret = runtime->pInit();
			runtime->ConnectState = runtime->pGetConnectStatus();
			if (CS_DEV_CONNECTED == runtime->ConnectState) {
				_tprintf(_T("ScentrealmDll Init Success \n"));
			}
			else {
				if (CS_DEV_UNCONNECTED == runtime->ConnectState)
					_tprintf(_T("ScentrealmDll Init Failed, Device Not Connected \n"));
				if (CS_CTR_UNCONNECTED == runtime->ConnectState)
					_tprintf(_T("ScentrealmDll Init Failed, Controller Not Connected \n"));
			}
		}
	}
	else
	{
		runtime->DllState = DS_UNLOAD;
		runtime->LastErrorCode = GetLastError();
		ShowLastErrorMsg(_T("LoadLibrary"));
	}
	//FreeLibrary(my_dll);
}
