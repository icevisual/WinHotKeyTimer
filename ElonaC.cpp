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
static BOOL G_StopCycle = FALSE;
static FileConfig g_config;

INT main(int argc, TCHAR * argv[]) {


	HWND hWnd = NULL;		// 窗口句柄
	HANDLE hThread = NULL;	// 多线程句柄
	TCHAR sourceFilename[] = L"config.conf";
	g_config.LoadConfigFromFile(sourceFilename);
	_tprintf(L"Load config ...\n");


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
	LocalRegisterHotKey(hWnd, m_HotKeyId4, MOD_NOREPEAT, 0x33);
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
		//		Sleep(3000);
				WORD k[] = { VK_ESCAPE };
				SimulateKeyArrayInput(k, 1);
				Sleep(200);
		/*		SimulateKeyArrayInput(k, 1);
				Sleep(200);
				ConvertChar2KeyWordAndSimulate("a");*/
			}
			else if (m_HotKeyId5 == msg.wParam) {
				
			}
			else if (m_HotKeyId7 == msg.wParam) {
				LPSTR addr = "ScreenCapture.png";
				ScreenCapture(addr, 32, NULL);
				Mat src = imread("ScreenCapture.png");

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