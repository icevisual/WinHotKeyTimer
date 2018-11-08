// WinGHotKey.cpp : �������̨Ӧ�ó������ڵ㡣
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

#include<windows.h>
#include<Mmsystem.h>
#pragma comment(lib,"winmm.lib")
#include <map> 
#include "opencv2/calib3d.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/xfeatures2d.hpp"

#include <io.h>

using namespace std;
using namespace cv;
using namespace cv::xfeatures2d;

USING_DEBUG_LOG_VARS

BOOL GetIORArea(Mat Src, Mat &Output, Rect LogRect)
{
	if (Src.empty())
		return -1;
	if (Src.rows < LogRect.y + LogRect.height || Src.cols < LogRect.x + LogRect.width)
		return -2;
	Output = Src(LogRect);
	return 1;
}
BOOL GetRiskerGuideArea(Mat Src, Mat &Output)
{
	Rect LogRect(100, 160, 200, 60);
	return GetIORArea(Src, Output, LogRect);
}

BOOL GetSelectRiskerArea(Mat Src, Mat &Output)
{
	Rect LogRect(180, 160, 250, 60);
	return GetIORArea(Src, Output, LogRect);
}



BOOL GetLogArea(Mat Src, Mat &Output)
{
	Rect LogRect(127, 540, 676, 68);
	return GetIORArea(Src, Output, LogRect);

	if (Src.empty())
		return -1;
	if (Src.rows < LogRect.y + LogRect.height || Src.cols < LogRect.x + LogRect.width)
		return -2;
	Output = Src(LogRect);
	return 1;
}


VOID GetScreenCaptureWithIOR(LPSTR addr, Rect ior_rect)
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

