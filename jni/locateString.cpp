#include <jni.h>
#include <string.h>
#include <memory.h>
#include <stdio.h>
#include <android/log.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
//#include "opencv2/imgproc.hpp"
//#include "opencv2/imgcodecs.hpp"

using namespace cv;

#include "array.h"
#include "common.h"

////////////////////////////////////////////////////////////////////////////////////////////
extern "C" {

#define STRING_COUNT 40
#define OCR_PIXEL_MAX 255
#define OCR_PIXEL_MIN 0
#define USER_DEFINED__H_MARGIN 5
#define USER_DEFINED__W_MARGIN 5
#define STRING_HEIGHT_ACCEPTABLE_RANGE_MIN (2 * USER_DEFINED__H_MARGIN + 5)
#define STRING_HEIGHT_ACCEPTABLE_RANGE_MAX 40

bool isMessage = false;

void getRectList(std::vector<cv::Rect>& rectlist,Mat extracted)
{
	int top, left, right, bottom;
	int *sum_h, *sum_v;
	uchar *data = extracted.data;
	sum_h = new int[extracted.rows];
	sum_v = new int[extracted.cols];
	////////Get Top & Bottom (an info about the horizontal location of an assumed StringRect)///////////////////////////
	calcHorizontalProjection(extracted, sum_h, OCR_PIXEL_MAX);
	eliminateLowValueRegion(sum_h, extracted.rows, 2);
	blurArray(sum_h, extracted.rows,1);
	///
	for (int i = 0; i < STRING_COUNT; i++)
	{
		if (!isMessage)
			findTopBottom(sum_h, extracted.rows, top, bottom);
		else
			findTopBottom_bottom2up(sum_h, extracted.rows, top, bottom);
		top -= USER_DEFINED__H_MARGIN;
		bottom += USER_DEFINED__H_MARGIN;
		if (top < 0) top = 0;
		if (bottom >= extracted.rows) bottom = extracted.rows - 1;
		//
		if (top == 0 &&
			bottom == (extracted.rows - 1))
			break;
		//Ignore Non-String Part(Line Part)//////////////////////////////////////////////////////////
		if (((bottom - top) < STRING_HEIGHT_ACCEPTABLE_RANGE_MIN) ||
			((bottom - top) > STRING_HEIGHT_ACCEPTABLE_RANGE_MAX))
		{
			//memset(data + extracted.cols * top, 0, extracted.cols * (bottom - top));
			for (int ii = top; ii < bottom; ii++)
				sum_h[ii] = 0;
			continue;
		}
		/////////Get Left & Right (an info about the vertical location of an assumed StringRect)///////////////////////////
		Mat croppedbyTopBottom(extracted, cv::Rect(0, top, extracted.cols, bottom - top));
		calcVerticalProjection(croppedbyTopBottom, sum_v, OCR_PIXEL_MAX);

		int left2, right2;
		findLeftRight_revised(sum_v, croppedbyTopBottom.cols, left, right,left2,right2,isMessage);
		////////////////////////////kjy-todo-2015.05.08///////////
		if (left2 == right2)
		{
			left -= USER_DEFINED__W_MARGIN;
			right += USER_DEFINED__W_MARGIN;
			////////////////////////////////////////////////
			if (left < 0) left = 0;
			if (right >= extracted.cols) right = extracted.cols - 1;
			/////////Form the rect of the suspicious StringRect///////////////////////////////
			cv::Rect rect = cv::Rect(left, top, right - left, bottom - top);
			/////////Reflect the Engine//////////////////////////////////////////////////////////
			for (int ii = top; ii < bottom; ii++)
				sum_h[ii] = 0;
			//exception handle////////////////////////////////////////////////
			if (top == 0 &&
				bottom == (extracted.rows - 1))
				continue;
			rectlist.push_back(rect);
		}
		else
		{
			left -= USER_DEFINED__W_MARGIN;
			left2 -= USER_DEFINED__W_MARGIN;
			right += USER_DEFINED__W_MARGIN;
			right2 += USER_DEFINED__W_MARGIN;
			////////////////////////////////////////////////
			if (left < 0) left = 0;
			if (left2 < 0) left2 = 0;
			if (right >= extracted.cols) right = extracted.cols - 1;
			if (right2 >= extracted.cols) right2 = extracted.cols - 1;
			/////////Form the rect of the suspicious StringRect///////////////////////////////
			cv::Rect rect1 = cv::Rect(left, top, right - left, bottom - top),
					 rect2 = cv::Rect(left2, top, right2 - left2, bottom - top);
			/////////Reflect the Engine//////////////////////////////////////////////////////////
			for (int ii = top; ii < bottom; ii++)
				sum_h[ii] = 0;
			//////////////////////////////////////////////////
			rectlist.push_back(rect1);
			rectlist.push_back(rect2);
		}
		if (isMessage)
			break;
	}
	delete[]sum_v;
	delete[]sum_h;
	return;
}
////////////////////////////////////////////
bool isGreenMessage(Mat in,Mat hsv,Mat& gray)
{
	if (hsv.empty() || hsv.type() != CV_8UC3)
		return false;

	bool isTrue = false;
	int  left_start = 0, right_start = 0;
	int j;

	for (j = 0; j < hsv.rows; j++)
	{
		bool isLeft = false;
		for (int i = 0; i < hsv.cols; i++)
		{
			//here rgb bgr? you must pay attention!!!
			//Black-Only-Filter
			int r = *in.ptr(j, i);
			int g = *(in.ptr(j, i) + 1);
			int b = *(in.ptr(j, i) + 2);
			if (*hsv.ptr(j, i) != 0 ||
				r != g ||
				r != b ||
				g != b)
			{
				*gray.ptr(j, i) = 255;
			}
			//GreenDetect
			if ((*hsv.ptr(j, i) >100 && *hsv.ptr(j, i) < 110) &&
				(*(hsv.ptr(j, i) + 1) > 100 && *(hsv.ptr(j, i) + 1) <180) &&
				*(hsv.ptr(j, i) + 2) > 180)
			{
				if (i * 2 < hsv.cols)
				{
					if (!isLeft && 2 * j > hsv.rows)
					{
						OCRTRACE;
						isLeft = true;
						left_start = i;
					}
				}
				else
				{
					if (isLeft && (i - left_start) * 2 > hsv.cols)
					{
						OCRTRACE;
						isTrue = true;
					}
				}
				if (j * 1.2 > in.rows && i * 2 > in.cols)
					*gray.ptr(j, i) = 0;
			}
		}
		if (isTrue)
			break;
	}
#define GREEN_STRING_HEIGHT 30
	if (isTrue)
	{
		for (int jj = (j - GREEN_STRING_HEIGHT); jj < hsv.rows; jj++)
		{
			for (int ii = 0; ii < hsv.cols; ii++)
			{
				//here rgb bgr? you must pay attention!!! android bitmap data ---rgb data
				int r = *in.ptr(jj, ii);
				int g = *(in.ptr(jj, ii) + 1);
				int b = *(in.ptr(jj, ii) + 2);
				//Black-Only-Filter
				if (jj >= j)
				{
					if (*hsv.ptr(jj, ii) != 0 ||
						r != g ||
						r != b ||
						g != b)
						*gray.ptr(jj, ii) = 255;
				}
				//GreenFilter
				if ((*hsv.ptr(jj, ii) >100 && *hsv.ptr(jj,ii) < 110) &&
					(*(hsv.ptr(jj, ii) + 1) > 100 && *(hsv.ptr(jj, ii) + 1) <180) &&
					*(hsv.ptr(jj, ii) + 2) > 150)
					*gray.ptr(jj, ii) = 0;
			}
		}
	}
	return isTrue;
}
////////////////////////////////////////////
Mat estimateType(Mat in)
{
	resize(in, in, Size(300, 600));

	Mat gray, hsv;
	cvtColor(in, hsv, CV_BGR2HSV);
	cvtColor(in, gray, CV_BGR2GRAY);

	Mat hist;
	resize(in, hist, Size(20, 40), 0.0, 0.0, 0);
	cvtColor(hist, hist, CV_BGRA2GRAY);
	threshold(hist, hist, 0, 255, CV_THRESH_OTSU + CV_THRESH_BINARY);

	isMessage = isGreenMessage(in,hsv,gray);

	//filtering
#define MESSAGE_MASK
#ifdef MESSAGE_MASK
	bool everInwhite = false;
	for (int j = 0; j < hist.rows; j++)
	{
		int whites = 0,
			blacks = 0;
		int first_white = -1,
			first_black = -1,
			last_white = -1,
			last_black = -1;
		//set first and last white,black pos
		for (int i = 0; i < hist.cols; i++)
		{
			if (*hist.ptr(j, i) > 128)
			{
				if (first_white == -1)
					first_white = i;
				last_white = i;
				whites++;
			}
			else
			{
				if (first_black == -1)
					first_black = i;
				last_black = i;
				blacks++;
			}
		}
		//
		int margin = 1;

		if (whites > blacks)
		{
			everInwhite = true;
			if (first_white != 0)
			{
				memset(hist.data + j * hist.cols + first_white, 255, last_white - first_white - margin);
				memset(hist.data + j * hist.cols + last_white, 0, margin);//shrink-row
			}
			else
				memset(hist.data + j * hist.cols + first_white, 255, hist.cols);
		}
		else
		{
			if (everInwhite)
				memset(hist.data + (j - 1) * hist.cols, 0, hist.cols);//shrink-col
			everInwhite = false;
			if (first_black != 0)
				memset(hist.data + j * hist.cols + first_black, 0, last_black-first_black);
			else
				memset(hist.data + j * hist.cols + first_black, 0, hist.cols);
		}
	}
	//
	threshold(hist, hist, 128, 255, THRESH_BINARY);
	int *sum_h = new int[hist.rows];
	int *sum_v = new int[hist.cols];
	calcHorizontalProjection(hist, sum_h, OCR_PIXEL_MAX);
	calcVerticalProjection(hist, sum_v, OCR_PIXEL_MAX);
	if (!isMessage)
		if (!isMessage &&
			sum_h[0] == 0 &&
			sum_h[hist.rows - 1] == 0 &&
			sum_v[0] == 0 &&
			sum_v[hist.cols - 1] == 0)
			isMessage = true;
		else
			isMessage = false;
	delete[]sum_h;
	delete[]sum_v;

	//
#endif
	//apply mask to in
	int h_ratio = in.rows / hist.rows,
		w_ratio = in.cols / hist.cols;

	memset(gray.data, 255, h_ratio * in.cols);
	memset(gray.data + (in.rows - h_ratio) * in.cols, 255, h_ratio* in.cols);

	for (int j = 0; j < hist.rows; j++)
	{
		memset(gray.data + j * h_ratio * gray.cols, 255, w_ratio);
		memset(gray.data + h_ratio * gray.cols + (in.cols - w_ratio), 255, w_ratio);
		for (int i = 0; i < hist.cols; i++)
		{
			if (*hist.ptr(j, i) < 128)
			{
				for (int k = 0; k < h_ratio; k++)
					memset(gray.data + (j * h_ratio + k) * in.cols + i * w_ratio, 255, w_ratio);
			}
		}
	}

	return gray;
}
/////////////////////////////////////////////
Mat g_ori;
Mat g_gray;
std::vector<cv::Rect> g_ROI_List;
JNIEXPORT void JNICALL Java_com_dynamsoft_tessocr_OCRActivity_locatestring(JNIEnv * env, jobject obj, jint width, jint height, jbyteArray original, jint rectsize, jintArray rectlist)
{
	jbyte *oriData = (jbyte*) (env->GetByteArrayElements(original, 0));
	jint *rects = (jint*) (env->GetIntArrayElements(rectlist, 0));
	//1.Load Image
	OCRTRACE;
	Mat in = Mat(height,width,CV_8UC4,oriData),bgr,gray,thr;
	in.copyTo(g_ori);
	cvtColor(in, bgr, CV_RGBA2BGR);
	cvtColor(bgr, g_gray, CV_BGRA2GRAY);

	gray = estimateType(bgr);
	//4.binarize
	OCRTRACE;
	threshold(gray, thr, 128, 255, THRESH_BINARY_INV);
	//5.locate
	std::vector<cv::Rect> ROI_List;
	getRectList(ROI_List, thr);
	//6.show
	//7.rectlist->rects
	OCRTRACE;
	int j = 0;
	float ratio_h = (height / 600.0);
	float ratio_w = (width / 300.0);
	rects[0] = ROI_List.size() * 4;
	g_ROI_List.clear();
#define _MARGIN_ 4
	for(int i = 0; i < ROI_List.size(); i++)
	{
		j = i * 4 + 1;
		rects[j] = (ROI_List[i].x - _MARGIN_)* ratio_w;
		rects[j + 1] = (ROI_List[i].y - _MARGIN_) * ratio_h;
		rects[j + 2] = (ROI_List[i].width + 2 * _MARGIN_) * ratio_w;
		rects[j + 3] = (ROI_List[i].height + 2 * _MARGIN_) * ratio_h;
		rects[j] -= (rects[j] % 4);
		rects[j + 1] -= (rects[j + 1] % 4);
		rects[j + 2] += (4 - (rects[j + 2] % 4));
		rects[j + 3] += (4 - (rects[j + 3] % 4));
		g_ROI_List.push_back(cv::Rect(rects[j], rects[j + 1], rects[j + 2], rects[j + 3]));
	}
	OCRChkInfo("rects[0] = %d",rects[0]);
    env->SetByteArrayRegion(original,0, width*height*4,oriData);
    env->ReleaseByteArrayElements(original, oriData, 0);
    env->SetIntArrayRegion(rectlist,0, rectsize, rects);
    env->ReleaseIntArrayElements(rectlist, rects, 0);
}
//deprecated - need to add margin-aware module to this function
JNIEXPORT void JNICALL Java_com_dynamsoft_tessocr_OCRActivity_concatstring(JNIEnv * env, jobject obj, jint width, jint height, jbyteArray original)
{
	jbyte *oriData = (jbyte*) (env->GetByteArrayElements(original, 0));
	//1.Load Image
	OCRChkInfo("single(width,height)= %d,%d",width,height);
	OCRChkInfo("orig(width,height)= %d,%d",g_ori.cols,g_ori.rows);
	Mat tmp = Mat(height,width,CV_8UC1);
	int offset_x = 0;
	OCRTRACE;
	for (int i = 0; i < g_ROI_List.size(); i++)
	{
		for (int h = 0; h < g_ROI_List[i].height ; h++)
		{
			memcpy(tmp.data + h * width + offset_x, g_gray.data + (g_ROI_List[i].y + h) * g_gray.cols + g_ROI_List[i].x, g_ROI_List[i].width);
		}
		offset_x += g_ROI_List[i].width;
	}
	Mat bgra;
	cvtColor(tmp, bgra, CV_GRAY2BGRA);
	memcpy(oriData, bgra.data, width*height*4);
	OCRTRACE;
    env->SetByteArrayRegion(original,0, width*height*4,oriData);
    env->ReleaseByteArrayElements(original, oriData, 0);
}

JNIEXPORT void JNICALL Java_com_dynamsoft_tessocr_OCRActivity_getsinglestringrect(JNIEnv * env, jobject obj, jint width, jint height, jint margin, jint index, jbyteArray original)
{
	jbyte *oriData = (jbyte*) (env->GetByteArrayElements(original, 0));
	//1.Load Image
	OCRTRACE;
	Mat tmp = Mat(height,width,CV_8UC1);
	OCRChkInfo("g_gray(width,height)= %d,%d",g_gray.cols,g_gray.rows);
	//initialize
	memset(tmp.data,255, width * height);
	//
	for (int h = 0; h < g_ROI_List[index].height ; h++)
	{
		memcpy(tmp.data + h * width + margin, g_gray.data + (g_ROI_List[index].y + h) * g_gray.cols + g_ROI_List[index].x, g_ROI_List[index].width);
	}
	Mat thr_;
	threshold(tmp, thr_, 0, 255, CV_THRESH_OTSU + CV_THRESH_BINARY);

	memcpy(oriData, thr_.data, width*height);
    env->SetByteArrayRegion(original,0, width*height,oriData);

    env->ReleaseByteArrayElements(original, oriData, 0);
}
}
