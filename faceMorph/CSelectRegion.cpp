#include <opencv2\opencv.hpp>
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include "CSelectRegion.h"
using namespace std;
using namespace cv;
#define ATC at<char>

void SelectRegoin::idxToPoints(vector<Point>&pList, vector<Point>&dst, vector<int>&idxList)
	{
		dst.clear();
		for (int i = 0; i < idxList.size();i++)
		{
			dst.push_back(pList[idxList[i]-1]);
		}
	}
void SelectRegoin::printInfo()
{
	vector<vector<int>> vertLists;
	cout << "------------------------------------------------------" << endl;
	cout << "              Blending block definition" << endl;
	cout << "Please enter the index of the point to define a block." << endl;
	cout << "One line to define a block: 1 2 3 4 5 end" << endl;
	cout << "Use new line to define antoher block." << endl;
	cout << "Type in 'end' to finish block definition." << endl;
	cout << "Please define the first block." << endl;
}
void SelectRegoin::setRegions(std::vector<cv::Point>&pList, std::vector<std::vector<cv::Point>>&lists)
{
	printInfo();
	bool endF = false;
	vector<vector<int>> idxLists;
	while (!endF)
	{
		vector<int> idxList;
		string content;
		while (cin.peek() != '\n')
		{
			if (cin >> content)
			{
				if (content == "end")
				{
					endF = true;
					break;
				}
				int x = atoi(content.c_str());
				if (x > 0 && x <= pList.size())
					idxList.push_back(x);
				else
				{
					cout << "Invalid input: " << content << endl;
					cin.clear();
					cin.ignore(1024, '\n');
					idxList.clear();
					break;
				}
			}		
		}
		if (idxList.size() > 0)
		{
			idxLists.push_back(idxList);
			vector<Point> vertList;
			idxToPoints(pList, vertList, idxList);
			lists.push_back(vertList);
			cout << "Block["<<idxLists.size()<<"] has been created successfully!"<< endl;
		}
		if (idxLists.size() == 0)
		{
			endF = false;
			cout << "Please define at least one block." << endl;
		}
		cin.clear();
		cin.ignore(1024, '\n');
		
	}
}
//sdsd
Rect SelectRegoin::getRect(vector<Point> &vert)
	{
		Rect re;
		int maxX, maxY, minX, minY;
		for (int i = 0; i < vert.size(); i++)
		{
			if (i== 0)
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
		return Rect(minX, minY, max(1,(maxX - minX+1)),max(1,(maxY - minY+1)));
	}
void SelectRegoin::getMask(vector<Point> &vert, Mat &img, Mat &mask)
	{
		
		mask = Mat::zeros(img.size(), CV_8UC1);
		Mat src = Mat::zeros(img.size(), CV_8UC1);
		for (int j = 0; j < vert.size(); j++)
		{
			line(src, vert[j], vert[(j + 1) % vert.size()], Scalar(255), 1, 8);
		}
		vector<vector<Point> > contours; vector<Vec4i> hierarchy;
		Mat src_copy = src.clone();
		findContours(src_copy, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);
		for (int j = 0; j < src.rows; j++)
		    { for (int i = 0; i < src.cols; i++)
				{
					float ppt=(float)pointPolygonTest(contours[0], Point2f((float)i, (float)j), false);
					if (ppt == 0)
						mask.ATC(j, i) = 122;
					else if (ppt>0)
						mask.ATC(j, i) = 255;
					else
						mask.ATC(j, i) = 0;
				}
		    }
	}