// SURF �����֪ ����
// int SURFDetect(Mat img_object, Mat img_scene, Point2f &StartPoint, int min_matches_size = 15, int rate = 5)
int SURFDetect(Mat img_object, Mat img_scene, Point2f &StartPoint, size_t min_matches_size, int rate)
{
	resize(img_object, img_object, Size(), rate, rate);
	resize(img_scene, img_scene, Size(), rate, rate);

	if (img_object.empty() || img_scene.empty())
	{
		return -1;
	}
	//-- Step 1: Detect the keypoints using SURF Detector, compute the descriptors
	int minHessian = 400;
	Ptr<SURF> detector = SURF::create(minHessian);
	std::vector<KeyPoint> keypoints_object, keypoints_scene;
	Mat descriptors_object, descriptors_scene;
	detector->detectAndCompute(img_object, noArray(), keypoints_object, descriptors_object);
	detector->detectAndCompute(img_scene, noArray(), keypoints_scene, descriptors_scene);
	if (descriptors_scene.empty())
		return -5;
	//-- Step 2: Matching descriptor vectors with a FLANN based matcher
	// Since SURF is a floating-point descriptor NORM_L2 is used
	Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create(DescriptorMatcher::FLANNBASED);
	std::vector< std::vector<DMatch> > knn_matches;
	matcher->knnMatch(descriptors_object, descriptors_scene, knn_matches, 2);
	//-- Filter matches using the Lowe's ratio test
	const float ratio_thresh = 0.75f;
	std::vector<DMatch> good_matches;
	for (size_t i = 0; i < knn_matches.size(); i++)
	{
		if (knn_matches[i][0].distance < ratio_thresh * knn_matches[i][1].distance)
		{
			good_matches.push_back(knn_matches[i][0]);
		}
	}
	//  printf(" good_matches.size = %d \n", good_matches.size());
	if (good_matches.empty())
		return -3;
	if (good_matches.size() < min_matches_size)
		return -4;

	DEBUG_LOG("\t\tkeypoints_object.size = %d \n", keypoints_object.size());
	DEBUG_LOG("\t\tkeypoints_scene.size = %d \n", keypoints_scene.size());
	DEBUG_LOG("\t\tgood_matches.size = %d \n", good_matches.size());
	DEBUG_LOG("\t\tPercent = %.2f \n", good_matches.size() * 1.0 / keypoints_object.size());

	if (good_matches.size() * 1.0 / keypoints_object.size() < 0.4)
		return -6;


	//-- Draw matches
	Mat img_matches;
	drawMatches(img_object, keypoints_object, img_scene, keypoints_scene, good_matches, img_matches, Scalar::all(-1),
		Scalar::all(-1), std::vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
	//-- Localize the object
	std::vector<Point2f> obj;
	std::vector<Point2f> scene;
	for (size_t i = 0; i < good_matches.size(); i++)
	{
		//-- Get the keypoints from the good matches
		obj.push_back(keypoints_object[good_matches[i].queryIdx].pt);
		scene.push_back(keypoints_scene[good_matches[i].trainIdx].pt);
	}

	Mat H = findHomography(obj, scene, RANSAC);
	//-- Get the corners from the image_1 ( the object to be "detected" )
	std::vector<Point2f> obj_corners(4);
	obj_corners[0] = Point2f(0, 0);
	obj_corners[1] = Point2f((float)img_object.cols, 0);
	obj_corners[2] = Point2f((float)img_object.cols, (float)img_object.rows);
	obj_corners[3] = Point2f(0, (float)img_object.rows);
	std::vector<Point2f> scene_corners(4);

	if (H.empty())
		return -2;

	perspectiveTransform(obj_corners, scene_corners, H);
	//-- Draw lines between the corners (the mapped object in the scene - image_2 )
	StartPoint = scene_corners[0] / rate;
	return 1;
}

// Mat ����Ϊ MD5 ֵ����
// INT RenameMatWithMD5(Mat Input, String StoreFolder, String TempFolder = "../data/Temp/", String Ext = ".jpg")
INT RenameMatWithMD5(Mat Input, String StoreFolder, String TempFolder, String Ext)
{
	CHAR name[250] = { 0 };
	sprintf_s(name, "%s/%d%s", TempFolder.c_str(), GetMilliSecondOfDay(), Ext.c_str());
	cv::String tname(name);
	imwrite(tname, Input);
	CHAR new_name[250] = { 0 };
	string MD5Value = FileDigest(tname);
	sprintf_s(new_name, "%s/%s%s", StoreFolder.c_str(), MD5Value.c_str(), Ext.c_str());
	int r = rename(name, new_name);

	if (r < 0)
	{
		remove(name);
	}
	printf("MD5=%s rename=%d\n", MD5Value.c_str(), r);
	return r;
}

// �������� ��ʾ����
VOID ShowIOR(Mat image, int x, int y, int w, int h)
{ // GetIOR(127, 540 ,676 ,68);
  // 127, 540 ,676 ,68
  //  Mat image = cv::imread("../data/Src/shop.bmp");

	if (x + w > image.cols)
	{
		w = image.cols - x;
	}
	if (y + h > image.rows)
	{
		h = image.cols - y;
	}

	Mat imageROI = image(Rect(x, y, w, h));
	namedWindow("src");
	imshow("src", imageROI);
	// imwrite("../data/ior1.bmp", imageROI);
	waitKey(30);
}


// �������� ��ʾ���� ͬ���ĳߴ�
VOID ShowIOR_Items(Mat image, int x, int y, int w, int h)
{
	if (x + w > image.cols)
	{
		w = image.cols - x;
	}
	if (y + h > image.rows)
	{
		h = image.cols - y;
	}
	int count = 0;
	for (int i = y; i < image.rows - h; i += h)
	{
		Mat imageROI = image(Rect(x, i, w, h));
		CHAR name[50] = { 0 };
		sprintf_s(name, "IOR-%d", i);
		namedWindow(name);
		moveWindow(name, 0, count * 60);
		imshow(name, imageROI);


		CHAR store_name[100] = { 0 };
		sprintf_s(store_name, "../data/IOR/%s.bmp", name);
		imwrite(store_name, imageROI);

		count++;
		if (count > 15)
			break;
	}
	waitKey();
}

// shop item ��ֵͼ
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
	int match_method, BOOL use_threshold, BOOL show_img )
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

// ��Ŀ¼�½���ͼƬ�����г� ��־�� ����
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

