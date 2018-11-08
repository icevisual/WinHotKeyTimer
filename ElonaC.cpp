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
