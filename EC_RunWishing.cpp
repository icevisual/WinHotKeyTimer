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

BOOL GetRiskerGuideArea(Mat Src, Mat &Output)
{
	Rect LogRect(152, 222, 180, 36);
	return GetIORArea(Src, Output, LogRect);
}

BOOL GetSelectRiskerArea(Mat Src, Mat &Output)
{
	// 275 230 250 40
	Rect LogRect(275, 230, 250, 40);
	return GetIORArea(Src, Output, LogRect);
}
// 165 680 847 85



// 使用 inRange 筛选 图片中的 EC 红色部分
VOID filter_ec_red(Mat src, Mat &output)
{
	// Convert from BGR to HSV colorspace
	cvtColor(src, output, COLOR_BGR2HSV);
	// Detect the object based on HSV Range Values
	//	inRange(output, Scalar(0, 28, 201), Scalar(180, 80, 224), output);
	inRange(output, Scalar(0, 28, 96), Scalar(8, 255, 255), output);
}
// 使用 inRange 筛选 图片中的 EC 绿色部分
VOID filter_ec_green(Mat src, Mat &output)
{
	// Convert from BGR to HSV colorspace
	cvtColor(src, output, COLOR_BGR2HSV);
	// Detect the object based on HSV Range Values
	inRange(output, Scalar(33, 18, 201), Scalar(77, 255, 255), output);
}

// 判断图片中是否包含 EC 红色 点
BOOL JudgeRedPoint(Mat Src)
{
	Mat Temp;
	filter_ec_red(Src, Temp);
	return JudgeHasWhitePoint(Temp);
}
// 判断图片中是否包含 EC 绿色 点
BOOL JudgeGreenPoint(Mat Src)
{
	Mat Temp;
	filter_ec_green(Src, Temp);
	return JudgeHasWhitePoint(Temp);
}



// 判断干涸
BOOL DetectGanhele(Mat img_scene)
{
	static Mat img_object = imread("../data/Src/ganhele.jpg", IMREAD_GRAYSCALE);
	Point2f StartPoint;
	if (SURFDetect(img_object, img_scene, StartPoint) > 0)
	{
		/*	rectangle(
		img_scene,
		Point2f(StartPoint.x, StartPoint.y),
		Point2f(StartPoint.x + img_object.cols, StartPoint.y + img_object.rows),
		Scalar(0, 255, 255),
		1,
		LINE_8
		);
		imshow("img_scene", img_scene);
		waitKey();*/
		return TRUE;
	}
	return FALSE;
}
BOOL DetectWhatYouWant(Mat img_scene)
{
	static Mat img_object = imread("../data/Src/whatyouwant.bmp", IMREAD_GRAYSCALE);
	Point2f StartPoint;
	if (SURFDetect(img_object, img_scene, StartPoint, 100) > 0)
	{
		return TRUE;
	}
	return FALSE;
}

BOOL DetectRiskerGuidePoint(Mat img_scene)
{
	static Mat img_object = imread("../data/Src/guidepoint.bmp", IMREAD_GRAYSCALE);
	Point2f StartPoint;
	if (SURFDetect(img_object, img_scene, StartPoint) > 0)
	{
		//	printf("StartPoint (%.2f, %.2f)\n", StartPoint.x,StartPoint.y);
		return TRUE;
	}
	return FALSE;
}

BOOL DetectSelectRisker(Mat img_scene)
{
	static Mat img_object = imread("../data/Src/select_risker.bmp", IMREAD_GRAYSCALE);
	Point2f StartPoint;
	if (SURFDetect(img_object, img_scene, StartPoint) > 0)
	{
		return TRUE;
	}
	return FALSE;
}




static DWORD WINAPI ResetRestartThreadFunction(LPVOID lpParam)
{
	system("D:\\desktop\\rr.sh");
	return 0;
}

VOID SaveEC()
{
	DEBUG_LOG("Save EC\n");
	system("D:\\desktop\\sv.sh");
}

