// WinGHotKey.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h"
#include "ElonaC.h"
//
#include <opencv2/opencv.hpp>  
#include<opencv2/highgui/highgui.hpp>  
#include<opencv2/imgproc/imgproc.hpp>  



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

static VOID ShowIOR_ItemsWithInput(Mat Src)
{
	int x = 0, y = 0, w = 0, h = 0;

	//  Mat Src = imread("../data/Src/shop.bmp");
	while (~scanf_s("%d %d %d %d", &x, &y, &w, &h))
	{
		if (x < 0 || y < 0 || w < 0 || h < 0)
			break;


		ShowIOR(Src, x, y, w, h);

		//  ShowIOR_Items(Src, x, y, w, h);
	}
}

static VOID DrawMatchLocation(Mat &display, Mat search, Point MinLoc, Scalar color = Scalar(0, 0, 255))
{
	rectangle(display, MinLoc, Point(MinLoc.x + search.cols, MinLoc.y + search.rows), color, 1, 8, 0);
}


static VOID Question0001()
{
	Mat Empty = imread("../data/IOR/IOR-backspace.bmp");
	Mat Juan = imread("../data/Quest/IOR-juan.bmp");
	DoThreshold(Juan, Juan, 150, 255);
	DoThreshold(Empty, Empty, 150, 255);
	//  imwrite("../data/Quest/IOR-juan2-DoThreshold.bmp", Juan);
	//  imwrite("../data/Quest/IOR-backspace-DoThreshold.bmp", Empty);
	Mat imageROI = imread("../data/Quest/IOR-298.bmp");
	DoThreshold(imageROI, imageROI, 150, 255);
	int a = imageROI.type();
	int b = Empty.type();
	printf("%d %d %d \n", imageROI.depth() == CV_8U, imageROI.depth() == CV_32F, imageROI.type() == Empty.type());
	// ((depth == CV_8U || depth == CV_32F) && type == _templ.type() && _img.dims() <= 2)
	//  imwrite("../data/Quest/IOR-298-DoThreshold.bmp", imageROI);

	int rate = 10;
	resize(Empty, Empty, Size(), rate, rate, INTER_NEAREST);
	resize(Juan, Juan, Size(), rate, rate, INTER_NEAREST);
	resize(imageROI, imageROI, Size(), rate, rate, INTER_NEAREST);

	Mat SCopy = imageROI.clone();

	imshow("SCopy", SCopy);
	imshow("Empty22", Empty);
	imshow("Juan22", Juan);


	Point MinLoc = GetMatchedStartPointOnly(imageROI, Empty, 0);
	Point MinLoc2 = GetMatchedStartPointOnly(imageROI, Juan, 0);
	printf(" MinLo0c = (%d, %d)", MinLoc.x, MinLoc.y);

	if (MinLoc2.x < 20)
	{
		DrawMatchLocation(imageROI, Juan, MinLoc2, Scalar(0, 255, 0));
		printf("\tMinLoc2 = (%d, %d)", MinLoc2.x, MinLoc2.y);
	}
	printf("\n");

	imshow("QUEST", imageROI(Rect(0, 0, MinLoc.x + 3, imageROI.rows)));
	waitKey();
}

static DWORD WINAPI MyThreadFunction_GetMatchedStartPointOnly(LPVOID lpParam)
{
	Mat image = imread("../data/Src/shop.bmp");
	// First Item TopLeft in ScreenCapture 170 165 542 19
	Rect TopLeftRect(170, 165, 542, 19);

	DoThreshold(image, image, 150, 255);

	namedWindow("SRC");
	moveWindow("SRC", 600, 60);
	imshow("SRC", image);

	Mat Empty = imread("../data/IOR/IOR-backspace.bmp");
	Mat Bottle = imread("../data/IOR/IOR-bottle.bmp");
	Mat Juan = imread("../data/IOR/IOR-juan.bmp");
	DoThreshold(Empty, Empty, 150, 255);
	DoThreshold(Bottle, Bottle, 150, 255);
	DoThreshold(Juan, Juan, 150, 255);
	int count = 0;
	for (int i = TopLeftRect.y; i < image.rows - TopLeftRect.height; i += TopLeftRect.height)
	{
		Mat imageROI = image(Rect(TopLeftRect.x, i, TopLeftRect.width, TopLeftRect.height));
		CHAR name[50] = { 0 };
		sprintf_s(name, "IOR-%d", i);
		namedWindow(name);
		moveWindow(name, 0, count * 60);

		Point MinLoc = GetMatchedStartPointOnly(imageROI, Empty, 0);

		Point MinLoc1 = GetMatchedStartPointOnly(imageROI, Bottle, 0);
		Point MinLoc2 = GetMatchedStartPointOnly(imageROI, Juan, 0);
		printf("[%d] MinLo0c = (%d, %d)", count, MinLoc.x, MinLoc.y);

		if (MinLoc1.x < 20)
		{
			DrawMatchLocation(imageROI, Bottle, MinLoc1, Scalar(0, 0, 255));
			printf("\tMinLoc1 = (%d, %d)", MinLoc1.x, MinLoc1.y);
		}
		if (MinLoc2.x < 20)
		{
			DrawMatchLocation(imageROI, Juan, MinLoc2, Scalar(0, 255, 0));
			printf("\tMinLoc2 = (%d, %d)", MinLoc2.x, MinLoc2.y);
		}
		printf("\n");

		//  rectangle(imageROI, MinLoc, Point(MinLoc.x + Empty.cols, MinLoc.y + Empty.rows), Scalar(0, 0, 255), 2, 8, 0);



		imshow(name, imageROI(Rect(0, 0, MinLoc.x + 3, imageROI.rows)));

		count++;
		if (count > 15)
			break;
	}
	waitKey();

	return 0;
}



