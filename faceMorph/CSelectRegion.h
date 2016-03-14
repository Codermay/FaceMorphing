#ifndef CSELECTREGION_H_
#define CSELECTREGION_H_
#include "common.h"
#include <opencv2\opencv.hpp>
#include <opencv2\core\core.hpp>
#include <vector>

class SelectRegoin
{
private:
	void printInfo();
public:
	void idxToPoints(std::vector<cv::Point>&pList, std::vector<cv::Point>&dst, std::vector<int>&idxList);
	cv::Rect getRect(std::vector<cv::Point> &vert);
	void getMask(std::vector<cv::Point> &vert, cv::Mat &img, cv::Mat &mask);
	void setRegions(std::vector<cv::Point>&pList, std::vector<std::vector<cv::Point>>&lists);
};
#endif