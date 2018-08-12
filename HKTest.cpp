// WinGHotKey.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h"
//
#include <opencv2/opencv.hpp>  
#include<opencv2/highgui/highgui.hpp>  
#include<opencv2/imgproc/imgproc.hpp>  


#include <cv.h>  
#include <highgui.h>  
#include <iostream>  

#include<windows.h>
#include<Mmsystem.h>
#pragma comment(lib,"winmm.lib")
#include <map> 


using namespace std;
using namespace cv;

static TCHAR g_CunnentTime[64] = { 0 };
typedef map<string, int> ConfigMAP;

ConfigMAP G_Config_Map;

INT IndexOf(CHAR *data,INT length, CHAR chr)
{
	for (int i = 0; i < length; i++)
		if (data[i] == chr)
			return i;
	return -1;
}

// 从 srt 文件 气味播放指令
BOOLEAN LoadConfigFromFile(TCHAR * filename) {
	FILE * fp = NULL;
	errno_t error = _tfopen_s(&fp, filename, _T("r"));
	if (0 != error) {
		ShowLastErrorMsg(L"_tfopen_s");
		return FALSE;
	}
	CHAR line[255] = { 0 };
	INT pos[2] = { 0 }, lLength = 0, smellID = 0;
	INT EqPos = -1;
	while (!feof(fp))
	{
		fgets(line, sizeof(line), fp);
		lLength = strlen(line);

		TrimNewLine(line, lLength);
		lLength = strlen(line);
		if ((EqPos = IndexOf(line, lLength, '=')) > 0 && EqPos < lLength - 1)
		{
			string key_str(line,EqPos);
			string value_str(line + EqPos + 1);
			stringstream value_stream(value_str);
			int value_int = 0;
			value_stream >> value_int;
			G_Config_Map.insert(pair<string, int>(key_str, value_int));
		}
		printf("%s %d\n", line, strlen(line));
	}
	fclose(fp);
	return TRUE;
}

INT Get_From_G_Config(string key_str,INT Default)
{
	printf("Get_From_G_Config (%s %d)\n", key_str.c_str(), Default);
	ConfigMAP::iterator finded;
	finded = G_Config_Map.find(key_str);
	if (finded != G_Config_Map.end())
		return finded->second;
	return Default;
}


BOOL G_StopCycle = FALSE;

VOID RunSomething(INT trytime)
{
	BYTE VK_E = 0x45;
	BYTE VK_R = 0x52;
	BYTE vkc = VK_ESCAPE;
	INT DefaultSleep = 100;
	INT vks[] = {
		VK_ESCAPE , // option
		0xff + 200,
		VK_E,
		0xff + DefaultSleep,
		VK_UP,  // target
		0xff + DefaultSleep,
		VK_E, // use
		0xff + DefaultSleep,
		VK_E, // use
		0xff, // random sleep
		VK_ESCAPE,
		0xff + DefaultSleep,
		VK_RIGHT,  // souls
		0xff + DefaultSleep,
		VK_E,
		0xff + 200,
		VK_E,
		0xff + 200,
	};
	int len = sizeof(vks) / sizeof(vks[0]);

	for (int j = 0; j < trytime; j++)
	{
		for (int i = 0; i < len; i++)
		{
			if (vks[i] == 0xff)
			{
				INT sleep = RandomInt(28, 32);
				_tprintf(_T("Sleep %d \n"), sleep);
				Sleep(sleep);
			}
			else if (vks[i] < 0xff)
			{
				INPUT input[2];
				memset(input, 0, sizeof(input));
				//按下 向下方向键
				input[0].ki.wVk = vks[i];
				input[0].type = INPUT_KEYBOARD;
				//松开 向下方向键
				input[1].ki.wVk = vks[i]; //你的字符
				input[1].type = INPUT_KEYBOARD;
				input[1].ki.dwFlags = KEYEVENTF_KEYUP;
				//该函数合成键盘事件和鼠标事件，用来模拟鼠标或者键盘操作。事件将被插入在鼠标或者键盘处理队列里面
				SendInput(2, input, sizeof(INPUT));
			}
			else
			{
				Sleep(vks[i] - 0xff);
			}
		}
		if(j < trytime - 1)
			Sleep(5000);

		if (G_StopCycle == TRUE)
		{
			G_StopCycle = FALSE;
			break;
		}
	}

}





