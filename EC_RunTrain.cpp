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

void LeftClick()
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

void LeftClick_Hold()
{
	INPUT    Input = { 0 };
	// left down 
	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
	::SendInput(1, &Input, sizeof(INPUT));
}
void LeftClick_Release()
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
	}
	HoldUp_ForTime();
}

VOID CutLeftStateBar()
{
	CHAR name[250] = { 0 };
	string fname;
	Gfname("../data/Temp/LSB", "SCRE-", ".bmp", fname);
	strcpy_s(name, fname.c_str());
	GetScreenCaptureWithIOR(name, Rect(0, 400, 70, 125));

	//GetScreenCaptureWithIOR(name, Rect(0, 0, 805, 628));
}

VOID RunTrain()
{
STARTR:
	SmOpenAndClick();
	DEBUG_LOG("You Can Stop In 4 Seconds\n");
	Sleep(4000);
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
