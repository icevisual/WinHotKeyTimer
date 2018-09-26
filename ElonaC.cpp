// WinGHotKey.cpp : �������̨Ӧ�ó������ڵ㡣
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



VOID GetIOR(int x,int y ,int w,int h)
{ // GetIOR(127, 540 ,676 ,68);
	// 127, 540 ,676 ,68
	Mat image = cv::imread("../data/ior.bmp");

	if (x + w > image.cols)
	{
		w = image.cols - x;
	}
	if (y + h > image.rows)
	{
		h = image.cols - y;
	}

	Mat imageROI = image(Rect(x,y,w,h));
	namedWindow("src");
	imshow("src", imageROI);
	imwrite("../data/ior1.bmp", imageROI);
	waitKey(0);
}

// Method: \n 0: SQDIFF \n 1: SQDIFF NORMED \n 2: TM CCORR \n 3: TM CCORR NORMED \n 4: TM COEFF \n 5: TM COEFF NORMED
Point GetMatchedStartPoint(Mat img, Mat templ, int match_method, BOOL show_img = FALSE)
{
	Mat img_display;
	Mat result;
	img.copyTo(img_display);
	int result_cols = img.cols - templ.cols + 1;
	int result_rows = img.rows - templ.rows + 1;
	result.create(result_rows, result_cols, CV_32FC1);
	matchTemplate(img, templ, result, match_method);
	normalize(result, result, 0, 1, NORM_MINMAX, -1, Mat());
	double minVal; double maxVal;
	Point minLoc; Point maxLoc;
	Point matchLoc;
	minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());
	if (match_method == TM_SQDIFF || match_method == TM_SQDIFF_NORMED)
	{
		matchLoc = minLoc;
	}
	else
	{
		matchLoc = maxLoc;
	}
	if (show_img == TRUE)
	{
		rectangle(img_display, matchLoc, Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows), Scalar(0, 0, 255), 2, 8, 0);
		rectangle(result, matchLoc, Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows), Scalar::all(0), 2, 8, 0);
		imshow("MT:Source Image", img_display);
		imshow("MT:matchTemplate Result Image", result);
	}
	return matchLoc;
}


// Method: \n 0: SQDIFF \n 1: SQDIFF NORMED \n 2: TM CCORR \n 3: TM CCORR NORMED \n 4: TM COEFF \n 5: TM COEFF NORMED
Point GetMatchedStartPoint1(Mat img, Mat templ, int match_method)
{
	Mat result;
	int result_cols = img.cols - templ.cols + 1;
	int result_rows = img.rows - templ.rows + 1;
	result.create(result_rows, result_cols, CV_32FC1);
	matchTemplate(img, templ, result, match_method);
	normalize(result, result, 0, 1, NORM_MINMAX, -1, Mat());
	double minVal; double maxVal;
	Point minLoc; Point maxLoc;
	Point matchLoc;
	minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());
	if (match_method == TM_SQDIFF || match_method == TM_SQDIFF_NORMED)
	{
		matchLoc = minLoc;
	}
	else
	{
		matchLoc = maxLoc;
	}
	return matchLoc;
}

VOID SplitFontImgTest(Mat TextSrc)
{
	INT TextPaddingTop = 3;
	INT TextPaddingLeft = 30;
	INT TextHeight = 15;
	INT TextYOffset = 4;
	INT Width = TextSrc.cols - 30;
	Mat Rows[4];

	Mat empty = imread("../data/empty.jpg", IMREAD_COLOR);
	Mat templ = empty(Rect(0,0, empty.cols, TextHeight));
	INT Offset = TextSrc.rows;
	INT TitleHeight = 40;
	imshow("TextSrc", TextSrc);
	moveWindow("TextSrc",0,0);
	for (int i = 0; i < 4; i++)
	{
		Offset = TextSrc.rows;
		Mat temp_gray;
		Mat temp_binary;
		Mat temp_match;
		Mat temp = TextSrc(Rect(TextPaddingLeft, TextYOffset + i * TextHeight, Width, TextHeight));
		
		cvtColor(temp, temp_gray, CV_BGR2GRAY);
		
		threshold(temp_gray, temp_binary, 70 + i * 25, 255, CV_THRESH_BINARY);

		cvtColor(temp_binary, temp_match, CV_GRAY2BGR);
		cv::String temp_window = "temp";
		cv::String  gray_window =  "gray";
		cv::String binary_window = "binary";
		cv::String  match_window =  "match";
		cv::String  result_window = "result";
		temp_window += '0' + i;
		gray_window += '0' + i;
		binary_window += '0' + i;
		match_window += '0' + i;
		result_window += '0' + i;
		imshow(temp_window, temp);
		imshow(gray_window, temp_gray);
		imshow(binary_window, temp_binary);
		imshow(match_window, temp_match);
		Offset += TitleHeight;
		moveWindow(temp_window, i * 400, Offset);
		Offset += TitleHeight + temp.rows;
		moveWindow(gray_window, i * 400, Offset);
		Offset += TitleHeight + temp_gray.rows;
		moveWindow(binary_window, i * 400, Offset);
		Offset += TitleHeight + temp_binary.rows;
		moveWindow(match_window, i * 400, Offset);
	
		for (int j = 0; j < 6; j++)
		{
			result_window = "result";
			result_window += '0' + i;
			Point Loc = GetMatchedStartPoint1(temp_match, templ, j);
			if (Loc.x > 50)
			{
				// Show Image
				result_window += '0' + j;
				Rows[i] = temp_match(Rect(0, 0, Loc.x, TextHeight));
				imshow(result_window, Rows[i]);
				Offset += TitleHeight + temp_match.rows;
				moveWindow(result_window, i * 400, Offset);

				//cv::String tname = "../data/Split/";
				//tname += ('0' + i);
				//tname += ".jpg";
				//printf("%s \n", tname.c_str());
				//imwrite(tname, Rows[i]);
			}
			printf("%d %d \n", Loc.x, Loc.y);
		}
	}
	waitKey();
}