// MergeImagesVertically("../data/Split/ConsoleArea/Diff",".jpg","../data/Split/ConsoleArea/Merge.bmp")
// ��ֱ�ϲ�ͼƬ���������� ��ɫɸѡ �ز�ͼƬ
VOID MergeImagesVertically(string Folder, string Ext, string MergedFilename, BOOL ShowImage = TRUE)
{
	vector<String> ResultVector;
	if (ListFilesWithExt_NDP(Folder, ResultVector, Ext))
	{
		vector<Mat> temp;
		INT Height = 0;
		INT Width = 0;
		if (ResultVector.empty())
			return;
		for (size_t i = 0; i < ResultVector.size(); i++)
		{
			Mat Src = imread(ResultVector[i], IMREAD_COLOR);
			temp.push_back(Src);
			Height += Src.rows;
			Width = max(Width, Src.cols);
		}
		Mat Merge(Height, Width, CV_8UC3);
		INT tmp = 0;
		for (size_t i = 0; i < temp.size(); i++)
		{
			//    img4_color.copyTo(canvas(Range::all(), Range(0, img1.cols)));
			Range rga(tmp, tmp + temp[i].rows);
			Range rgb(0, temp[i].cols);
			temp[i].copyTo(Merge(rga, rgb));
			tmp += temp[i].rows;
		}
		imwrite(MergedFilename, Merge);
		if (ShowImage == TRUE)
		{
			imshow("Merged", Merge);
			waitKey();
		}
	}
}


// ���ڵ��� ��ɫɸѡ �� inRange ��ֵ 
#pragma region inRange Color Filter

const int max_value_H = 360 / 2;
const int max_value = 255;
const String window_capture_name = "Video Capture";
const String window_detection_name = "Object Detection";
int low_H = 0, low_S = 0, low_V = 0;
//int low_H = 0, low_S = 43, low_V = 46;
int high_H = max_value_H, high_S = max_value, high_V = max_value;
//int high_H = 10, high_S = max_value, high_V = max_value;
Mat frame_threshold, frame_HSV;
static void redraw()
{
	inRange(frame_HSV, Scalar(low_H, low_S, low_V), Scalar(high_H, high_S, high_V), frame_threshold);
	imshow(window_detection_name, frame_threshold);
}
static void on_low_H_thresh_trackbar(int, void *)
{
	low_H = min(high_H - 1, low_H);
	setTrackbarPos("Low H", window_detection_name, low_H);
	redraw();
}
static void on_high_H_thresh_trackbar(int, void *)
{
	high_H = max(high_H, low_H + 1);
	setTrackbarPos("High H", window_detection_name, high_H);
	redraw();
}
static void on_low_S_thresh_trackbar(int, void *)
{
	low_S = min(high_S - 1, low_S);
	setTrackbarPos("Low S", window_detection_name, low_S);
	redraw();
}
static void on_high_S_thresh_trackbar(int, void *)
{
	high_S = max(high_S, low_S + 1);
	setTrackbarPos("High S", window_detection_name, high_S);
	redraw();
}
static void on_low_V_thresh_trackbar(int, void *)
{
	low_V = min(high_V - 1, low_V);
	setTrackbarPos("Low V", window_detection_name, low_V);
	redraw();
}
static void on_high_V_thresh_trackbar(int, void *)
{
	high_V = max(high_V, low_V + 1);
	setTrackbarPos("High V", window_detection_name, high_V);
	redraw();

}
int inRange_DM()
{
	namedWindow(window_capture_name);
	namedWindow(window_detection_name);
	// Trackbars to set thresholds for HSV values
	createTrackbar("Low H", window_detection_name, &low_H, max_value_H, on_low_H_thresh_trackbar);
	createTrackbar("High H", window_detection_name, &high_H, max_value_H, on_high_H_thresh_trackbar);
	createTrackbar("Low S", window_detection_name, &low_S, max_value, on_low_S_thresh_trackbar);
	createTrackbar("High S", window_detection_name, &high_S, max_value, on_high_S_thresh_trackbar);
	createTrackbar("Low V", window_detection_name, &low_V, max_value, on_low_V_thresh_trackbar);
	createTrackbar("High V", window_detection_name, &high_V, max_value, on_high_V_thresh_trackbar);

	Mat srcMerge = imread("../data/Split/ConsoleArea/Merge1.bmp", IMREAD_COLOR);
	// ../data/Temp/40661682/LOGAREA-40671469.bmp
	//Mat srcMerge = imread("../data/Temp/40661682/LOGAREA-40671469.bmp", IMREAD_COLOR);

	Mat frame;
	frame = srcMerge;
	// Convert from BGR to HSV colorspace
	cvtColor(frame, frame_HSV, COLOR_BGR2HSV);
	// Detect the object based on HSV Range Values
	inRange(frame_HSV, Scalar(low_H, low_S, low_V), Scalar(high_H, high_S, high_V), frame_threshold);
	// Show the frames
	imshow(window_capture_name, frame);
	imshow(window_detection_name, frame_threshold);
	waitKey();
	return 0;
}
#pragma endregion


