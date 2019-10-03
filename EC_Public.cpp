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
#include "opencv2/calib3d.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/xfeatures2d.hpp"

#include <io.h>

USING_DEBUG_LOG_VARS

// 判断图片中是否包含 白色 点
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


VOID Gfname(string storage, string prefix, string suffix, string &output)
{
	CHAR temp[50] = { 0 };
	sprintf_s(temp, "%d", GetMilliSecondOfDay());
	output = storage + "/" + prefix + temp + suffix;
}

BOOL GetIORArea(Mat Src, Mat &Output, Rect LogRect)
{
	if (Src.empty())
		return -1;
	if (Src.rows < LogRect.y + LogRect.height || Src.cols < LogRect.x + LogRect.width)
		return -2;
	Output = Src(LogRect);
	return 1;
}

BOOL GetLogArea(Mat Src, Mat &Output)
{
	Rect LogRect(127, 540, 676, 68);
	return GetIORArea(Src, Output, LogRect);
}

BOOL GetGameArea(Mat Src, Mat &Output)
{
	Rect LogRect(0, 0, 805, 628);
	return GetIORArea(Src, Output, LogRect);
}


VOID GetScreenCapture_GameArea(LPSTR addr)
{
	//GetScreenCaptureWithIOR(addr, Rect(0, 0, 805, 628));
	GetScreenCaptureWithIOR(addr, Rect(0, 0, 1010, 800));
}

VOID GetScreenCapture_LogArea(LPSTR addr)
{
	GetScreenCaptureWithIOR(addr, Rect(127, 540, 676, 68));
}

VOID GetScreenCapture_LeftStateArea(LPSTR addr)
{
	GetScreenCaptureWithIOR(addr, Rect(5, 400, 70, 120));
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



// SURF 检测已知 物体
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


// Mat 保存为 MD5 值名称
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



// 用于调试 颜色筛选 的 inRange 阈值 
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

// MergeImagesVertically("../data/Split/ConsoleArea/Diff",".jpg","../data/Split/ConsoleArea/Merge.bmp")
// 垂直合并图片，用于整合 颜色筛选 素材图片
VOID MergeImagesVertically(string Folder, string Ext, string MergedFilename, BOOL ShowImage)
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


//// AREA Utils

// 根据坐标 显示区域
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
	moveWindow("src",0,0);
	imshow("src", imageROI);
	// imwrite("../data/ior1.bmp", imageROI);
	waitKey(30);
}


// 根据坐标 显示区域 同样的尺寸
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


VOID ShowSURFDetectImage(Mat img_scene, Mat img_object, Point2f StartPoint)
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
	waitKey(3000);
}