#pragma once

#ifndef ECC_Base_H  
#define ECC_Base_H  

// EC Class Header
//
//
#include "utils.h"
#include "opencv2/core.hpp"
#include <opencv2/opencv.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>  
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"

#include <iostream>  
#include "opencv2/calib3d.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/xfeatures2d.hpp"

#include <io.h>

using namespace std;
using namespace cv;
using namespace cv::xfeatures2d;


class  ECC_Base
{
public:
	 ECC_Base();
	~ ECC_Base();
private:

public:


	VOID ShowSURFDetectImage(Mat img_scene, Mat img_object, Point2f StartPoint);
	// show interest area 
	VOID ShowIOR(Mat image, int x, int y, int w, int h);
	// show interest areas with the same size orderly
	VOID ShowIOR_Items(Mat image, int x, int y, int w, int h);
	// calc file md5 as filenam / Input Mat / dist folder / target ext
	INT RenameMatWithMD5(Mat Input, String StoreFolder, String TempFolder = "../data/Temp/", String Ext = ".jpg");
	// Using SURF to Dectect ping juan In Shop List Items Img
	INT SURFDetect(Mat img_object, Mat img_scene, Point2f &StartPoint, size_t min_matches_size = 15, int rate = 5);
	// storage / prefix / suffix for ext / output is the filename / using GetMilliSecondOfDay as filename
	VOID Gfname(string storage, string prefix, string suffix, string &output);
	// using minMaxLoc
	BOOL JudgeHasWhitePoint(Mat Src);

	// scan files / merge image vertically / specify merged filename / show merged file 
	VOID MergeImagesVertically(string Folder, string Ext, string MergedFilename, BOOL ShowImage = TRUE);
	// Screen Capture / Save File / Cut Image Using Rect
	VOID GetScreenCaptureWithIOR(LPSTR addr, Rect ior_rect);
	// Screen Capture / Save File / Cut Log Area
	VOID GetScreenCapture_LogArea(LPSTR addr);
	// Screen Capture / Save File / Cut Game Area
	VOID GetScreenCapture_GameArea(LPSTR addr);

	VOID GetScreenCapture_LeftStateArea(LPSTR addr);

	// Mat(Rect(x,y,w,h)) / check size / return success or not
	BOOL GetIORArea(Mat Src, Mat &Output, Rect LogRect);
	// input Mat / cut Log area
	BOOL GetLogArea(Mat Src, Mat &Output);
	// input Mat / cut Game area
	BOOL GetGameArea(Mat Src, Mat &Output);


	VOID CutLeftStateBar();
	VOID CutGameArea();
};

 ECC_Base:: ECC_Base()
{
}

 ECC_Base::~ ECC_Base()
{
}

#endif