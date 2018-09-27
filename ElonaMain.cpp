// WinGHotKey.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h"
#include "ElonaC.h"
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

static VOID ShowIOR_ItemsWithInput()
{
	int x = 0, y = 0, w = 0, h = 0;

	Mat Src = imread("../data/Src/shop.bmp");
	while (~scanf_s("%d %d %d %d", &x, &y, &w, &h))
	{
		if (x < 0 || y < 0 || w < 0 || h < 0)
			break;
		ShowIOR_Items(Src, x, y, w, h);
	}
}



static DWORD WINAPI MyThreadFunction(LPVOID lpParam)
{
	Mat image = imread("../data/Src/shop.bmp");
	// First Item TopLeft in ScreenCapture 170 165 542 19
	Rect TopLeftRect(170,165,542,19);

	DoThreshold(image, image, 150, 255);

	namedWindow("SRC");
	moveWindow("SRC", 600,  60);
	imshow("SRC", image);

	Mat Empty(19, 6, CV_32FC1, Scalar::all(0));

	int count = 0;
	for (int i = TopLeftRect.y; i < image.rows - TopLeftRect.height; i += TopLeftRect.height)
	{
		Mat imageROI = image(Rect(TopLeftRect.x, i, TopLeftRect.width, TopLeftRect.height));
		CHAR name[50] = { 0 };
		sprintf_s(name, "IOR-%d", i);
		namedWindow(name);
		moveWindow(name, 0, count * 60);
		imshow(name, imageROI);


		Point MinLoc = GetMatchedStartPointOnly(imageROI, Empty,0);
		rectangle(imageROI, MinLoc, Point(MinLoc.x + Empty.cols, MinLoc.y + Empty.rows), Scalar(0, 0, 255), 2, 8, 0);

		count++;
		if (count > 15)
			break;
	}
	waitKey();

	return 0;
}


VOID FindAndCloseWindow()
{
	LPCWSTR name = L"IOR-165";
	HWND hd = FindWindow(NULL, name);
	printf("hd = %d\n" , hd);
	PostMessage(hd,WM_CLOSE,NULL, NULL);
}




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
	ATOM m_HotKeyId6 = GlobalAddAtom(_T("WinHotKeySRCtl-WhatIdThis")) - 0xc000;
	ATOM m_HotKeyId7 = GlobalAddAtom(_T("WinHotKeySRCtl-ScreenShot")) - 0xc000;
	ATOM m_HotKeyId8 = GlobalAddAtom(_T("WinHotKeySRCtl-StopCycle")) - 0xc000;
	ATOM m_HotKeyId9 = GlobalAddAtom(_T("WinHotKeySRCtl-Test")) - 0xc000;
	_tprintf(L"Register HotKeys ... VK_NUMPAD1 ~ VK_NUMPAD9\n");
	LocalRegisterHotKey(hWnd, m_HotKeyId1, MOD_NOREPEAT, VK_NUMPAD1);
	LocalRegisterHotKey(hWnd, m_HotKeyId2, MOD_NOREPEAT, VK_NUMPAD2);
	LocalRegisterHotKey(hWnd, m_HotKeyId3, MOD_NOREPEAT, VK_NUMPAD3);
	LocalRegisterHotKey(hWnd, m_HotKeyId4, MOD_NOREPEAT, VK_NUMPAD4);
	LocalRegisterHotKey(hWnd, m_HotKeyId5, MOD_NOREPEAT, 0x33);
	LocalRegisterHotKey(hWnd, m_HotKeyId6, MOD_NOREPEAT, VK_NUMPAD6);
	LocalRegisterHotKey(hWnd, m_HotKeyId7, MOD_NOREPEAT, VK_NUMPAD7);
	LocalRegisterHotKey(hWnd, m_HotKeyId8, MOD_NOREPEAT, VK_NUMPAD8);
	LocalRegisterHotKey(hWnd, m_HotKeyId9, MOD_NOREPEAT, VK_NUMPAD9);

	_tprintf(L"Press Key `1` To Start 1.bat \n");
	_tprintf(L"Press Key `2` To Start 2.bat \n");
	_tprintf(L"Press Key `3` To Exit\n");
	//_tprintf(L"Press Key `4` To Ka Bug\n");
	//_tprintf(L"Press Key `5` To Cycle Ka Bug 50\n");
	//_tprintf(L"Press Key `6` To Exit\n");
	//_tprintf(L"Press Key `7` To Scan ScreenCapture\n");
	//_tprintf(L"Press Key `8` To Stop Cycle Ka Bug\n");
	//_tprintf(L"Press Key `9` To Ka Bug Key Frame\n");

//	Mat Empty(6,19, CV_8UC1,Scalar::all(0));


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

				HANDLE  hThreadArray;
				DWORD   dwThreadIdArray;
				INT * a = new INT[4]{ 1,2,3,4 };
				hThreadArray = CreateThread(
					NULL,                   // default security attributes
					0,                      // use default stack size  
					MyThreadFunction,       // thread function name
					a,          // argument to thread function 
					0,                      // use default creation flags 
					&dwThreadIdArray);   // returns the thread identifier 

				printf("Mission 4 Over\n");
			}
			else if (m_HotKeyId5 == msg.wParam) {

				CHAR name[50] = { 0 };
				sprintf_s(name, "../data/ScreenShot/%lld.jpg", GetTimestamp());
				//	CHAR * addr = "../data/mix.bmp";
				GetScreenCapture_LogArea(name);
				SplitFontImg_AutoIOR(name);
			}
			else if (m_HotKeyId6 == msg.wParam) {

				GetMatchedStartPointWithFName("../data/Src/backpack.bmp", "../data/Src/de.bmp", 0, FALSE);
				GetMatchedStartPointWithFName("../data/Src/backpack.bmp", "../data/Src/de1.bmp", 0, FALSE);

				// SplitFontImgTest_AutoIOR("../data/Screen.bmp");
			}
			else if (m_HotKeyId7 == msg.wParam) {

				Mat imag, result;
				imag = imread("../data/ior1.bmp", 0);	//将读入的彩色图像直接以灰度图像读入
				namedWindow("原图", 1);
				imshow("原图", imag);
				result = imag.clone();
				//进行二值化处理，选择30，200.0为阈值
				threshold(imag, result, 50.0, 255, CV_THRESH_BINARY);
				namedWindow("二值化图像");
				imshow("二值化图像", result);
				imwrite("../data/threshold.jpg", result);

				Mat img = imread("../data/threshold.jpg", IMREAD_COLOR);
				Mat templ = imread("../data/empty.jpg", IMREAD_COLOR);
				Point MinPoint = GetMatchedStartPoint(img, templ, 0);
				printf("matchLoc = (%d,%d)\n", MinPoint.x, MinPoint.y);
				waitKey(0);
				/*LPSTR addr = "ScreenCapture.png";
				ScreenCapture(addr, 32, NULL);
				Mat src = imread("ScreenCapture.png");
				*/
			}
			else if (m_HotKeyId8 == msg.wParam) {
				G_StopCycle = TRUE;
			}
			else if (m_HotKeyId9 == msg.wParam) {
				printf("Mission m_HotKeyId9 Start\n");
				//FindAndCloseWindow();

				destroyAllWindows();

				printf("Mission m_HotKeyId9 Over\n");
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