VOID RunSimulateKeys(INT * vks,INT len)
{
	for (int i = 0; i < len; i++)
	{
		if (vks[i] == 0xff)
		{
			INT sleep = RandomInt(28, 32);
			_tprintf(_T("Sleep %d \n"), sleep);
			Sleep(sleep);
		}
		else if (vks[i] < 0xff)
		{
			INPUT input[2];
			memset(input, 0, sizeof(input));
			//按下 向下方向键
			input[0].ki.wVk = vks[i];
			input[0].type = INPUT_KEYBOARD;
			//松开 向下方向键
			input[1].ki.wVk = vks[i]; //你的字符
			input[1].type = INPUT_KEYBOARD;
			input[1].ki.dwFlags = KEYEVENTF_KEYUP;
			//该函数合成键盘事件和鼠标事件，用来模拟鼠标或者键盘操作。事件将被插入在鼠标或者键盘处理队列里面
			SendInput(2, input, sizeof(INPUT));
		}
		else
		{
			Sleep(vks[i] - 0xff);
		}
	}
}



VOID RunPrepare()
{
	BYTE VK_E = 0x45;
	BYTE VK_R = 0x52;
	BYTE vkc = VK_ESCAPE;
	INT DefaultSleep = 100;
	INT vks[] = {
		VK_ESCAPE , // option
		0xff + 200,
		VK_E,
		0xff + DefaultSleep,
		VK_UP,  // target
		0xff + DefaultSleep,
		VK_E, // use
		0xff + DefaultSleep,
		VK_E, // use
		0xff, // random sleep
		VK_ESCAPE,
		0xff + DefaultSleep,
		VK_RIGHT,  // souls
		0xff + DefaultSleep,
		VK_E,
		0xff + 200,
		VK_E,
		0xff + 200,
	};
	RunSimulateKeys(vks,sizeof(vks) / sizeof(vks[0]));
}



VOID RunUse()
{
	BYTE VK_E = 0x45;
	BYTE VK_R = 0x52;
	BYTE vkc = VK_ESCAPE;
	INT DefaultSleep = 100;
	INT vks[] = {
		VK_E,
		0xff + 200,
		VK_E,
		0xff + 200,
	};
	RunSimulateKeys(vks, sizeof(vks) / sizeof(vks[0]));
}


void get_subimg_in_middle(Mat img,Mat &out,INT target_width, INT ys, INT he)
{
	INT w = img.size().width;
	INT h = img.size().height;
	INT sx = (w - target_width) / 2.0;
	INT ex = (w + target_width) / 2.0;
	out = img(Rect(sx,ys, target_width,he));
}

INT CountPoint(Mat Input)
{
	Mat_<Vec3b> I = Input;
	INT Count_1 = 0;
	for (int i = 0; i < I.rows; ++i)
	{
		for (int j = 0; j < I.cols; ++j)
		{
			if (abs(I(i, j)[0] - 9) < 10 &&
				abs(I(i, j)[1] - 26) < 10 &&
				abs(I(i, j)[2] - 70) < 10)
			{
				//I(i, j)[0] = 0;
				//I(i, j)[1] = 0;
				//I(i, j)[2] = 0;
			}
			else
				Count_1++;
		}

	}
	return Count_1;
}


INT CountROI_CanUseOrNot(Mat src)
{
	Mat ImageROI, ImageROI1;
	get_subimg_in_middle(src, ImageROI, 360, 310, 40);
	INT pC = CountPoint(ImageROI);
	DOUBLE p = 1000000.0 * pC / (360.0 * 40);
	INT iP = ceil(p);

	_tprintf(L"Count = %d, %.6f\n", pC, p);
	// 0.116181
	if (iP == 116181)
	{
		// Can Use
		return 1;
	}
	// 0.214375
	if (iP == 214375)
	{
		// Can'T Use
		return 2;
	}
	return -1;
}


