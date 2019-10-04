#include "utils.h"

#include "stdafx.h"
#include <vector>
#include <iostream>  
#include <map>
#include <io.h>
#include <direct.h>
using namespace std;
using std::vector;
using std::string;


int MakeDIR(string dir)
{
	if (_access(dir.c_str(), 0) == -1)
	{
		int flag = _mkdir(dir.c_str());
		return flag;
	}
	return 0;
}




//LPBYTE lpBuf = (LPBYTE)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwNeedSize);
//if (lpBuf)
//{
//	// ��ȡ�ַ�ͼ�ε����ݵ���������
//	GetGlyphOutline(hDC, TextDemo[i], GGO_BITMAP, &gm, dwNeedSize, lpBuf, &mat2);
//
//	OutputFontGlyphFixSizeToByteArrayToFile(gm, lpBuf, SmellName_FSize, SmellName_FSize, DistFfp);
//	//
//	HeapFree(GetProcessHeap(), 0, lpBuf);
//}
//   ����̨ǰ����ɫ
enum ConsoleForegroundColor
{
	enmCFC_Red = FOREGROUND_INTENSITY | FOREGROUND_RED,
	enmCFC_Green = FOREGROUND_INTENSITY | FOREGROUND_GREEN,
	enmCFC_Blue = FOREGROUND_INTENSITY | FOREGROUND_BLUE,
	enmCFC_Yellow = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN,
	enmCFC_Purple = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE,
	enmCFC_Cyan = FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE,
	enmCFC_Gray = FOREGROUND_INTENSITY,
	enmCFC_White = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
	enmCFC_HighWhite = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
	enmCFC_Black = 0,
};

enum ConsoleBackGroundColor
{
	enmCBC_Red = BACKGROUND_INTENSITY | BACKGROUND_RED,
	enmCBC_Green = BACKGROUND_INTENSITY | BACKGROUND_GREEN,
	enmCBC_Blue = BACKGROUND_INTENSITY | BACKGROUND_BLUE,
	enmCBC_Yellow = BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN,
	enmCBC_Purple = BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_BLUE,
	enmCBC_Cyan = BACKGROUND_INTENSITY | BACKGROUND_GREEN | BACKGROUND_BLUE,
	enmCBC_White = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE,
	enmCBC_HighWhite = BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE,
	enmCBC_Black = 0,
};
// ���ÿ���̨�����ɫ
void SetConsoleColor(ConsoleForegroundColor foreColor = enmCFC_White, ConsoleBackGroundColor backColor = enmCBC_Black)
{
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(handle, foreColor | backColor);
}

string FileDigest(const string &file)
{
	ifstream in(file.c_str(), ios::binary);
	if (!in)
		return "";

	MD5 md5;
	std::streamsize length;
	char buffer[1024];
	while (!in.eof()) {
		in.read(buffer, 1024);
		length = in.gcount();
		if (length > 0)
			md5.update(buffer, length);
	}
	in.close();
	return md5.toString();
}


UINT64 GetTimestamp()
{
	struct timeval tv;
	time_t clock;
	struct tm tm;
	SYSTEMTIME wtm;

	GetLocalTime(&wtm);
	tm.tm_year = wtm.wYear - 1900;
	tm.tm_mon = wtm.wMonth - 1;
	tm.tm_mday = wtm.wDay;
	tm.tm_hour = wtm.wHour;
	tm.tm_min = wtm.wMinute;
	tm.tm_sec = wtm.wSecond;
	tm.tm_isdst = -1;
	clock = mktime(&tm);
	tv.tv_sec = clock;
	tv.tv_usec = wtm.wMilliseconds * 1000;
	return ((unsigned long long)tv.tv_sec * 1000 + (unsigned long long)tv.tv_usec / 1000);
}

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


