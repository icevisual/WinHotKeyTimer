#pragma once

#include "stdafx.h"

#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>  

#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/calib3d.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/xfeatures2d.hpp"

#include <iostream>  
using namespace std;
using namespace cv;
using namespace cv::xfeatures2d;


#ifndef ElonaCH
#define ElonaCH



VOID RunWishing();

VOID RunTrain();

VOID CutLeftStateBar();

//////// ITEMS OPTS WITH MATCHTEMPLATE

// only used in shop items split / wipe off disturb
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
// Only Used In Log output split / save sub image / input Mat
VOID SplitFontImg(Mat TextSrc);
// auto detect Log output area / input filename / ref SplitFontImgTest
VOID SplitFontImgTest_AutoIOR(string filename);
// auto detect Log output area / input filename / ref SplitFontImg
VOID SplitFontImg_AutoIOR(string filename);



//////// PUBLIC
// Screen Capture / Save File / Cut Image Using Rect
VOID GetScreenCaptureWithIOR(LPSTR addr, Rect ior_rect);
// Screen Capture / Save File / Cut Log Area
VOID GetScreenCapture_LogArea(LPSTR addr);
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
// Mat(Rect(x,y,w,h)) / check size / return success or not
BOOL GetIORArea(Mat Src, Mat &Output, Rect LogRect);
// scan files / merge image vertically / specify merged filename / show merged file 
VOID MergeImagesVertically(string Folder, string Ext, string MergedFilename, BOOL ShowImage = TRUE);
// input Mat / cut Log area
BOOL GetLogArea(Mat Src, Mat &Output);

#endif // !ElonaCH