VOID SplitFontImgTest_AutoIOR(string filename)
{
	Mat Src = imread(filename, IMREAD_COLOR);
	Mat IOR;
	if (Src.rows > 100)
		IOR = Src(Rect(127, 540, 676, 68));
	else
		IOR = Src;
	SplitFontImgTest(IOR);
}


VOID SplitFontImg(Mat TextSrc)
{
	INT TextPaddingTop = 3;
	INT TextPaddingLeft = 30;
	INT TextHeight = 15;
	INT TextYOffset = 4;
	INT Width = TextSrc.cols - 30;
	Mat Rows;
	Mat empty = imread("../data/empty.jpg", IMREAD_COLOR);
	Mat templ = empty(Rect(0, 0, empty.cols, TextHeight));
	INT Offset = TextSrc.rows;
	INT TitleHeight = 40;
	for (int i = 0; i < 4; i++)
	{
		Offset = TextSrc.rows;
		Mat temp_gray;
		Mat temp_binary;
		Mat temp_match;
		Mat temp = TextSrc(Rect(TextPaddingLeft, TextYOffset + i * TextHeight, Width, TextHeight));
		cvtColor(temp, temp_gray, CV_BGR2GRAY);
		threshold(temp_gray, temp_binary, 70 + i * 25, 255, CV_THRESH_BINARY);
		cvtColor(temp_binary, temp_match, CV_GRAY2BGR);
		Point Loc = GetMatchedStartPoint1(temp_match, templ, 0);
		if (Loc.x > 50)
		{
			Rows = temp_match(Rect(0, 0, Loc.x, TextHeight));


			CHAR name[50] = { 0 };
			sprintf_s(name,"../data/Temp/%d.jpg",GetMilliSecondOfDay());
			cv::String tname(name);
			imwrite(tname, Rows);
			CHAR new_name[250] = { 0 };
			string MD5Value = FileDigest(tname);
			sprintf_s(new_name, "../data/Split/%s.jpg", MD5Value.c_str());
			int r = rename(name, new_name);
			printf("MD5=%s rename=%d\n", MD5Value.c_str(),r);
		}
		printf("%d %d \n", Loc.x, Loc.y);
	}
}

VOID GetScreenCaptureWithIOR(LPSTR addr)
{
	RECT rect;
	rect.left = 127;
	rect.top = 540;
	rect.right = 127 + 676;
	rect.bottom = 540 + 68;
	ScreenCapture(addr, 32, &rect);
}

INT main(int argc, TCHAR * argv[]) {

	HWND hWnd = NULL;		// ���ھ��
	HANDLE hThread = NULL;	// ���߳̾��
	TCHAR sourceFilename[] = L"config.conf";
	g_config.LoadConfigFromFile(sourceFilename);
	_tprintf(L"Load config ...\n");

	MSG msg = { 0 };		// ��Ϣ
	DWORD dwThreadId = 0;	// �߳� ID
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

				int x = 0, y = 0, w = 0, h = 0;
				while (~scanf_s("%d %d %d %d", &x, &y, &w, &h))
				{
					if (x < 0 || y < 0 || w < 0 || h < 0)
						break;
					GetIOR(x, y ,w ,h);
				}
			}
			else if (m_HotKeyId5 == msg.wParam) {
				CHAR * addr = "../data/sc.bmp";
			//	CHAR * addr = "../data/mix.bmp";
				GetScreenCaptureWithIOR(addr);
				Mat Src = imread(addr, IMREAD_COLOR);

			//	Mat IOR = Src(Rect(127, 540, 676, 68));
				SplitFontImg(Src);
			}
			else if (m_HotKeyId6 == msg.wParam) {
				SplitFontImgTest_AutoIOR("../data/Screen.bmp");
			}
			else if (m_HotKeyId7 == msg.wParam) {

				Mat imag, result;
				imag = imread("../data/ior1.bmp", 0);	//������Ĳ�ɫͼ��ֱ���ԻҶ�ͼ�����
				namedWindow("ԭͼ", 1);
				imshow("ԭͼ", imag);
				result = imag.clone();
				//���ж�ֵ������ѡ��30��200.0Ϊ��ֵ
				threshold(imag, result, 50.0, 255, CV_THRESH_BINARY);
				namedWindow("��ֵ��ͼ��");
				imshow("��ֵ��ͼ��", result);
				imwrite("../data/threshold.jpg", result);

				Mat img = imread("../data/threshold.jpg", IMREAD_COLOR);
				Mat templ = imread("../data/empty.jpg", IMREAD_COLOR); 
				Point MinPoint = GetMatchedStartPoint(img, templ,0);
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