VOID GetHMS_CHAR(CHAR * output)
{
	SYSTEMTIME sys;
	GetLocalTime(&sys);
	sprintf_s(output, 64, "%02d:%02d:%02d.%03d", sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds);
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

VOID TrimNewLine(CHAR * string, INT length) {

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


// �� dd:dd:dd,ddd ת���� ����
INT ConvertHMSF2Int(CHAR * target, INT offset) {
	// "dd:dd:dd,ddd"

	INT h = (target[offset] - '0') * 10 + target[offset + 1] - '0';
	INT m = (target[offset + 3] - '0') * 10 + target[offset + 4] - '0';
	INT s = (target[offset + 6] - '0') * 10 + target[offset + 7] - '0';
	INT f = (target[offset + 9] - '0') * 100 + (target[offset + 10] - '0') * 10 + target[offset + 11] - '0';
	return h * 3600000 + m * 60000 + s * 1000 + f;
}



// ��ȡ ��ζ ID ����1 ~ +00 ��,�ַ������� #!#
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


DWORD char2Wchar(const char * sBuf, int sBufSize, WCHAR * output)
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



INT RandomInt(INT min, INT max)
{
	static time_t i = 0;
	if (i == 0)
	{
		srand((unsigned)time(NULL));
		i++;
	}

	INT rd = rand();

	rd %= (max - min);

	return rd + min;
}
BOOL DoubleEqual(DOUBLE a, DOUBLE b)
{
	if (abs(a - b) < 1e-10)
	{
		return 1;
	}
	return 0;
}


HBITMAP ScreenCapture(LPCSTR filename, WORD BitCount, LPRECT lpRect)
{
	HBITMAP hBitmap;
	// ��ʾ����ĻDC
	HDC hScreenDC = CreateDC(L"DISPLAY", NULL, NULL, NULL);
	HDC hmemDC = CreateCompatibleDC(hScreenDC);
	// ��ʾ����Ļ�Ŀ�͸�
	int ScreenWidth = GetDeviceCaps(hScreenDC, HORZRES);
	int ScreenHeight = GetDeviceCaps(hScreenDC, VERTRES);
	// �ɵ�BITMAP�������������ȡ��λ�ý���
	HBITMAP hOldBM;
	// ����λͼ����
	PVOID lpvpxldata;
	// ������ȡ�ĳ������
	INT ixStart;
	INT iyStart;
	INT iX;
	INT iY;
	// λͼ���ݴ�С
	DWORD dwBitmapArraySize;
	// ������С
	DWORD nBitsOffset;
	DWORD lImageSize;
	DWORD lFileSize;
	// λͼ��Ϣͷ
	BITMAPINFO bmInfo;
	// λͼ�ļ�ͷ
	BITMAPFILEHEADER bmFileHeader;
	// д�ļ���
	HANDLE hbmfile;
	DWORD dwWritten;

	// ���LPRECT ΪNULL ��ȡ������Ļ
	if (lpRect == NULL)
	{
		ixStart = iyStart = 0;
		iX = ScreenWidth;
		iY = ScreenHeight;
	}
	else
	{
		ixStart = lpRect->left;
		iyStart = lpRect->top;
		iX = lpRect->right - lpRect->left;
		iY = lpRect->bottom - lpRect->top;
	}
	// ����BTIMAP
	hBitmap = CreateCompatibleBitmap(hScreenDC, iX, iY);
	// ��BITMAPѡ�����ڴ�DC��
	hOldBM = (HBITMAP)SelectObject(hmemDC, hBitmap);
	// BitBlt��ĻDC���ڴ�DC�����������ȡ�Ļ�ȡ���ò���
	BitBlt(hmemDC, 0, 0, iX, iY, hScreenDC, ixStart, iyStart, SRCCOPY);
	// ���ɵ�BITMAP����ѡ����ڴ�DC������ֵΪ���滻�Ķ��󣬼�����ȡ��λͼ
	hBitmap = (HBITMAP)SelectObject(hmemDC, hOldBM);
	if (filename == NULL)
	{
		DeleteDC(hScreenDC);
		DeleteDC(hmemDC);
		return hBitmap;
	}
	// Ϊλͼ���������ڴ�ռ�
	dwBitmapArraySize = ((((iX * 32) + 31) & ~31) >> 3)* iY;
	lpvpxldata = HeapAlloc(GetProcessHeap(), HEAP_NO_SERIALIZE, dwBitmapArraySize);
	ZeroMemory(lpvpxldata, dwBitmapArraySize);

	// ��� BITMAPINFO �ṹ
	ZeroMemory(&bmInfo, sizeof(BITMAPINFO));
	bmInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmInfo.bmiHeader.biWidth = iX;
	bmInfo.bmiHeader.biHeight = iY;
	bmInfo.bmiHeader.biPlanes = 1;
	bmInfo.bmiHeader.biBitCount = BitCount;
	bmInfo.bmiHeader.biCompression = BI_RGB;

	// ��� BITMAPFILEHEADER �ṹ
	ZeroMemory(&bmFileHeader, sizeof(BITMAPFILEHEADER));
	nBitsOffset = sizeof(BITMAPFILEHEADER) + bmInfo.bmiHeader.biSize;
	lImageSize =
		((((bmInfo.bmiHeader.biWidth * bmInfo.bmiHeader.biBitCount) + 31) & ~31) >> 3)
		* bmInfo.bmiHeader.biHeight;
	lFileSize = nBitsOffset + lImageSize;
	bmFileHeader.bfType = 'B' + ('M' << 8);
	bmFileHeader.bfSize = lFileSize;
	bmFileHeader.bfOffBits = nBitsOffset;

	// ��ȡDIB����д�뵽�ļ�
	GetDIBits(hmemDC, hBitmap, 0, bmInfo.bmiHeader.biHeight,
		lpvpxldata, &bmInfo, DIB_RGB_COLORS);
	// д�ļ�
	hbmfile = CreateFileA(filename,
		GENERIC_WRITE,
		FILE_SHARE_WRITE,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (hbmfile == INVALID_HANDLE_VALUE)
	{
		MessageBoxA(NULL, "create file error", "error", MB_OK);
	}

	WriteFile(hbmfile, &bmFileHeader, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);
	WriteFile(hbmfile, &bmInfo, sizeof(BITMAPINFO), &dwWritten, NULL);
	WriteFile(hbmfile, lpvpxldata, lImageSize, &dwWritten, NULL);
	CloseHandle(hbmfile);
	//	MessageBox(NULL,"�ļ�д��ɹ�","error",MB_OK);

	// �ͷ��ڴ棬�����ͬ��DC��
	// ����û��ɾ��BITMAP����������ʾ��ɺ�ɾ��
	HeapFree(GetProcessHeap(), HEAP_NO_SERIALIZE, lpvpxldata);
	ReleaseDC(0, hScreenDC);
	DeleteDC(hmemDC);
	return hBitmap;
}

// ģ���������
VOID SimulateKeyArrayInput(WORD  Keys[], CHAR Count)
{
	INT RevK = 0;
	INPUT input[10] = {0};
	CHAR KeyCount = Count * 2;
	memset(input, 0, Count * sizeof(INPUT));
	for (int i = 0; i < Count; i++)
	{
		RevK = KeyCount - 1 - i;
		input[i].ki.wVk = input[RevK].ki.wVk = Keys[i];
		input[RevK].ki.dwFlags = KEYEVENTF_KEYUP;
		input[i].type = input[RevK].type = INPUT_KEYBOARD;
	}
	//�ú����ϳɼ����¼�������¼�������ģ�������߼��̲������¼����������������߼��̴����������
	SendInput(KeyCount, input, sizeof(INPUT));
}



WORD ConvertChar2KeyWordAndSimulate(string str)
{
	static BOOL IsMapInited = FALSE;
	static map<CHAR, WORD> SpecialCharsMapShift;
	static map<CHAR, WORD> SpecialCharsMapNoShift;
	if (IsMapInited == FALSE)
	{
		CHAR SpecialCharsShift[][2] = {
			{ ')',0x30 } ,{ '!',0x31 } ,{ '@',0x32 } ,{ '#',0x33 } ,{ '$',0x34 } ,
			{ '%',0x35 } ,{ '^',0x36 } ,{ '&',0x37 } ,{ '*',0x38 } ,{ '(',0x39 } ,
			{ '~',VK_OEM_3 } ,{ '_',VK_OEM_MINUS },
			{ '{',VK_OEM_4 } ,{ '}',VK_OEM_6 },{ '|',VK_OEM_5 } ,
			{ ':',VK_OEM_1 } ,{ '"',VK_OEM_7 },
			{ '<',VK_OEM_COMMA } ,{ '?',VK_OEM_2 } ,
			{ '>',VK_OEM_PERIOD }
		};
		CHAR SpecialCharsNoShift[][2] = {
			{ '`',VK_OEM_3 } ,{ '-',VK_OEM_MINUS },{ '=',VK_OEM_PLUS } ,{ '+',VK_ADD } ,
			{ '[',VK_OEM_4 } ,{ ']',VK_OEM_6 },{ '\\',VK_OEM_5 } ,
			{ ';',VK_OEM_1 } ,{ '\'',VK_OEM_7 },
			{ ',',VK_OEM_COMMA } ,{ '.',VK_DECIMAL } ,{ '/',VK_OEM_2 }
		};
		INT SpecialCharsShiftLength = sizeof(SpecialCharsShift) / (2 * sizeof(char));
		INT SpecialCharsNoShiftLength = sizeof(SpecialCharsNoShift) / (2 * sizeof(char));
		for (int i = 0; i < SpecialCharsShiftLength; i++)
		{
			SpecialCharsMapShift.insert(pair<CHAR, WORD>(SpecialCharsShift[i][0], SpecialCharsShift[i][1]));
		}
		for (int i = 0; i < SpecialCharsNoShiftLength; i++)
		{
			SpecialCharsMapNoShift.insert(pair<CHAR, WORD>(SpecialCharsNoShift[i][0], SpecialCharsNoShift[i][1]));
		}
		IsMapInited = TRUE;
	}
	
	const char * chrs = str.c_str();
	vector<WORD> Result;
	INT Length = str.length();
	for (int i = 0; i < Length; i++)
	{
		if (chrs[i] >= '0' && chrs[i] <= '9')
			Result.push_back(0x30 + chrs[i] - '0');
		else if (chrs[i] >= 'a' && chrs[i] <= 'z')
			Result.push_back(chrs[i] - 'a' + 'A');
		else if (chrs[i] >= 'A' && chrs[i] <= 'Z')
		{
			Result.push_back(VK_SHIFT);
			Result.push_back(chrs[i]);
		}
		else 
		{
			map<CHAR, WORD>::iterator l_it;;
			l_it = SpecialCharsMapShift.find(chrs[i]);
			if (l_it == SpecialCharsMapShift.end())
			{
				Result.push_back(l_it->second);
			}
			else
			{
				l_it = SpecialCharsMapNoShift.find(chrs[i]);
				if (l_it == SpecialCharsMapNoShift.end())
				{
					Result.push_back(VK_SHIFT);
					Result.push_back(l_it->second);
				}
				else
				{
					printf("==========What is this %c\n", chrs[i]);
				}
			}
		}
		SimulateKeyArrayInput(&Result[0], Result.size());
		Result.clear();
	}
	return 1;
}


INT IndexOf(CHAR *data, INT length, CHAR chr)
{
	for (int i = 0; i < length; i++)
		if (data[i] == chr)
			return i;
	return -1;
}

// List Files With Extension No Deep Search
INT ListFilesWithExt_NDP(const string& folder_path, vector<string> &result_vector, string Ext)
{
	result_vector.clear();
	_finddata_t file;
	long flag;
	string filename = folder_path + "\\*" + Ext;//�����ƶ��ļ����ڵ�jpg�ļ�
	if ((flag = _findfirst(filename.c_str(), &file)) == -1)//Ŀ¼���Ҳ����ļ�
	{
		cout << "There is no such type file" << endl;
		return -1;
	}
	else
	{
		//ͨ��ǰ���_findfirst�ҵ���һ���ļ�
		string name = folder_path + "\\" + file.name;//file.name��ŵ��Ǳ����õ����ļ���
		result_vector.push_back(name);
		//����Ѱ���Ժ���ļ�
		while (_findnext(flag, &file) == 0)
		{
			string name = string(folder_path + "\\" + string(file.name));
			result_vector.push_back(name);
		}
	}
	_findclose(flag);
	return 1;
}










VOID LeftClick()
{
	INPUT    Input = { 0 };
	// left down 
	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
	::SendInput(1, &Input, sizeof(INPUT));

	Sleep(160);
	// left up
	::ZeroMemory(&Input, sizeof(INPUT));
	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
	::SendInput(1, &Input, sizeof(INPUT));
}

VOID LeftClick_Hold()
{
	INPUT    Input = { 0 };
	// left down 
	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
	::SendInput(1, &Input, sizeof(INPUT));
}
VOID LeftClick_Release()
{
	INPUT    Input = { 0 };
	::ZeroMemory(&Input, sizeof(INPUT));
	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
	::SendInput(1, &Input, sizeof(INPUT));
}

VOID SimulateMouseClick(int x, int y)
{
	::SetCursorPos(x, y);

	LeftClick();
	//	mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
	//mouse_event(MOUSEEVENTF_LEFTDOWN , 0, 0, 0, 0);
	//mouse_event(MOUSEEVENTF_XDOWN, 0, 0, 0, 0);
	//mouse_event(MOUSEEVENTF_XUP, 0, 0, 0, 0);
}

VOID SimulateMouseClick_Hold(int x, int y)
{
	::SetCursorPos(x, y);

	LeftClick_Hold();
	//	mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
	//mouse_event(MOUSEEVENTF_LEFTDOWN , 0, 0, 0, 0);
	//mouse_event(MOUSEEVENTF_XDOWN, 0, 0, 0, 0);
	//mouse_event(MOUSEEVENTF_XUP, 0, 0, 0, 0);
}