static DWORD WINAPI MyThreadFunction(LPVOID lpParam)
{
	Mat image = imread("../data/Src/shop.bmp", IMREAD_GRAYSCALE);
	// First Item TopLeft in ScreenCapture 170 165 542 19
	Rect TopLeftRect(170, 165, 542, 19);


	namedWindow("SRC");
	moveWindow("SRC", 600, 60);
	imshow("SRC", image);



	Mat Empty = imread("../data/IOR/IOR-backspace.bmp", IMREAD_GRAYSCALE);
	Mat Bottle = imread("../data/IOR/IOR-bottle.bmp", IMREAD_GRAYSCALE);
	Mat Juan = imread("../data/IOR/IOR-juan.bmp", IMREAD_GRAYSCALE);



	// IMREAD_GRAYSCALE

	int count = 0;
	for (int i = TopLeftRect.y; i < image.rows - TopLeftRect.height; i += TopLeftRect.height)
	{
		Mat imageROI = image(Rect(TopLeftRect.x, i, TopLeftRect.width, TopLeftRect.height));
		CHAR name[50] = { 0 };
		//  sprintf_s(name, "../data/Split/IOR-%d.bmp", i);
		sprintf_s(name, "IOR-%d", i);
		//    imwrite(name, imageROI);

		namedWindow(name);
		moveWindow(name, 0, count * 60);

		//  Point MinLoc = GetMatchedStartPointOnly(imageROI, Empty, 0);

		//Point MinLoc1 = GetMatchedStartPointOnly(imageROI, Bottle, 0);
		//Point MinLoc2 = GetMatchedStartPointOnly(imageROI, Juan, 0);
		//  printf("[%d] MinLo0c = (%d, %d)", count, MinLoc.x, MinLoc.y);

		Point2f StartPoint_Juan, StartPoint_Bottle;
		if (SURFDetect(Juan, imageROI, StartPoint_Juan) > 0)
		{
			printf("\t [%d] Juan = (%.3f, %.3f)", i, StartPoint_Juan.x, StartPoint_Juan.y);
			DrawMatchLocation(imageROI, Juan, StartPoint_Juan, Scalar(0, 0, 255));
		}
		if (SURFDetect(Bottle, imageROI, StartPoint_Bottle) > 0)
		{
			printf("\t [%d] Bottle = (%.3f, %.3f)", i, StartPoint_Bottle.x, StartPoint_Bottle.y);
			DrawMatchLocation(imageROI, Bottle, StartPoint_Bottle, Scalar(0, 0, 255));
		}
		printf("\n");
		imshow(name, imageROI);

		//  imshow(name, imageROI(Rect(0, 0, MinLoc.x + 3, imageROI.rows)));

		count++;
		if (count > 15)
			break;
	}
	waitKey();

	return 0;
}

static INT sfsddsf()
{
	Mat input_image = (Mat_<uchar>(8, 8) <<
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 255, 255, 255, 0, 0, 0, 255,
		0, 255, 255, 255, 0, 0, 0, 0,
		0, 255, 255, 255, 0, 255, 0, 0,
		0, 0, 255, 0, 0, 0, 0, 0,
		0, 0, 255, 0, 0, 255, 255, 0,
		0, 255, 0, 255, 0, 0, 255, 0,
		0, 255, 255, 255, 0, 0, 0, 0);
	Mat kernel = (Mat_<int>(3, 3) <<
		0, 1, 0,
		1, -1, 1,
		0, 1, 0);
	Mat output_image;
	morphologyEx(input_image, output_image, MORPH_HITMISS, kernel);
	const int rate = 50;
	kernel = (kernel + 1) * 127;
	kernel.convertTo(kernel, CV_8U);
	resize(kernel, kernel, Size(), rate, rate, INTER_NEAREST);
	imshow("kernel", kernel);
	moveWindow("kernel", 0, 0);
	resize(input_image, input_image, Size(), rate, rate, INTER_NEAREST);
	imshow("Original", input_image);
	moveWindow("Original", 0, 200);
	resize(output_image, output_image, Size(), rate, rate, INTER_NEAREST);
	imshow("Hit or Miss", output_image);
	moveWindow("Hit or Miss", 500, 200);
	waitKey(0);
	return 0;
}


VOID FindAndCloseWindow()
{
	LPCWSTR name = L"IOR-165";
	HWND hd = FindWindow(NULL, name);
	printf("hd = %d\n", hd);
	PostMessage(hd, WM_CLOSE, NULL, NULL);
}



