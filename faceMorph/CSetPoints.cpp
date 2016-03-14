#include <opencv2\opencv.hpp>
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include "CSetPoints.h"
using namespace std;
using namespace cv;
float SetPoints::ratio;
void SetPoints::printInfo()
{
	cout << "----------------------------------------------------------------" << endl;
	cout << "               Thin Plate Spline Point Setting." << endl;
	cout << "|Mouse:    +LeftClick: Add a point    +RightClick: Delete last point." << endl;
	cout << "|Keyboard: +'s' save points setting.  +'l' load points setting." << endl;
	cout << "|          +'e' save and exit setting.+'esc' exit the program." << endl;
	cout << "----------------------------------------------------------------" << endl;
}

void SetPoints::on_mouse(int event, int x, int y, int flags, void* _pts)
{
	vector <cv::Point>* pts;
	pts = (vector <cv::Point>*)_pts;
	if (event == CV_EVENT_LBUTTONDOWN){
		cout << "Point >> " << x / ratio << ' ' << y / ratio << endl;

		cv::Point tmp = cv::Point(x / ratio, y / ratio);
		pts->push_back(tmp);
	}
	if (event == CV_EVENT_RBUTTONDOWN){
		if (pts->size()>0)
			pts->pop_back();
	}
}

void SetPoints::savePoints(std::vector<cv::Point> &pts,string fileName)
{
	ofstream lf;
	lf.open(fileName + ".pts", ios::out);


	for (int j = 0; j < pts.size(); j++){
		lf << "p " << pts.at(j).x << " " << pts.at(j).y << "\n";
	}
	
	lf.close();
	cout << "points saved! " << endl;
}
void SetPoints::loadPoints(std::vector<cv::Point> &pts, string fileName)
{
	pts.clear();
	FILE* lf;
	fopen_s(&lf, (fileName + ".pts").c_str(), "r");
	cv::Point tmp;
	if (lf == NULL)return;
	while (!feof(lf)){
		double x, y;
		int re;
		re = fscanf_s(lf, "p %lf %lf \n", &x, &y);
		if (re != 2)break;
		tmp.x = (int)x;
		tmp.y = (int)y;
		pts.push_back(tmp);
	}
	fclose(lf);
	cout << "done " << endl;
}
void SetPoints::drawPoints(vector <cv::Point>&_pts, cv::Mat &_outputImg)
{
	int i = 0;
	while (i < _pts.size()){
		cv::Scalar s = cv::Scalar(0 + rand() % 255, 0 + rand() % 255, 0 + rand() % 255);
		s = cv::Scalar(240, 0 , 0);
		cv::Point tmpl = cv::Point(_pts.at(i).x, _pts.at(i).y);
		cv::circle(_outputImg, tmpl, 1, s, 1);
		cv::putText(_outputImg, to_string(i+1), tmpl, 1, 1.0, s, 1, 8, false);
		i++;
	}
}
void SetPoints::drawGrid( cv::Mat &_outputImg, int _xBlock,int _yBlock)
{
	cv::Scalar s = cv::Scalar(102, 255, 51);
	for (int i =1; i < _xBlock; i++)
	{
		int x = (int)(_outputImg.size().width*i / _xBlock);
		cv::Point p1 = cv::Point(x,0);
		cv::Point p2 = cv::Point(x,_outputImg.size().height);
		cv::line(_outputImg, p1, p2, s, 0.1, 8, 0);
	}
	for (int i = 1; i < _yBlock; i++)
	{
		int y = (int)(_outputImg.size().height*i / _yBlock);
		cv::Point p1 = cv::Point(0, y);
		cv::Point p2 = cv::Point(_outputImg.size().width, y);
		cv::line(_outputImg, p1, p2, s, 0.1, 8, 0);
	}
}
int SetPoints::run(cv::Mat leftImage, cv::Mat rightImage, std::string srcFile, std::string refFile)
{
	ratio = 1.0;
	printInfo();
	loadPoints(SrcPoints, srcFile);
	loadPoints(RefPoints, refFile);
	cv::namedWindow("source", 1);
	cv::moveWindow("source", 10, 10);
	cv::namedWindow("reference", 1);
	cv::moveWindow("reference", 1000, 10);
	cv::setMouseCallback("source", on_mouse, &SrcPoints);
	cv::setMouseCallback("reference", on_mouse, &RefPoints);

	while (1){

		cv::Mat tmpSrcImage, tmpRefImage;

		leftImage.copyTo(tmpSrcImage);
		rightImage.copyTo(tmpRefImage);

		drawPoints(SrcPoints, tmpSrcImage);
		drawPoints(RefPoints, tmpRefImage);
		//drawGrid(tmpSrcImage, 15, 15);
		//drawGrid(tmpRefImage, 15, 15);
		resize(tmpSrcImage, tmpSrcImage, Size(leftImage.cols*ratio, leftImage.rows*ratio));
		resize(tmpRefImage, tmpRefImage, Size(rightImage.cols*ratio, rightImage.rows*ratio));
		cv::imshow("source", tmpSrcImage);
		cv::imshow("reference", tmpRefImage);

		int c = cvWaitKey(100);

		if (c == 27)
		{
			exit(-1);
		}
		else if (c == 'g')
		{
			ratio += 0.25;
		}
		else if (c == 'h')
		{
			ratio -= 0.25;
			if (ratio < 0.25) ratio = 0.25;
		}
		else if (c == 's'){    // load saved points from files
			cout << "save points " << endl;
			savePoints(SrcPoints, srcFile);
			savePoints(RefPoints, refFile);
		}
		else if (c == 'l'){
			cout << "load points " << endl;
			loadPoints(SrcPoints, srcFile);
			loadPoints(RefPoints, refFile);
		}
		else if (c == 'e'){
			if (SrcPoints.size() != RefPoints.size())
			{
				cout << "Each image should have the same number of points!";
			}
			else
			{
				savePoints(SrcPoints, srcFile);
				savePoints(RefPoints, refFile);
				break;
			}
		}
	}
	cv::destroyAllWindows();
	return 0;
}
int SetPoints::run(cv::Mat leftImage, std::string fileName)
{
	ratio = 1.0;
	printInfo();

	loadPoints(SrcPoints, fileName);

	cv::namedWindow("Set_Points", 1);
	cv::moveWindow("Set_Points", 10, 10);
	cv::setMouseCallback("Set_Points", on_mouse, &SrcPoints);

	while (1){

		cv::Mat tmpSrcImage;

		leftImage.copyTo(tmpSrcImage);

		drawPoints(SrcPoints, tmpSrcImage);
		drawGrid(tmpSrcImage, 15, 15);
		resize(tmpSrcImage, tmpSrcImage, Size(leftImage.cols*ratio, leftImage.rows*ratio));
		cv::imshow("Set_Points", tmpSrcImage);

		int c = cvWaitKey(100);

		if (c == 27)
		{
			exit(-1);
		}
		else if (c == 'g')
		{
			ratio += 0.25;
		}
		else if (c == 'h')
		{
			ratio -= 0.25;
			if (ratio < 0.25) ratio = 0.25;
		}
		else if (c == 's'){    // load saved points from files
			cout << "save points " << endl;
			savePoints(SrcPoints, fileName);
		} 
		else if (c == 'l'){
			cout << "load points " << endl;
			loadPoints(SrcPoints, fileName);
		}
		else if (c == 'e'){
				savePoints(SrcPoints, fileName);
				break;
		}
	}
	cv::destroyAllWindows();
	return 0;
}
void SetPoints::SPresize(cv::Mat &img, cv::Size size, bool keepRatio)
{
	float ratio = 1;
	int maxCols = size.width, maxRows = size.height;
	if (img.cols < 10 || img.rows < 10)
		Error("Invalid image file");
	if (img.cols>maxCols)
		ratio = float(img.cols) / maxCols;
	if (img.rows>maxRows)
		ratio = max(ratio, float(img.rows) / maxRows);
	int dstCols = img.cols / ratio;
	int dstRows = img.rows / ratio;
	if (keepRatio)
		cv::resize(img, img, cv::Size(dstCols, dstRows));
	else
		cv::resize(img, img, size);
}
Rect SetPoints::getRect(vector<Point> &vert)
{
	Rect re;
	int maxX, maxY, minX, minY;
	for (int i = 0; i < vert.size(); i++)
	{
		if (i == 0)
		{
			maxX = vert[i].x;
			minX = maxX;
			maxY = vert[i].y;
			minY = maxY;
		}
		maxX = max(maxX, vert[i].x);
		maxY = max(maxY, vert[i].y);
		minX = min(minX, vert[i].x);
		minY = min(minY, vert[i].y);
	}
	return Rect(minX, minY, max(1, (maxX - minX + 1)), max(1, (maxY - minY + 1)));
}

void SetPoints::convertToRegion(Mat &src,
	Mat &ref,
	std::vector<cv::Point> &srcPts,
	std::vector<cv::Point> &refPts,
	Rect &srcR,
	Rect &refR
	)
{
	src.convertTo(src, ref.type());
	Mat t_src, t_ref;
	srcR = getRect(srcPts);
	refR = getRect(refPts);
	Rect t_srcR = srcR, t_refR = refR;
	t_srcR.x = 0;
	t_srcR.y = 0;
	t_refR.x = 0;
	t_refR.y = 0;
	t_src = Mat(max(srcR.height, refR.height), max(srcR.width, refR.width), src.type());
	t_ref = Mat(refR.height, refR.width, ref.type());
	
	src(srcR).copyTo(t_src(t_srcR));
	ref(refR).copyTo(t_ref(t_refR));
	src = t_src;
	ref = t_ref;
}