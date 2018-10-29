#include "stdafx.h"


#include <iostream>

#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/calib3d.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/xfeatures2d.hpp"
using namespace cv;
using namespace cv::xfeatures2d;


int SURFDetect(Mat img_object, Mat img_scene, Point2f &StartPoint, int rate = 5)
{
	img_scene = img_scene(Rect(0, 0, img_scene.cols / 8, img_scene.rows));
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



int main(int argc, char* argv[])
{
	//Mat img_object = imread(parser.get<String>("input1"), IMREAD_GRAYSCALE);
	//Mat img_scene = imread(parser.get<String>("input2"), IMREAD_GRAYSCALE);

	Mat img_object = imread("../data/Quest/IOR-juan2.bmp", IMREAD_GRAYSCALE);
	//Mat img_scene = imread("../data/Quest/IOR-298.bmp", IMREAD_GRAYSCALE);
	// 279
//	Mat img_scene = imread("../data/IOR/IOR-279.bmp", IMREAD_GRAYSCALE);
	// ∫Û√Ê
	Mat img_scene = imread("../data/IOR/IOR-336.bmp", IMREAD_GRAYSCALE);

	Point2f StartPoint;
	if (SURFDetect(img_object, img_scene, StartPoint))
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
		waitKey();
	}
	return 0;

}
