#ifndef CSETPOINTS_H_
#define CSETPOINTS_H_
#include "common.h"
#include <vector>
#include <opencv2\opencv.hpp>

class SetPoints
{
private:
	static float ratio;
	static void on_mouse(int event, int x, int y, int flags, void* _pts);
	void printInfo();
public:
	std::vector <cv::Point> SrcPoints;
	std::vector <cv::Point> RefPoints;
	static void SPresize(cv::Mat &img, cv::Size size = (cv::Size(800, 600)), bool keepRatio = true);
	static cv::Rect getRect(std::vector<cv::Point> &vert);
	static void convertToRegion(cv::Mat &src,
		cv::Mat &ref,
		std::vector<cv::Point> &srcPts,
		std::vector<cv::Point> &refPts,
		cv::Rect &srcR,
		cv::Rect &refR
		);
	void savePoints(std::vector<cv::Point> &pts, std::string fileName);
	void loadPoints(std::vector<cv::Point> &pts, std::string fileName);
	void drawPoints(std::vector <cv::Point>&_pts, cv::Mat &_outputImg);
	void drawGrid( cv::Mat &_outputImg, int _xBlock, int _yBlock);
	int run(cv::Mat leftImage, cv::Mat rightImage,std::string srcFile,std::string refFile);
	int run(cv::Mat Image, std::string Filename);
};
#endif