int TEST_SURF()
{
	//Mat img_object = imread(parser.get<String>("input1"), IMREAD_GRAYSCALE);
	//Mat img_scene = imread(parser.get<String>("input2"), IMREAD_GRAYSCALE);

	Mat img_object = imread("../data/Quest/IOR-juan2.bmp", IMREAD_GRAYSCALE);
	//Mat img_scene = imread("../data/Quest/IOR-298.bmp", IMREAD_GRAYSCALE);
	// 279
	//  Mat img_scene = imread("../data/IOR/IOR-279.bmp", IMREAD_GRAYSCALE);
	// 后面
	Mat img_scene = imread("../data/Split/IOR-412.bmp", IMREAD_GRAYSCALE);

	Point2f StartPoint;
	if (SURFDetect(img_object, img_scene(Rect(0, 0, img_scene.cols / 10, img_scene.rows)), StartPoint))
	{
		rectangle(
			img_scene,
			Point2f(StartPoint.x, StartPoint.y),
			Point2f(StartPoint.x + img_object.cols, StartPoint.y + img_object.rows),
			Scalar(0, 255, 255),
			1,
			LINE_8
		);
		imshow("img_scene", img_scene);
		waitKey();
	}
	return 0;

}

INT main(int argc, TCHAR * argv[]) {


	// TEST_SURF();


	HWND hWnd = NULL;   // 窗口句柄
	HANDLE hThread = NULL;  // 多线程句柄
	TCHAR sourceFilename[] = L"config.conf";
	g_config.LoadConfigFromFile(sourceFilename);
	_tprintf(L"Load config ...\n");

	MSG msg = { 0 };    // 消息
	DWORD dwThreadId = 0; // 线程 ID
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
	LocalRegisterHotKey(hWnd, m_HotKeyId1, MOD_NOREPEAT, VK_OEM_MINUS);
	LocalRegisterHotKey(hWnd, m_HotKeyId2, MOD_NOREPEAT, VK_OEM_PLUS);
	LocalRegisterHotKey(hWnd, m_HotKeyId3, MOD_NOREPEAT, VK_NUMPAD3);
	//  LocalRegisterHotKey(hWnd, m_HotKeyId4, MOD_NOREPEAT, VK_NUMPAD4);
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

	//  Mat Empty(6,19, CV_8UC1,Scalar::all(0));


	while (GetMessage(&msg, NULL, 0, 0) != 0) {
		if (msg.message == WM_HOTKEY) {
			GetHMS(g_CunnentTime);

			if (m_HotKeyId1 == msg.wParam) {

				for (int i = 0; i < 100; i++)
				{
					ConvertChar2KeyWordAndSimulate("f");
					Sleep(40);
				}


				//    //  Sleep(100);
				//      ConvertChar2KeyWordAndSimulate("S");
				//      Sleep(100);
				//      ConvertChar2KeyWordAndSimulate("a");
				//      Sleep(100);
				//      WORD  Keys[] = { 
				////        VK_ESCAPE ,
				//  //      VK_LEFT,
				//        VK_NUMPAD4
				//      };
				//      SimulateKeyArrayInput(Keys, 1);
			}
			else if (m_HotKeyId2 == msg.wParam) {

				RunWishing();
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

				// Question0001();

				//CHAR name[50] = { 0 };
				//sprintf_s(name, "../data/ScreenShot/%lld.jpg", GetTimestamp());
				/*    CHAR * name = "../data/mix.bmp";
				GetScreenCapture_LogArea(name);
				SplitFontImg_AutoIOR(name);*/
			}
			else if (m_HotKeyId6 == msg.wParam) {

				/*  GetMatchedStartPointWithFName("../data/Src/backpack.bmp", "../data/Src/de.bmp", 0, FALSE);
				GetMatchedStartPointWithFName("../data/Src/backpack.bmp", "../data/Src/de1.bmp", 0, FALSE);
				*/
				SplitFontImgTest_AutoIOR("../data/Src/xuyuan/benzhoule.bmp");
			}
			else if (m_HotKeyId7 == msg.wParam) {
				Mat Src = imread("../data/Src/Enter/select.bmp", IMREAD_COLOR);
				//Main 100 160 200 60
				//Select 180 160 250 60
				ShowIOR_ItemsWithInput(Src);


				//Mat imag, result;
				//imag = imread("../data/ior1.bmp", 0); //将读入的彩色图像直接以灰度图像读入
				//namedWindow("原图", 1);
				//imshow("原图", imag);
				//result = imag.clone();
				////进行二值化处理，选择30，200.0为阈值
				//threshold(imag, result, 50.0, 255, 0);
				//namedWindow("二值化图像");
				//imshow("二值化图像", result);
				//imwrite("../data/threshold.jpg", result);

				//Mat img = imread("../data/threshold.jpg", IMREAD_COLOR);
				//Mat templ = imread("../data/empty.jpg", IMREAD_COLOR);
				//Point MinPoint = GetMatchedStartPoint(img, templ, 0);
				//printf("matchLoc = (%d,%d)\n", MinPoint.x, MinPoint.y);
				//waitKey(0);
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