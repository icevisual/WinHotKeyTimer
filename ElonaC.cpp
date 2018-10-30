// WinGHotKey.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h"
// #include "ElonaC.h"
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

int SURFDetect(Mat img_object, Mat img_scene, Point2f &StartPoint,int min_matches_size = 15, int rate = 5)
{
	img_scene = img_scene(Rect(0, 0, img_scene.cols / 10, img_scene.rows));
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
//	printf(" good_matches.size = %d \n", good_matches.size());
	if (good_matches.empty())
		return -3;
	if (good_matches.size() < min_matches_size)
		return -4;

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


INT RenameMatWithMD5(Mat Input, String StoreFolder, String TempFolder = "../data/Temp/", String Ext = ".jpg")
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


VOID ShowIOR(Mat image, int x,int y ,int w,int h)
{ // GetIOR(127, 540 ,676 ,68);
	// 127, 540 ,676 ,68
//	Mat image = cv::imread("../data/Src/shop.bmp");

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
	// imwrite("../data/ior1.bmp", imageROI);
	waitKey(0);
}



VOID ShowIOR_Items(Mat image ,int x, int y, int w, int h)
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
		moveWindow(name,0, count * 60);
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
	int match_method,BOOL use_threshold = TRUE, BOOL show_img = TRUE)
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

VOID ScanXuyuanFolder_GetLogArea()
{
	vector<String> ResultVector;

	if (ListFilesWithExt_NDP("../data/Src/xuyuan", ResultVector,".bmp"))
	{
		for (int i = 0; i < ResultVector.size(); i++)
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


const int max_value_H = 360 / 2;
const int max_value = 255;
const String window_capture_name = "Video Capture";
const String window_detection_name = "Object Detection";
int low_H = 0, low_S = 0, low_V = 0;
int high_H = max_value_H, high_S = max_value, high_V = max_value;
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
	
	createTrackbar("Low S", window_detection_name, &low_S, max_value, on_low_S_thresh_trackbar);

	createTrackbar("Low V", window_detection_name, &low_V, max_value, on_low_V_thresh_trackbar);

	createTrackbar("High H", window_detection_name, &high_H, max_value_H, on_high_H_thresh_trackbar);
	createTrackbar("High S", window_detection_name, &high_S, max_value, on_high_S_thresh_trackbar);
	createTrackbar("High V", window_detection_name, &high_V, max_value, on_high_V_thresh_trackbar);

	Mat src0hong = imread("../data/Split/ConsoleArea/4a2e0a21024688dd9355c0d737626910.jpg", IMREAD_COLOR);
	// bai
	Mat src1bai = imread("../data/Split/ConsoleArea/6edcf4542d873097e366d2dfe2d2f0f4.jpg", IMREAD_COLOR);
	// lu
	Mat src2lu = imread("../data/Split/ConsoleArea/d8ce3bef07ee2f4322a0025980ee3e28.jpg", IMREAD_COLOR);

	Mat frame;
	frame = src0hong;
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




INT calcHist_DM(Mat src,String name)
{
	if (src.empty())
	{
		return -1;
	}
	vector<Mat> bgr_planes;
	split(src, bgr_planes);
	int histSize = 256;
	float range[] = { 0, 256 }; //the upper boundary is exclusive
	const float* histRange = { range };
	bool uniform = true, accumulate = false;
	Mat b_hist, g_hist, r_hist;
	calcHist(&bgr_planes[0], 1, 0, Mat(), b_hist, 1, &histSize, &histRange, uniform, accumulate);
	calcHist(&bgr_planes[1], 1, 0, Mat(), g_hist, 1, &histSize, &histRange, uniform, accumulate);
	calcHist(&bgr_planes[2], 1, 0, Mat(), r_hist, 1, &histSize, &histRange, uniform, accumulate);
	int hist_w = 512, hist_h = 400;
	int bin_w = cvRound((double)hist_w / histSize);
	Mat histImage(hist_h, hist_w, CV_8UC3, Scalar(0, 0, 0));
	normalize(b_hist, b_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());
	normalize(g_hist, g_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());
	normalize(r_hist, r_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());
	for (int i = 1; i < histSize; i++)
	{
		line(histImage, Point(bin_w*(i - 1), hist_h - cvRound(b_hist.at<float>(i - 1))),
			Point(bin_w*(i), hist_h - cvRound(b_hist.at<float>(i))),
			Scalar(255, 0, 0), 2, 8, 0);
		line(histImage, Point(bin_w*(i - 1), hist_h - cvRound(g_hist.at<float>(i - 1))),
			Point(bin_w*(i), hist_h - cvRound(g_hist.at<float>(i))),
			Scalar(0, 255, 0), 2, 8, 0);
		line(histImage, Point(bin_w*(i - 1), hist_h - cvRound(r_hist.at<float>(i - 1))),
			Point(bin_w*(i), hist_h - cvRound(r_hist.at<float>(i))),
			Scalar(0, 0, 255), 2, 8, 0);
	}
	imshow("Source image" + name, src);
	imshow("calcHist Demo" + name, histImage);
	return 0;
}

INT ColorStatistics()
{
	// Color Filter
	// hong
	Mat src0hong = imread("../data/Split/ConsoleArea/4a2e0a21024688dd9355c0d737626910.jpg", IMREAD_COLOR);
	// bai
	Mat src1bai = imread("../data/Split/ConsoleArea/6edcf4542d873097e366d2dfe2d2f0f4.jpg", IMREAD_COLOR);
	// lu
	Mat src2lu = imread("../data/Split/ConsoleArea/d8ce3bef07ee2f4322a0025980ee3e28.jpg", IMREAD_COLOR);
	
	calcHist_DM(src0hong,"src0hong");
	calcHist_DM(src1bai,"src1bai");
	calcHist_DM(src2lu,"src2lu");
	waitKey();
	return 0;
}


VOID SplitFontImgTest(Mat TextSrc)
{
	INT TextPaddingTop = 3;
	INT TextPaddingLeft = 30;
	INT TextHeight = 15;
	INT TextYOffset = 4;
	INT Width = TextSrc.cols - 30;
	Mat Rows[4];

	inRange_DM();
	return;



	RenameMatWithMD5(TextSrc,"../data/Split/ConsoleArea");

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
		
		cvtColor(temp, temp_gray, COLOR_BGR2GRAY);
		
		threshold(temp_gray, temp_binary, 70 + i * 25, 255, 0);

		cvtColor(temp_binary, temp_match, COLOR_GRAY2BGR);
		cv::String temp_window = "temp";
		cv::String gray_window =  "gray";
		cv::String binary_window = "binary";
		cv::String match_window =  "match";
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
			RenameMatWithMD5(Rows,"../data/Split");
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
