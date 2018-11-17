// WinGHotKey.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h"
#include "ElonaC.h"
//
#include "opencv2/core.hpp"
#include <opencv2/opencv.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>  
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"

#include <iostream>  
#include <io.h>

using namespace std;
using namespace cv;
using namespace cv::xfeatures2d;

USING_DEBUG_LOG_VARS


VOID SimulateClickUp()
{
	SimulateMouseClick(941, 917);
}
VOID SimulateClickLeft()
{
	SimulateMouseClick(869, 983);
}
VOID SimulateClickDown()
{
	SimulateMouseClick(941, 983);
}
VOID SimulateClickRight()
{
	SimulateMouseClick(1012, 983);
}

VOID FireSoftKeyBoard_CN()
{
	DEBUG_LOG("Fire SoftKeyboard \n");
	SimulateMouseClick(1716, 1059);
	Sleep(500);
}

VOID HoldUp_ForTime()
{
	DEBUG_LOG("Hold Up Key For 20 s\n");
	SimulateMouseClick_Hold(941, 917);
	Sleep(20000);
	LeftClick_Release();
}

VOID SmOpenAndClick()
{
	static bool SKB_Fired = false;
	if (!SKB_Fired)
	{
		FireSoftKeyBoard_CN();
		SKB_Fired = true;
		// Detect Soft Keyboard
	}
	HoldUp_ForTime();
}

VOID CutLeftStateBar()
{
	CHAR name[250] = { 0 };
	string fname;
	Gfname("../data/Temp/LSB", "SCRE-", ".bmp", fname);
	strcpy_s(name, fname.c_str());
	// GetScreenCapture_LeftStateArea(name);
	GetScreenCapture_GameArea(name);
}

// 判断空腹
BOOL DetectKongfu(Mat img_scene)
{
	static Mat img_object = imread("../data/Src/LSB/kongfu.bmp", IMREAD_GRAYSCALE);
	Point2f StartPoint;
	if (SURFDetect(img_object, img_scene, StartPoint,30) > 0)
	{
		// ShowSURFDetectImage(img_scene, img_object, StartPoint);
		return TRUE;
	}
	return FALSE;
}


// 判断可睡眠
BOOL DetectKeshuimian(Mat img_scene)
{
	static Mat img_object = imread("../data/Src/LSB/keshuimian.bmp", IMREAD_GRAYSCALE);
	Point2f StartPoint;
	if (SURFDetect(img_object, img_scene, StartPoint,38) > 0)
	{
		// ShowSURFDetectImage(img_scene, img_object, StartPoint);
		return TRUE;
	}
	return FALSE;
}

static VOID TestDetect();


VOID SimulateEat()
{
	// Detect Empty
	DEBUG_LOG("SimulateEat\n");
	ConvertChar2KeyWordAndSimulate("e");
	Sleep(100);
	ConvertChar2KeyWordAndSimulate("a");
	Sleep(200);
}


void SMKey(WORD Key ,INT Interval = 160)
{
	INPUT    Input = { 0 };
	// left down 
	Input.ki.wVk = Key;
	Input.type = INPUT_KEYBOARD;
	::SendInput(1, &Input, sizeof(INPUT));

	Sleep(Interval);
	// left up
	::ZeroMemory(&Input, sizeof(INPUT));
	Input.ki.wVk = Key;
	Input.type = INPUT_KEYBOARD;
	Input.mi.dwFlags = KEYEVENTF_KEYUP;
	::SendInput(1, &Input, sizeof(INPUT));
}


VOID SimulateSleep()
{
	// Detect Empty
	DEBUG_LOG("SimulateSleep\n");
	ConvertChar2KeyWordAndSimulate("t");
	Sleep(100);
	ConvertChar2KeyWordAndSimulate("a");
	Sleep(200);
	WORD Keys[] = {VK_RETURN};
	SimulateKeyArrayInput(Keys, 1);

	//WORD Keys[] = { VK_RETURN };
	//SimulateKeyArrayInput(Keys, 1);
}




VOID RunTrain()
{
	//SMKey(VK_RETURN);
	//return;
	CutLeftStateBar();
	return;
	SimulateEat();
	return;
	CHAR name[250] = { 0 };
	string fname;

STARTR:
	SmOpenAndClick();
	DEBUG_LOG("You Can Stop In 4 Seconds\n");
	Sleep(4000);

	Gfname("../data/Temp/LSB", "LSB-", ".bmp", fname);
	strcpy_s(name, fname.c_str());
	GetScreenCapture_LeftStateArea(name);
	Mat ste = imread(name,IMREAD_COLOR);

	if (DetectKongfu(ste) == TRUE )
	{
		DEBUG_LOGN("Hungry Detected");
		SimulateEat();
	}

	//if (DetectKeshuimian(ste) == TRUE)
	//{

	//}

	goto STARTR;
	return;
	LPPOINT cPoint = new POINT;
	::GetCursorPos(cPoint);

	//	::SetCursorPos(cPoint);
	//	mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
	DEBUG_LOG("cPoint = (%d, %d)\n", cPoint->x, cPoint->y);
	//	SimulateClickUp();

	Sleep(3000);
	//Mat Src = imread("../data/Src/SKB/kb2.bmp");
	//Rect fr(908,887,66,60);
	//imshow("Src1", Src(fr));
	//imshow("Src2", Src(Rect(836, 953, 66, 60)));
	//waitKey(0);
}



static VOID TestDetect()
{
	// zhenghe
	Mat ste1 = imread("../data/Src/LSB/STE/SCRE-43500659.bmp", IMREAD_COLOR);
	// zhonghe kongfu
	Mat ste2 = imread("../data/Src/LSB/STE/SCRE-43578612.bmp", IMREAD_COLOR);
	// zhonghe red kongfu
	Mat ste3 = imread("../data/Src/LSB/STE/SCRE-43604860.bmp", IMREAD_COLOR);
	// zhonghe keshuimian manfu
	Mat ste4 = imread("../data/Src/LSB/STE/SCRE-45879417.bmp", IMREAD_COLOR);
	// zhonghe keshuimian kongfu
	Mat ste5 = imread("../data/Src/LSB/STE/SCRE-46028138.bmp", IMREAD_COLOR);
	// SCRE-43578612 SCRE-43604860 SCRE-45879417 SCRE-46028138

	if (DetectKongfu(ste1) == TRUE || DetectKeshuimian(ste1) == TRUE)
	{
		DEBUG_LOG("ERROR 1");
	}

	if (DetectKongfu(ste2) != TRUE || DetectKeshuimian(ste2) == TRUE)
	{
		DEBUG_LOG("ERROR 2");
	}

	if (DetectKongfu(ste3) != TRUE || DetectKeshuimian(ste3) == TRUE)
	{
		DEBUG_LOG("ERROR 3");
	}

	if (DetectKongfu(ste4) == TRUE || DetectKeshuimian(ste4) != TRUE)
	{
		DEBUG_LOG("ERROR 4");
	}

	if (DetectKongfu(ste5) != TRUE || DetectKeshuimian(ste5) != TRUE)
	{
		DEBUG_LOG("ERROR 5");
	}

}