VOID StartEC()
{
	DEBUG_LOG("Start EC\n");
	HANDLE  hThreadArray;
	DWORD   dwThreadIdArray;
	hThreadArray = CreateThread(
		NULL,                   // default security attributes
		0,                      // use default stack size  
		ResetRestartThreadFunction,       // thread function name
		NULL,          // argument to thread function 
		0,                      // use default creation flags 
		&dwThreadIdArray);   // returns the thread identifier 
}


VOID SimulateDrink()
{
	DEBUG_LOG("SimulateDrink\n");
	ConvertChar2KeyWordAndSimulate("q");
	Sleep(100);
	ConvertChar2KeyWordAndSimulate("a");
	Sleep(100);
}


VOID SimulateQuiteGame()
{
	GetScreenCapture_GameArea("../data/Temp/Before-SimulateQuiteGame.bmp");
	Sleep(500);
	GetScreenCapture_GameArea("../data/Temp/Mid-SimulateQuiteGame.bmp");
	DEBUG_LOG("SimulateQuiteGame\n");
	ConvertChar2KeyWordAndSimulate("S");
	Sleep(200);
	ConvertChar2KeyWordAndSimulate("a");
	Sleep(400);
	WORD Keys[] = { VK_RETURN };
	SimulateKeyArrayInput(Keys, 1);
	GetScreenCapture_GameArea("../data/Temp/After-SimulateQuiteGame.bmp");
}

VOID SimulateEnterGame()
{
	DEBUG_LOG("SimulateEnterGame\n");
	ConvertChar2KeyWordAndSimulate("a");
	Sleep(200);
	ConvertChar2KeyWordAndSimulate("a");
	Sleep(1500);
}

VOID SimulateRead(string itemIndex)
{
	DEBUG_LOG("SimulateRead\n");
	ConvertChar2KeyWordAndSimulate("r");
	Sleep(200);
	ConvertChar2KeyWordAndSimulate(itemIndex);
	Sleep(200);
}


#include <fstream>


VOID RunRead()
{
	Mat DetectArea;
	Mat Screen;
	BOOL DetectRet;
	CHAR name[250] = { 0 };
	CHAR storage[100] = { 0 };
	string fname;
	wstring KeyWords[] = {L"陨石",L"丰收",L"许愿" ,L"许" };
	int Score = 0;
	int MaxRetryTime = 10;
RESTERT:
	if (MaxRetryTime < 0)
		return;
	int index = 0;
	StartEC();

	Sleep(1500);
	int BatchNumber = GetMilliSecondOfDay();
	sprintf_s(storage, "../data/Temp/%d", BatchNumber);


	MakeDIR(storage);

	do {
		// 1500
		Sleep(300);
		Gfname(storage, "SCRE-", ".bmp", fname);
		strcpy_s(name, fname.c_str());


		GetScreenCapture_GameArea(name);
		Screen = imread(name, IMREAD_COLOR);

		GetRiskerGuideArea(Screen, DetectArea);
		
		DetectRet = DetectRiskerGuidePoint(DetectArea);
		DEBUG_LOG("RiskerGuide = %d\n", DetectRet);
	} while (DetectRet == FALSE);

	SimulateEnterGame();

	do {
		// 1500
		Sleep(500);
		Gfname(storage, "SCRE-", ".bmp", fname);
		strcpy_s(name, fname.c_str());
		GetScreenCapture_GameArea(name);
		Screen = imread(name, IMREAD_COLOR);

		GetSelectRiskerArea(Screen, DetectArea);
		DetectRet = DetectRiskerGuidePoint(DetectArea);
		DEBUG_LOG("SelectRisker = %d\n", DetectRet);
	} while (DetectRet == TRUE);

	DEBUG_LOG("Enter Game\n");
	int max_read = 4;
	wstring result_text;
	bool get_magic = false;
	for (int i = 0; i < max_read; i++)
	{
		SimulateRead("e");
		Gfname(storage, "SCRE-", ".bmp", fname);
		strcpy_s(name, fname.c_str());
		GetScreenCapture_GameArea(name);
		Mat  Screen1 = imread(name, IMREAD_COLOR);
		Mat DetectArea1;
		GetLogArea(Screen1, DetectArea1);
		imwrite("../data/Temp/DetectArea2.bmp",DetectArea1);
		system("tesseract ../data/Temp/DetectArea2.bmp ../data/Temp/result -l chi_sim");
		file_get_content_utf8(L"../data/Temp/result.txt", result_text);
		wcout.imbue(locale("chs"));
		wcout << endl << result_text << endl << endl;;
		for (int k = 0; k < 3; k++)
		{
			get_magic = ws_contains_utf8(result_text, KeyWords[k]);
			if (get_magic)
			{
				break;
			}
			else
			{
				DEBUG_LOG("Not Match\n");
			}
		}

		if (get_magic)
			break;
		else
		{
			DEBUG_LOG("Not Match\n");
		}
	}
	SimulateQuiteGame();
	if (get_magic)
	{
		SaveEC();
		DEBUG_LOG(" Match\n");
		
		MaxRetryTime--;
	}
	goto RESTERT;
}



