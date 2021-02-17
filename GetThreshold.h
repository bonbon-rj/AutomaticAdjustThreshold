#pragma once 
#include <iostream>
#include <opencv2/opencv.hpp>
#include <regex>
#include <time.h>
//定义排序结构体 成员变量为框选ROI中二值化255的点的数量以及对应的hsv值

int Check(int number);
bool isNum(std::string &str);
void PrintProgressBar(int NowValue,int StartValue,int EndValue);
bool FindHsvMax(cv::Mat &frame,cv::Rect &ROI_Rect,cv::Rect &Judge_Rect,std::vector<int> &Hsv,int hmin,int smin,int vmin,int offset);
bool FindHsvMin(cv::Mat &frame,cv::Rect &ROI_Rect,cv::Rect &Judge_Rect,std::vector<int> &Hsv,int hmin,int smin,int vmin,int offset);
int FindPixelMost(cv::Mat &frame,int channel,int num);
bool ReadFrame(cv::Mat &frame,std::string &path,int width=320);
bool HeightMerge(cv::Mat &img1,cv::Mat &img2,cv::Mat &OutputImg);


