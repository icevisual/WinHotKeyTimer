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
#include "opencv2/calib3d.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/xfeatures2d.hpp"

#include <io.h>

USING_DEBUG_LOG_VARS

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


// 分解 四行 输出栏，每行截取文字图段，使用 templateMatch ，轮用不通 method ，测试
VOID SplitFontImgTest(Mat TextSrc)
{
	INT TextPaddingTop = 3;
	INT TextPaddingLeft = 30;
	INT TextHeight = 15;
	INT TextYOffset = 4;
	INT Width = TextSrc.cols - 30;
	Mat Rows[4];

	RenameMatWithMD5(TextSrc, "../data/Split/ConsoleArea");

	Mat empty = imread("../data/empty.jpg", IMREAD_COLOR);
	Mat templ = empty(Rect(0, 0, empty.cols, TextHeight));
	INT Offset = TextSrc.rows;
	INT TitleHeight = 40;
	imshow("TextSrc", TextSrc);
	moveWindow("TextSrc", 0, 0);
	for (int i = 0; i < 4; i++)
	{
		Offset = TextSrc.rows;
		Mat temp_gray;
		Mat temp_binary;
		Mat temp_match;
		Mat temp = TextSrc(Rect(TextPaddingLeft, TextYOffset + i * TextHeight, Width, TextHeight));

		cvtColor(temp, temp_gray, COLOR_BGR2GRAY);

		threshold(temp_gray, temp_binary, 70 + i * 25, 255, 0);

		cvtColor(temp_binary, temp_match, COLOR_GRAY2BGR);
		cv::String temp_window = "temp";
		cv::String gray_window = "gray";
		cv::String binary_window = "binary";
		cv::String match_window = "match";
		cv::String result_window = "result";
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
			Point Loc = GetMatchedStartPointOnly(temp_match, templ, j);
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



// 分解 四行 输出栏，每行截取文字图段
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
		cvtColor(temp, temp_gray, COLOR_BGR2GRAY);
		threshold(temp_gray, temp_binary, 70 + i * 25, 255, 0);
		cvtColor(temp_binary, temp_match, COLOR_GRAY2BGR);
		Point Loc = GetMatchedStartPointOnly(temp_match, templ, 0);
		if (Loc.x > 50)
		{
			Rows = temp_match(Rect(0, 0, Loc.x, TextHeight));
			RenameMatWithMD5(Rows, "../data/Split");
		}
		printf("%d %d \n", Loc.x, Loc.y);
	}
}
////// 行处理


////// MatchTemplate

// shop item 二值图
VOID DoThreshold(Mat Src, Mat &out, double thresh, double maxval)
{
	Mat temp_gray;
	Mat temp_binary;
	Mat temp_match;
	cvtColor(Src, temp_gray, COLOR_BGR2GRAY);
	threshold(temp_gray, temp_binary, 150, 255, 0);
	cvtColor(temp_binary, temp_match, COLOR_GRAY2BGR);
	out = temp_match.clone();
}

// Method: \n 0: SQDIFF \n 1: SQDIFF NORMED \n 2: TM CCORR \n 3: TM CCORR NORMED \n 4: TM COEFF \n 5: TM COEFF NORMED
Point GetMatchedStartPointWithFName(
	string img_name, string templ_name,
	int match_method, BOOL use_threshold, BOOL show_img)
{
	Mat img = imread(img_name, IMREAD_COLOR);
	Mat templ = imread(templ_name, IMREAD_COLOR);

	if (use_threshold == TRUE)
	{
		DoThreshold(img, img, 150, 255);
		DoThreshold(templ, templ, 150, 255);
	}

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

	printf("matchLoc = (%d,%d)\n", matchLoc.x, matchLoc.y);

	if (show_img == TRUE)
	{
		rectangle(img_display, matchLoc, Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows), Scalar(0, 0, 255), 2, 8, 0);
		rectangle(result, matchLoc, Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows), Scalar::all(0), 2, 8, 0);
		imshow("MT:Source Image", img_display);
		imshow("MT:matchTemplate Result Image", result);
		waitKey();
	}
	return matchLoc;
}

// Method: \n 0: SQDIFF \n 1: SQDIFF NORMED \n 2: TM CCORR \n 3: TM CCORR NORMED \n 4: TM COEFF \n 5: TM COEFF NORMED
Point GetMatchedStartPoint(Mat img, Mat templ, int match_method, BOOL show_img)
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
Point GetMatchedStartPointOnly(Mat img, Mat templ, int match_method)
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
	// printf("minVal = %f,maxVal = %f\n", minVal, maxVal);
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