VOID RunWishing()
{
	Mat DetectArea;
	Mat Screen;
	BOOL DetectRet;
	CHAR name[250] = { 0 };
	CHAR storage[100] = { 0 };
	string fname;

	int Score = 0;
RESTERT:

	int index = 0;
	StartEC();

	Sleep(1500);
	int BatchNumber = GetMilliSecondOfDay();
	sprintf_s(storage, "../data/Temp/%d", BatchNumber);


	MakeDIR(storage);

	do {
		// 1500
		Sleep(300);
		Gfname(storage, "SCRE-", ".bmp", fname);
		strcpy_s(name, fname.c_str());


		GetScreenCapture_GameArea(name);
		Screen = imread(name, IMREAD_COLOR);

		GetRiskerGuideArea(Screen, DetectArea);
		imwrite("../data/Temp/DetectArea.bmp",DetectArea);
		DetectRet = DetectRiskerGuidePoint(DetectArea);
		DEBUG_LOG("RiskerGuide = %d\n", DetectRet);
	} while (DetectRet == FALSE);

	SimulateEnterGame();

	do {
		// 1500
		Sleep(500);
		Gfname(storage, "SCRE-", ".bmp", fname);
		strcpy_s(name, fname.c_str());
		GetScreenCapture_GameArea(name);
		Screen = imread(name, IMREAD_COLOR);

		GetSelectRiskerArea(Screen, DetectArea);
		DetectRet = DetectRiskerGuidePoint(DetectArea);
		DEBUG_LOG("SelectRisker = %d\n", DetectRet);
	} while (DetectRet == TRUE);

	DEBUG_LOG("Enter Game\n");

	do {

		SimulateDrink();
		index++;
		Gfname(storage, "LOGAREA-", ".bmp", fname);
		strcpy_s(name, fname.c_str());
		//	GetScreenCapture_LogArea(name);
		GetScreenCaptureWithIOR(name, Rect(127, 570, 676, 38));
		Screen = imread(name, IMREAD_COLOR);

		DEBUG_LOG("Detect Color From File: %s \r\n", name);
		if (JudgeRedPoint(Screen))
		{
			DEBUG_LOG(">>> RED Detected\n");
			SimulateQuiteGame();
			goto RESTERT;
			// Restart
		}
		else if (JudgeGreenPoint(Screen))
		{
			Score += 1;
			DEBUG_LOG(">>> GREEN Detected\n");
			SimulateQuiteGame();
			SaveEC();
			DEBUG_LOG("DetectGanhele\n");
			if (DetectGanhele(Screen))
			{
				DEBUG_LOG(">>> YES,Score = %d\n", Score);
				break;
			}
			goto RESTERT;
			// Save
		}
		else
		{
			DEBUG_LOG(">>> Not RED or GREEN\n");
			DEBUG_LOG("DetectGanhele\n");
			// ../data/Temp/LOGAREA-64498098.bmp
			if (DetectGanhele(Screen))
			{

				DEBUG_LOG(">>> YES,Score = %d\n", Score);
				if (index == 1)
				{
					DEBUG_LOG("Ganhele Detected At First Drink\n");
					break;
				}
				SimulateQuiteGame();
				goto RESTERT;
			}
			else
			{
				DEBUG_LOG(">>> NO\n");
			}
			DEBUG_LOG("DetectWhatYouWant\n");
			if (DetectWhatYouWant(Screen))
			{
				DEBUG_LOG(">>> YES\n");
				break;
			}
			else
			{
				DEBUG_LOG(">>> NO\n");
			}
		}
	} while (true);

	DEBUG_LOG("Simulate Stopped\n");
	// 打开 EC 程序
	// 运行程序 [启动中,小框][全黑，全框][冒险的路标][选择冒险者]
	// 判断进入首页
	// 模拟 a
	// 判断进入资料选择页面
	// 模拟 a
	// 判断 不在资料选择页面
	// 截屏 获取输出栏
	// 判断 红色：重启，绿色：保存重启
	// 判断 干涸：重启，冲出来：重启
	// 判断 许愿

}