INT CountROI_ConfirmYesNo(Mat src)
{
	Mat ImageROI,ImageROI_1, ImageROI_2;
	get_subimg_in_middle(src, ImageROI, 740, 760, 50);
	ImageROI_1 = ImageROI(Rect(0, 0, 370, 50));
	ImageROI_2 = ImageROI(Rect(370, 0, 370, 50));
	INT pLC = CountPoint(ImageROI_1);
	INT pRC = CountPoint(ImageROI_2);
	INT Total = 370 * 50;
	_tprintf(L"Left = %d,%.6f Right = %d,%.6f\n", pLC, pLC * 1.0/ Total, pRC, pRC * 1.0/ Total);

	DOUBLE pL = pLC * 1.0 / Total;
	DOUBLE pR = pRC * 1.0 / Total;

	if ((DoubleEqual(pL, 0.39) || DoubleEqual(pL, 0.40)) && DoubleEqual(pR, 1.00))
	{
		// Left Selected
		return 1;
	}
	if ((DoubleEqual(pR, 0.39) || DoubleEqual(pR, 0.40)) && DoubleEqual(pL, 1.00))
	{
		// Right Selected
		return 2;
	}
	return -1;
}


VOID BugWorker()
{
	static INT MaxWait			= Get_From_G_Config("MaxWaitForUseEnabled",10000);
	static INT AnimationTime	= Get_From_G_Config("SuccessAnimationTime", 4000);
	static INT FailedTime		= Get_From_G_Config("FailedWaitTime", 1000);

	RunPrepare();

	LPSTR addr = "ScreenCapture.png";
	ScreenCapture(addr, 32, NULL);
	Mat src = imread("ScreenCapture.png");
	
	INT CanUseOrNot = CountROI_CanUseOrNot(src);

	if (CanUseOrNot == 1)
	{
		_tprintf(_T("Can Use\n"));
		RunUse();
		// wait animation
		Sleep(AnimationTime);
	}
	else if (CanUseOrNot == 2)
	{
		_tprintf(_T("Can'T Use\n"));
		
		INT Step = 300;
		INT Cur = 0;
		while (Cur < MaxWait)
		{
			Sleep(Step);
			ScreenCapture(addr, 32, NULL);
			src.clone();
			src = imread("ScreenCapture.png");
			CanUseOrNot = CountROI_CanUseOrNot(src);
			if (CanUseOrNot == 1)
			{
				break;
			}
			Cur += Step;
		}
		if (CanUseOrNot == 1)
		{
			_tprintf(_T("Can Use After Wait(%d)\n"), Cur);
			RunUse();
			// wait animation
			Sleep(AnimationTime);
		}
	}
	else
	{
		_tprintf(_T("Not match\n"));
		Sleep(FailedTime);
	}
}