// ʹ�� inRange ɸѡ ͼƬ�е� EC ��ɫ����
VOID filter_ec_red(Mat src, Mat &output)
{
	// Convert from BGR to HSV colorspace
	cvtColor(src, output, COLOR_BGR2HSV);
	// Detect the object based on HSV Range Values
//	inRange(output, Scalar(0, 28, 201), Scalar(180, 80, 224), output);
	inRange(output, Scalar(0, 28, 96), Scalar(8, 255, 255), output);
}
// ʹ�� inRange ɸѡ ͼƬ�е� EC ��ɫ����
VOID filter_ec_green(Mat src, Mat &output)
{
	// Convert from BGR to HSV colorspace
	cvtColor(src, output, COLOR_BGR2HSV);
	// Detect the object based on HSV Range Values
	inRange(output, Scalar(33, 18, 201), Scalar(77, 255, 255), output);
}


// �ж�ͼƬ���Ƿ���� ��ɫ ��
BOOL JudgeHasWhitePoint(Mat Src)
{
	double minVal; double maxVal;
	Point minLoc; Point maxLoc;
	minMaxLoc(Src, &minVal, &maxVal, &minLoc, &maxLoc, Mat());
	// DEBUG_LOG("JudgeHasWhitePoint >> minVal = %.2lf maxVal = %.2lf minLoc=(%d,%d) maxLoc=(%d,%d)\n", minVal, maxVal, minLoc.x, minLoc.y, maxLoc.x, maxLoc.y);

	if (minVal < 1e-6 && maxVal < 1e-6)
		return FALSE;

	return TRUE;
}

// �ж�ͼƬ���Ƿ���� EC ��ɫ ��
BOOL JudgeRedPoint(Mat Src)
{
	Mat Temp;
	filter_ec_red(Src, Temp);
	return JudgeHasWhitePoint(Temp);
}
// �ж�ͼƬ���Ƿ���� EC ��ɫ ��
BOOL JudgeGreenPoint(Mat Src)
{
	Mat Temp;
	filter_ec_green(Src, Temp);
	return JudgeHasWhitePoint(Temp);
}


//  ͨ���ļ����ڵ�ͼƬ������ filter_ec_red | green 
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

// �жϸɺ�
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
	if (SURFDetect(img_object, img_scene, StartPoint,100) > 0)
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



