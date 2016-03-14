#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <vector>
#include <iostream>
#include <omp.h>
#include "math.h"
#include "CPoissonBlending.h"
using namespace cv;
using namespace std;
#define ATF at<float>
#define ATU at<uchar>
enum  GRADIENT_TYPE
{
	UP,
	DOWN,
	LEFT,
	RIGHT
};
Mat PoissonBlending::getGradient(Mat &img)
{
	int height = img.rows;
	int width = img.cols;
	Mat img_rep = repeat(img, 2, 2);
	Rect roi[4];
	Mat roiMat[4];
	Mat gradient[4];
	// img(i-1, j) - img(i,j) up
	roi[UP] = Rect(0, height - 1, width, height);
	roiMat[UP] = img_rep(roi[UP]);
	gradient[UP] = roiMat[UP] - img;
	// img(i+1, j) - img(i,j) down
	roi[DOWN] = Rect(0, 1, width, height);
	roiMat[DOWN] = img_rep(roi[DOWN]);
	gradient[DOWN] = roiMat[DOWN] - img;
	// img(i, j-1) - img(i,j) left
	roi[LEFT] = Rect(width - 1, 0, width, height);
	roiMat[LEFT] = img_rep(roi[LEFT]);
	gradient[LEFT] = roiMat[LEFT] - img;
	// img(i, j + 1) - img(i,j) right
	roi[RIGHT] = Rect(1, 0, width, height);
	roiMat[RIGHT] = img_rep(roi[RIGHT]);
	gradient[RIGHT] = roiMat[RIGHT] - img;

	return gradient[UP] + gradient[DOWN] + gradient[LEFT] + gradient[RIGHT] ;
}
bool PoissonBlending::isAdjacent(Mat &roiMask, vector<Point> &Points, int i, int j)
{
	Point c = Points[i] - Points[j];
	if ((c.x*c.x + c.y*c.y) == 1)
		return true;
	return false;
}
// get Matrix A.
Mat PoissonBlending::getA(Mat &roiMask,vector<Point> &Points)
{
	Mat A = Mat::eye(Points.size(), Points.size(), CV_32FC1);
	A *= -4;
	#pragma omp parallel for
	for (int i = 0; i < A.rows; i++)
	{
		#pragma omp parallel for
		for (int j = 0; j < A.cols; j++)
		{
			if (i == j)
				A.ATF(i, j) = -4.0;
			else if (isAdjacent(roiMask,Points, i, j))
			{
				A.ATF(i, j) = 1.0;
			}
		}
	}
	return A;
}
// Get the following Laplacian matrix
// 0  1  0
// 1 -4  1
// 0  1  0
Mat PoissonBlending::getLaplacian()
{
	Mat lap = Mat::zeros(3, 3, CV_32FC1);
	lap.ATF(0, 1) = 1.0;
	lap.ATF(1, 0) = 1.0;
	lap.ATF(1, 2) = 1.0;
	lap.ATF(2, 1) = 1.0;
	lap.ATF(1, 1) = -4.0;
	return lap;
}
// Calculate b
// using convolution.
Mat PoissonBlending::getB1(Mat &src, Mat &dst, Mat &roiMask,vector<Point>&Points,Point loc, Rect ROI)
{
	Mat lapMat;
	int mh = roiMask.rows;
	int mw = roiMask.cols;
	filter2D(src, lapMat, -1, getLaplacian());
	Mat B = Mat::zeros(Points.size(), 1, CV_32FC1);

	for (int i = 0; i < Points.size(); i++)
	{
		Point pt = Points[i];
		double tmp = 0.0;
		tmp = lapMat.ATF(ROI.y + pt.y, ROI.x + pt.x);
		if (pt.y>0 && roiMask.ATU(pt.y - 1, pt.x) != 255)
			tmp -= dst.ATF(pt.y + loc.y - 1, pt.x + loc.x);
		if (pt.y+1 <mh && roiMask.ATU(pt.y + 1, pt.x) != 255)
			tmp -= dst.ATF(pt.y + loc.y + 1, pt.x + loc.x);
		if (pt.x + 1<mw && roiMask.ATU(pt.y, pt.x + 1) != 255)
			tmp -= dst.ATF(pt.y + loc.y, pt.x + loc.x + 1);
		if (pt.x>0 && roiMask.ATU(pt.y , pt.x-1) != 255)
			tmp -= dst.ATF(pt.y + loc.y, pt.x + loc.x - 1);
		B.ATF(i, 0) = tmp;
	}
	return B;
}
// Calculate b
// using getGradient functions.
Mat PoissonBlending::getB2(Mat &src, Mat &dst, Mat &roiMask, vector<Point>&Points, Point loc, Rect ROI)
{
	Mat gradMat = getGradient(src);
	int mh = roiMask.rows;
	int mw = roiMask.cols;
	Mat B = Mat::zeros(Points.size(), 1, CV_32FC1);

	for (int i = 0; i < Points.size(); i++)
	{
		Point pt = Points[i];
		double tmp = 0.0;
		tmp = gradMat.ATF(ROI.y + pt.y, ROI.x + pt.x);
		if (pt.y>0 && roiMask.ATU(pt.y - 1, pt.x) != 255)
			tmp -= dst.ATF(pt.y + loc.y - 1, pt.x + loc.x);
		if (pt.y+1<mh && roiMask.ATU(pt.y + 1, pt.x) != 255)
			tmp -= dst.ATF(pt.y + loc.y + 1, pt.x + loc.x);
		if (pt.x + 1<mw && roiMask.ATU(pt.y, pt.x + 1) != 255)
			tmp -= dst.ATF(pt.y + loc.y, pt.x + loc.x + 1);
		if (pt.x>0 && roiMask.ATU(pt.y , pt.x-1) != 255)
			tmp -= dst.ATF(pt.y + loc.y, pt.x + loc.x - 1);
		B.ATF(i, 0) = tmp;
	}
	return B;
}
// Calculate b
// using Gradients Mixing.
Mat PoissonBlending::getB3(Mat &src, Mat &dst, Mat &roiMask, vector<Point>&Points, Point loc, Rect ROI)
{
	Mat gradMat = getGradient(src) + getGradient(dst);
	int mh = roiMask.rows;
	int mw = roiMask.cols;
	Mat B = Mat::zeros(Points.size(), 1, CV_32FC1);
	
	for (int i = 0; i < Points.size(); i++)
	{
		Point pt = Points[i];
		double tmp = 0.0;
		tmp = gradMat.ATF(ROI.y + pt.y, ROI.x + pt.x);
		if (pt.y>0 && roiMask.ATU(pt.y - 1, pt.x) != 255)
			tmp -= dst.ATF(pt.y + loc.y - 1, pt.x + loc.x);
		if (pt.y + 1<mh && roiMask.ATU(pt.y + 1, pt.x) != 255)
			tmp -= dst.ATF(pt.y + loc.y + 1, pt.x + loc.x);
		if (pt.x + 1<mw && roiMask.ATU(pt.y, pt.x + 1) != 255)
			tmp -= dst.ATF(pt.y + loc.y, pt.x + loc.x + 1);
		if (pt.x>0 && roiMask.ATU(pt.y, pt.x - 1) != 255)
			tmp -= dst.ATF(pt.y + loc.y, pt.x + loc.x - 1);
		B.ATF(i, 0) = tmp;
	}
	return B;
}
// Solve equation and reshape it back to the right height and width.
Mat PoissonBlending::getResult(Mat &A, Mat &B)
{
	Mat result;
	solve(A, B, result);
	return  result;
}
// img1: 3-channel image, we wanna move something in it into img2.
// img2: 3-channel image, dst image.
// ROI: the position and size of the block we want to move in img1.
// posX, posY: where we want to move the block to in img2
void PoissonBlending::poisson_blending(Mat &src, Mat &dst, Mat &result, Mat &roiMask, Rect roi, Point loc)
{
	cout << "Poisson Blender" << endl;
	src.convertTo(src, CV_32FC3);
	src /= 255;
	dst.convertTo(dst, CV_32FC3);
	dst /= 255;

	vector<Point> Points;
#pragma omp parallel for
	for (int i = 0; i < roi.height; i++)
	{
#pragma omp parallel for
		for (int j = 0; j < roi.width; j++)
		{
			int z=roiMask.ATU(i, j);
			if (roiMask.ATU(i, j) == 255)
			{
#pragma omp critical(ppb)
		{
			Points.push_back(Point(j, i));
		}
			}
		} 
	}
	cout << "Total Points: " << Points.size()<< endl;
	cout << "Calculating Matrix A" << endl;
	Mat A = getA(roiMask, Points);

	// we must do the poisson blending to each channel.
	vector<Mat> src_rgb;
	split(src, src_rgb);
	vector<Mat> dst_rgb;
	split(dst, dst_rgb);
	vector<Mat> res_v;
	res_v.resize(3);
	cout << "Calculating B" << endl;
	// process 3 channels

	#pragma omp parallel for schedule(dynamic, 1) 
	for (int i = 0; i < 3; i++)
	{
		#pragma omp critical(cout)
		cout << "  " << i + 1 << "/3 channel processing..." << endl;
		Mat res, b;
		b = getB2(src_rgb[i], dst_rgb[i], roiMask, Points, loc, roi);
		//b = getB1(src_rgb[i], dst_rgb[i], roiMask, Points, loc, roi);
		#pragma omp critical(cout)
		res = getResult(A, b);
		#pragma omp critical(res_v)
			res_v[i]=res; 
		#pragma omp critical(cout)
			cout << "  "<< i + 1 << "/3 channel finished!!!" << endl;

	}
	Mat res_m;
	// merge the 3 gray images into a 3-channel image
	cout << "Merging result" << endl;
	merge(res_v, res_m);
	dst.copyTo(result);
	#pragma omp parallel for
	for (int i = 0; i < res_m.rows; i++)
	{
		Point p = Points[i]+Point(roi.x,roi.y);
		result.at<Vec3f>(p) = res_m.at<Vec3f>(i, 0);
	}

}
/*
int
main(int argc, char** argv)
{
long start, end;
Mat img1, img2;
Mat in1 = imread("face.png");
Mat in2 = imread("lisa.png");
imshow("src", in1);
imshow("dst", in2);
in1.convertTo(img1, CV_64FC3);
in2.convertTo(img2, CV_64FC3);
Rect rc = Rect(25, 40, 80, 20);
Mat result = poisson_blending(img1, img2, rc, 15, 50);
result.convertTo(result, CV_32FC1);
Rect rc2 = Rect(15, 50, 80, 20);
Mat roimat = in2(rc2);
result.copyTo(roimat);
imshow("roi", result);
imshow("result", in2);
waitKey(0);
return 0;
}
*/