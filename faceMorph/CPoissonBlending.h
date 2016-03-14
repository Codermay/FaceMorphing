// Poisson Blending
// Using stupid way (just solving Ax = b)
// Author: Eric Yuan
// Blog: http://eric-yuan.me
// You are free to use the following code for ANY purpose.
//
// You can improve it by using advanced methods,
// About solving discrete Poisson Equation using Jacobi, SOR, Conjugate Gradients, and FFT
// see here: http://www.cs.berkeley.edu/~demmel/cs267/lecture24/lecture24.html
//
#ifndef CPOISSONBLENDING_H_
#define CPOISSONBLENDING_H_
#include "common.h"
#include "opencv2/core/core.hpp"
#include <vector>
#include <iostream>
#include "math.h"
class PoissonBlending
{
private:
	// calculate horizontal gradient, img(i, j + 1) - img(i, j)
	cv::Mat getGradient(cv::Mat &img);
	bool PoissonBlending::isAdjacent(cv::Mat &roiMask, std::vector<cv::Point> &Points, int i, int j);
	// get Matrix A.
	cv::Mat getA(cv::Mat &roiMask, std::vector<cv::Point> &Points);
	// Get the following Laplacian matrix
	// 0  1  0
	// 1 -4  1
	// 0  1  0
	cv::Mat getLaplacian();
	// Calculate b
	// using convolution.
	cv::Mat getB1(cv::Mat &img1, cv::Mat &img2, cv::Mat &roiMask, std::vector<cv::Point>&Points, cv::Point loc, cv::Rect ROI);
	// Calculate b
	// using getGradient functions.
	cv::Mat getB2(cv::Mat &img1, cv::Mat &img2, cv::Mat &roiMask, std::vector<cv::Point>&Points, cv::Point loc, cv::Rect ROI);
	// Calculate b
	// using Gradients Mixing.
	cv::Mat getB3(cv::Mat &img1, cv::Mat &img2, cv::Mat &roiMask, std::vector<cv::Point>&Points, cv::Point loc, cv::Rect ROI);
	// Solve equation and reshape it back to the right height and width.
	cv::Mat getResult(cv::Mat &A, cv::Mat &B);
public:
	// img1: 3-channel image, we wanna move something in it into img2.
	// img2: 3-channel image, dst image.
	// ROI: the position and size of the block we want to move in img1.
	// posX, posY: where we want to move the block to in img2
	void poisson_blending(cv::Mat &src, cv::Mat &dst, cv::Mat &result, cv::Mat &roiMask, cv::Rect roi, cv::Point loc);
};
#endif