int CreateProcessDM(TCHAR * sConLin)
{
//	char cWindowsDirectory[MAX_PATH];

	//LPTSTR �� wchar_t* �ȼ�(Unicode������)  
	LPTSTR cWinDir = new TCHAR[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, cWinDir);

	//	LPTSTR sConLin = wcscat_s(cWinDir, MAX_PATH, L"\"D:\\Program Files\\Git\\usr\\bin\\bash.exe\" D:\\desktop\\rr.sh");
	// wcscat_s(cWinDir, MAX_PATH, L"\"D:\\Program Files\\Git\\usr\\bin\\bash.exe\" D:\\desktop\\rr.sh");
	wsprintf(cWinDir, sConLin);// L"\"D:\\Program Files\\Git\\usr\\bin\\bash.exe\" D:\\desktop\\rr.sh");

	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	ZeroMemory(&pi, sizeof(pi));

	//����һ���½���  
	if (CreateProcess(
		NULL,   //  ָ��һ��NULL��β�ġ�����ָ����ִ��ģ��Ŀ��ֽ��ַ���  
		cWinDir, // �������ַ���  
		NULL, //    ָ��һ��SECURITY_ATTRIBUTES�ṹ�壬����ṹ������Ƿ񷵻صľ�����Ա��ӽ��̼̳С�  
		NULL, //    ���lpProcessAttributes����Ϊ�գ�NULL������ô������ܱ��̳С�<ͬ��>  
		false,//    ָʾ�½����Ƿ�ӵ��ý��̴��̳��˾����   
		0,  //  ָ�����ӵġ���������������ͽ��̵Ĵ����ı�  
			//  CREATE_NEW_CONSOLE  �¿���̨���ӽ���  
			//  CREATE_SUSPENDED    �ӽ��̴��������ֱ������ResumeThread����  
		NULL, //    ָ��һ���½��̵Ļ����顣����˲���Ϊ�գ��½���ʹ�õ��ý��̵Ļ���  
		NULL, //    ָ���ӽ��̵Ĺ���·��  
		&si, // �����½��̵������������ʾ��STARTUPINFO�ṹ��  
		&pi  // �����½��̵�ʶ����Ϣ��PROCESS_INFORMATION�ṹ��  
	))
	{
		cout << "create process success" << endl;

		//�������йرվ������������̺��½��̵Ĺ�ϵ����Ȼ�п��ܲ�С�ĵ���TerminateProcess�����ص��ӽ���  

		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
	else {
		ShowLastErrorMsg(L"CreateProcess");
		cerr << "failed to create process" << endl;
	}
	return 0;
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
	GetScreenCaptureWithIOR("../data/Temp/Before-SimulateQuiteGame.bmp", Rect(0, 0, 805, 628));
	Sleep(500);
	GetScreenCaptureWithIOR("../data/Temp/Mid-SimulateQuiteGame.bmp", Rect(0, 0, 805, 628));
	DEBUG_LOG("SimulateQuiteGame\n");
	ConvertChar2KeyWordAndSimulate("S");
	Sleep(200);
	ConvertChar2KeyWordAndSimulate("a");
	Sleep(400);
	WORD Keys[] = {VK_RETURN};
	SimulateKeyArrayInput(Keys, 1);
	GetScreenCaptureWithIOR("../data/Temp/After-SimulateQuiteGame.bmp", Rect(0, 0, 805, 628));
}

VOID SimulateEnterGame()
{
	DEBUG_LOG("SimulateEnterGame\n");
	ConvertChar2KeyWordAndSimulate("a");
	Sleep(200);
	ConvertChar2KeyWordAndSimulate("a");
	Sleep(1500);
}

VOID Gfname(string storage,string prefix,string suffix, string &output)
{
	CHAR temp[50] = {0};
	sprintf_s(temp, "%d", GetMilliSecondOfDay());
	output = storage + "/" + prefix + temp + suffix;
}

VOID Testfff()
{
	//Mat Screen1  = imread("../data/Temp/40661682/LOGAREA-40671469.bmp",IMREAD_COLOR);
	//// ../data/Temp/LOGAREA-64498098.bmp
	//GetLogArea(Screen1, Screen1);
	//if (DetectWhatYouWant(Screen1) == TRUE)
	//{
	//	printf("YES\n");
	//}
	//else
	//{
	//	printf("NO\n");
	//}

	//if (JudgeRedPoint(Screen1) == TRUE)
	//{
	//	printf("YES\n");
	//}
	//else
	//{
	//	printf("NO\n");
	//}
	//if (JudgeGreenPoint(Screen1) == TRUE)
	//{
	//	printf("YES\n");
	//}
	//else
	//{
	//	printf("NO\n");
	//}
	//SaveEC();
	//return;
}


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

VOID SimulateMouseClick(int x, int y )
{
	::SetCursorPos(x,y);

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
	GetScreenCaptureWithIOR(name,Rect(0,400,70,125));

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

		GetScreenCaptureWithIOR(name, Rect(0, 0, 805, 628));
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
		GetScreenCaptureWithIOR(name, Rect(0, 0, 805, 628));
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
	// �� EC ����
	// ���г��� [������,С��][ȫ�ڣ�ȫ��][ð�յ�·��][ѡ��ð����]
	// �жϽ�����ҳ
	// ģ�� a
	// �жϽ�������ѡ��ҳ��
	// ģ�� a
	// �ж� ��������ѡ��ҳ��
	// ���� ��ȡ�����
	// �ж� ��ɫ����������ɫ����������
	// �ж� �ɺԣ������������������
	// �ж� ��Ը

}


// �ֽ� ���� �������ÿ�н�ȡ����ͼ�Σ�ʹ�� templateMatch �����ò�ͨ method ������
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



// �ֽ� ���� �������ÿ�н�ȡ����ͼ��
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