INT main(int argc, TCHAR * argv[]) {

	HWND hWnd = NULL;		// 窗口句柄
	HANDLE hThread = NULL;	// 多线程句柄
	TCHAR sourceFilename[] = L"config.conf";
	LoadConfigFromFile(sourceFilename);
	_tprintf(L"Load config ...\n");

	INT MaxCycleCount = Get_From_G_Config("MaxCycleCount", 10000);

	MSG msg = { 0 };		// 消息
	DWORD dwThreadId = 0;	// 线程 ID
	DWORD error = 0;
	ATOM m_HotKeyId1 = GlobalAddAtom(_T("WinHotKeySRCtl-PlayScript")) - 0xc000;
	ATOM m_HotKeyId2 = GlobalAddAtom(_T("WinHotKeySRCtl-StopScript")) - 0xc000;
	ATOM m_HotKeyId3 = GlobalAddAtom(_T("WinHotKeySRCtl-Terminate")) - 0xc000;
	ATOM m_HotKeyId4 = GlobalAddAtom(_T("WinHotKeySRCtl-Simulate")) - 0xc000;
	ATOM m_HotKeyId5 = GlobalAddAtom(_T("WinHotKeySRCtl-RepeatSimulate")) - 0xc000;
	ATOM m_HotKeyId7 = GlobalAddAtom(_T("WinHotKeySRCtl-ScreenShot")) - 0xc000;
	ATOM m_HotKeyId8 = GlobalAddAtom(_T("WinHotKeySRCtl-StopCycle")) - 0xc000;
	ATOM m_HotKeyId9 = GlobalAddAtom(_T("WinHotKeySRCtl-Test")) - 0xc000;
	_tprintf(L"Register HotKeys ...\n");
	LocalRegisterHotKey(hWnd, m_HotKeyId1, MOD_NOREPEAT, VK_NUMPAD1);
	LocalRegisterHotKey(hWnd, m_HotKeyId2, MOD_NOREPEAT, VK_NUMPAD2);
	LocalRegisterHotKey(hWnd, m_HotKeyId3, MOD_NOREPEAT, VK_NUMPAD3);
	LocalRegisterHotKey(hWnd, m_HotKeyId4, MOD_NOREPEAT, VK_NUMPAD4);
//	LocalRegisterHotKey(hWnd, m_HotKeyId9, MOD_NOREPEAT, VK_NUMPAD9);
	LocalRegisterHotKey(hWnd, m_HotKeyId5, MOD_NOREPEAT, VK_NUMPAD5);
	LocalRegisterHotKey(hWnd, m_HotKeyId7, MOD_NOREPEAT, VK_NUMPAD7);
	LocalRegisterHotKey(hWnd, m_HotKeyId8, MOD_NOREPEAT, VK_NUMPAD8);

	_tprintf(L"Press Key `1` To Start 1.bat \n");
	_tprintf(L"Press Key `2` To Start 2.bat \n");
	_tprintf(L"Press Key `3` To Exit\n");
	_tprintf(L"Press Key `4` To Ka Bug\n");
	_tprintf(L"Press Key `5` To Cycle Ka Bug 50\n");
	//_tprintf(L"Press Key `6` To Exit\n");
	_tprintf(L"Press Key `7` To Scan ScreenCapture\n");
	_tprintf(L"Press Key `8` To Stop Cycle Ka Bug\n");
	//_tprintf(L"Press Key `9` To Ka Bug Key Frame\n");


	while (GetMessage(&msg, NULL, 0, 0) != 0) {
		if (msg.message == WM_HOTKEY) {
			GetHMS(g_CunnentTime);

			if (m_HotKeyId1 == msg.wParam) {
				// Start Script
				_tprintf(_T("[%s] Button `1`,Save \n"), g_CunnentTime);
				system("1.bat");
			}
			else if (m_HotKeyId2 == msg.wParam) {
				
				// Stop Script
				_tprintf(_T("[%s] Button `2`,Reset \n"), g_CunnentTime);
				system("2.bat");
			}
			else if (m_HotKeyId3 == msg.wParam) {
				// Exit
				_tprintf(_T("[%s] Button `3`,Exit\n"), g_CunnentTime);
				break;
			}
			else if (m_HotKeyId4 == msg.wParam) {
				BugWorker();
			}
			else if (m_HotKeyId5 == msg.wParam) {
				for(int ii = 0 ; ii < MaxCycleCount; ii ++)
					BugWorker();
			}
			else if (m_HotKeyId7 == msg.wParam) {
				LPSTR addr = "ScreenCapture.png";
				ScreenCapture(addr, 32, NULL);
				Mat src = imread("ScreenCapture.png");
				CountROI_CanUseOrNot(src);
				CountROI_ConfirmYesNo(src);
			}
			else if (m_HotKeyId8 == msg.wParam) {
				G_StopCycle = TRUE;
			}
			else if (m_HotKeyId9 == msg.wParam) {
				
			}
		}
	}
	CloseHandle(hThread);
	UnregisterHotKey(hWnd, m_HotKeyId1);
	UnregisterHotKey(hWnd, m_HotKeyId2);
	UnregisterHotKey(hWnd, m_HotKeyId3);
	GlobalDeleteAtom(m_HotKeyId1);
	GlobalDeleteAtom(m_HotKeyId2);
	GlobalDeleteAtom(m_HotKeyId3);
	//system("pause");

	return 0;
}