/////////// Test





// 将目录下截屏图片。剪切出 日志栏 保存
VOID ScanXuyuanFolder_GetLogArea()
{
	vector<String> ResultVector;

	if (ListFilesWithExt_NDP("../data/Src/xuyuan", ResultVector, ".bmp"))
	{
		for (size_t i = 0; i < ResultVector.size(); i++)
		{
			cout << ResultVector[i] << endl;

			Mat Src = imread(ResultVector[i], IMREAD_COLOR);
			Mat IOR;
			if (Src.rows > 100)
				IOR = Src(Rect(127, 540, 676, 68));
			else
				IOR = Src;
			RenameMatWithMD5(IOR, "../data/Split/ConsoleArea");
		}
	}
}

//  通过文件夹内的图片，测试 filter_ec_red | green 
VOID TestFolderImages()
{
	vector<String> ResultVector;
	if (ListFilesWithExt_NDP("../data/Split/ConsoleArea/Red", ResultVector, ".jpg"))
	{
		for (size_t i = 0; i < ResultVector.size(); i++)
		{
			Mat srcMerge = imread(ResultVector[i], IMREAD_COLOR);
			Mat red;
			filter_ec_red(srcMerge, red);
			if (JudgeHasWhitePoint(red) != TRUE)
			{
				imshow("RED-" + ResultVector[i], srcMerge);
			}
		}
	}
	if (ListFilesWithExt_NDP("../data/Split/ConsoleArea/Green", ResultVector, ".jpg"))
	{
		for (size_t i = 0; i < ResultVector.size(); i++)
		{
			Mat srcMerge = imread(ResultVector[i], IMREAD_COLOR);
			Mat red;
			filter_ec_green(srcMerge, red);
			if (JudgeHasWhitePoint(red) != TRUE)
			{
				imshow("GRN-" + ResultVector[i], srcMerge);
			}
		}
	}
	if (ListFilesWithExt_NDP("../data/Split/ConsoleArea/Other", ResultVector, ".jpg"))
	{
		for (size_t i = 0; i < ResultVector.size(); i++)
		{
			Mat srcMerge = imread(ResultVector[i], IMREAD_COLOR);
			Mat red;
			filter_ec_red(srcMerge, red);
			if (JudgeHasWhitePoint(red) == TRUE)
			{
				imshow("OTR-" + ResultVector[i], srcMerge);
			}
			filter_ec_green(srcMerge, red);
			if (JudgeHasWhitePoint(red) == TRUE)
			{
				imshow("OTR-" + ResultVector[i], srcMerge);
			}
		}
	}
	waitKey();
}

VOID TestDetect()
{
	Mat WhatYouWant = imread("../data/Src/wyw.bmp", IMREAD_COLOR);
	Mat RiskerGuidePoint = imread("../data/Src/Enter/main.bmp", IMREAD_COLOR);
	Mat SelectRisker = imread("../data/Src/Enter/select.bmp", IMREAD_COLOR);
	//Main 100 160 200 60
	//Select 180 160 250 60
	Mat LogArea;
	GetLogArea(WhatYouWant, LogArea);
	if (DetectWhatYouWant(LogArea) == FALSE)
	{
		printf("WhatYouWant Error");
	}
	if (DetectRiskerGuidePoint(RiskerGuidePoint(Rect(100, 160, 200, 60))) == FALSE)
	{
		printf("RiskerGuidePoint Error");
	}
	if (DetectSelectRisker(SelectRisker(Rect(180, 160, 250, 60))) == FALSE)
	{
		printf("SelectRisker Error");
	}

}

