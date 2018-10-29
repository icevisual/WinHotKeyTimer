#pragma once

#include "stdafx.h"
#include <opencv2/opencv.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>  

#include <iostream>  

#include <windows.h>
#include <Mmsystem.h>
#pragma comment(lib,"winmm.lib")
#include <map> 

using namespace std;
using namespace cv;

// int SURFDetect(Mat img_object, Mat img_scene, Point2f &StartPoint, int rate = 5);


VOID ShowIOR(Mat image, int x, int y, int w, int h);

VOID ShowIOR_Items(Mat image, int x, int y, int w, int h);

VOID DoThreshold(Mat Src, Mat &out, double thresh, double maxval);

// Image Match / input Filename/ use_threshold / show_img
// Method: \n 0: SQDIFF \n 1: SQDIFF NORMED \n 2: TM CCORR \n 3: TM CCORR NORMED \n 4: TM COEFF \n 5: TM COEFF NORMED
Point GetMatchedStartPointWithFName(
	string img_name, string templ_name,
	int match_method, BOOL use_threshold = TRUE, BOOL show_img = TRUE);

// Image Match / input Mat / show_img
// Method: \n 0: SQDIFF \n 1: SQDIFF NORMED \n 2: TM CCORR \n 3: TM CCORR NORMED \n 4: TM COEFF \n 5: TM COEFF NORMED
Point GetMatchedStartPoint(Mat img, Mat templ, int match_method, BOOL show_img = FALSE);

// Image Match / input Mat / return Min Point
// Method: \n 0: SQDIFF \n 1: SQDIFF NORMED \n 2: TM CCORR \n 3: TM CCORR NORMED \n 4: TM COEFF \n 5: TM COEFF NORMED
Point GetMatchedStartPointOnly(Mat img, Mat templ, int match_method);

// Only Used In Log output split / Use 0 - 5 Match Method / show image / use threshold / input Mat
VOID SplitFontImgTest(Mat TextSrc);

// auto detect Log output area / input filename / ref SplitFontImgTest
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


// Only Used In Log output split / save sub image / input Mat
VOID SplitFontImg(Mat TextSrc);

// auto detect Log output area / input filename / ref SplitFontImg
VOID SplitFontImg_AutoIOR(string filename)
{
	Mat Src = imread(filename, IMREAD_COLOR);
	Mat IOR;
	if (Src.rows > 100)
		IOR = Src(Rect(127, 540, 676, 68));
	else
		IOR = Src;
	SplitFontImg(IOR);
}

VOID GetScreenCaptureWithIOR(LPSTR addr,Rect ior_rect)
{
	RECT rect;
	rect.left = ior_rect.x;
	rect.top = ior_rect.y;
	rect.right = ior_rect.x + ior_rect.width;
	rect.bottom = ior_rect.y + ior_rect.height;
	ScreenCapture(addr, 32, &rect);
}

VOID GetScreenCapture_LogArea(LPSTR addr)
{
	GetScreenCaptureWithIOR(addr, Rect(127, 540, 676, 68));
}
