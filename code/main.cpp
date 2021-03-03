#include<opencv2/opencv.hpp>
#include<iostream>
#include "GetThreshold.h"

/***********************该算法实现框选ROI自动调阈值****************************/
/***********************第一个ROI框选所要调阈值的区域**************************/
/******第二个区域框选所要判断的区域（可以为整副图像但是时间复杂度很高）*******/

int main(void)
{
	//打印使用要求
	std::cout<<"\n";
	std::cout<<"/*********************************************************/\n";
	std::cout<<"/**********支持文件格式：jpg、png、jpeg、avi、mp4*********/\n";
	std::cout<<"/*******本程序可以实现图像或视频第一帧的自动调整阈值******/\n";
	std::cout<<"/***************最终会有hsv的阈值以及效果图***************/\n";
	std::cout<<"/*******需要你输入文件绝对路径和框选两个框****************/\n";
	std::cout<<"/**************文件路径要绝对路径且不需要引号*************/\n";
	std::cout<<"/*********第一个框需要基本覆盖你所要调整阈值的东西********/\n";
	std::cout<<"/*****************第二个框要包含第一个框******************/\n";
	std::cout<<"/*****不包含第一个框同样颜色的东西 包含其他颜色的东西 ****/\n";
	std::cout<<"/*********************************************************/\n";
	
	//读取路径
	std::cout<<"(Ctrl+C退出)  File Path:";
	std::string path;
	getline(std::cin,path);

	//读取视频流第一帧或者图像 自动按比缩放至宽为320 也可以自己指定ReadFrame的width参数
	cv::Mat frame;
	while(!ReadFrame(frame,path,320))
	{
		std::cout<<"文件名输入错误，请按要求输入\n";
		std::cout<<"(Ctrl+C退出)  File Path:";
		getline(std::cin,path);
	}

	//读取偏移量
	// std::cout<<"(Ctrl+C退出)  阈值偏移预估值的范围(建议为40):";
	// std::string offsetStr;
	// getline(std::cin,offsetStr);

	// //判断是否为数字
	// while(!isNum(offsetStr))
	// {
	// 	std::cout<<"请输入数字\n";
	// 	std::cout<<"(Ctrl+C退出)  阈值偏移预估值的范围(建议为40):";
	// 	getline(std::cin,offsetStr);
	// }
	// int offset =stoi(offsetStr);
	int offset =40;
		
	bool end = false;
	while(!end)
	{
		std::cout<<"\n";
		//框选第一个ROI 为我们所需调阈值区域
		cv::Rect ROI_Rect = cv::selectROI("SelectROI",frame,0,0);
		cv::destroyWindow("SelectROI");

		//框选第二个区域 为我们所需判断的区域 可以为整幅图像但是时间复杂度很高
		cv::Rect Judge_Rect = cv::selectROI("SelectJudgeArea",frame,0,0);
		cv::destroyWindow("SelectJudgeArea");

		//没框选则返回
		if(ROI_Rect.empty() ||Judge_Rect.empty())
		{
			std::cout<<"Esc或者x掉不会关闭程序 Ctrl+c可以退出 或者重新框选"<<std::endl;
			continue;
		}

		// 获得ROI的HSV用于获得hsv直方图
		cv::Mat ROI =frame(ROI_Rect);
		cv::Mat ROI_hsv;
		cv::cvtColor(ROI,ROI_hsv,cv::COLOR_BGR2HSV);

		//分离hsv三通道
		cv::Mat h,s,v;
		std::vector<cv::Mat> HsvVec;
		split(ROI_hsv,HsvVec);
		h =HsvVec.at(0);
		s =HsvVec.at(1);
		v =HsvVec.at(2);

		//通过kmeans获得预估值 graphcast
		int EstimatedHmin =KmeansGetThreshold(h);
		int EstimatedSmin =KmeansGetThreshold(s);
		int EstimatedVmin =KmeansGetThreshold(v);

		// //通过hsv直方图获得hsv最小值的预估值
		// int EstimatedHmin=FindPixelMost(ROI_hsv,0,5);
		// int EstimatedSmin=FindPixelMost(ROI_hsv,1,5);
		// int EstimatedVmin=FindPixelMost(ROI_hsv,2,5);

		//打印预估值
		std::cout<<"\n";
		std::cout<<"Estimate:";
		std::cout<<"hmin:"<<EstimatedHmin<<"," <<"smin:"<<EstimatedSmin<<","  <<"vmin:"<<EstimatedVmin<<std::endl;
		
		//找hsv最小值
		std::cout<<"\n";
		std::cout<<"Finding HsvMin...\n";
		std::vector<int> HsvMin(3);
		if(!FindHsvMin(frame,ROI_Rect,Judge_Rect,HsvMin,EstimatedHmin,EstimatedSmin,EstimatedVmin,(offset>>1)))
		{
			continue;
		}

		//打印输出
		std::cout<<"Final:";
		std::cout<<"hmin:"<<HsvMin[0]<<"," <<"smin:"<<HsvMin[1]<<","  <<"vmin:"<<HsvMin[2]<<std::endl;

		//找hsv最大值
		std::cout<<"\n";
		std::cout<<"Finding HsvMax...\n";
		std::vector<int> HsvMax(3);
		if(!FindHsvMax(frame,ROI_Rect,Judge_Rect,HsvMax,HsvMin[0],HsvMin[1],HsvMin[2],offset))
		{
			continue;
		}

		//打印输出
		std::cout<<"Final:";
		std::cout<<"hmax:"<<HsvMax[0]<<"," <<"smax:"<<HsvMax[1]<<"," <<"vmax:"<<HsvMax[2]<<std::endl;

		//将获得的阈值作用到图片上 
		cv::Mat hsv,threshold;
		cv::cvtColor(frame,hsv,cv::COLOR_BGR2HSV);
		cv::inRange(hsv,cv::Scalar(HsvMin[0],HsvMin[1],HsvMin[2]),cv::Scalar(HsvMax[0],HsvMax[1],HsvMax[2]),threshold);

		//将两幅图按高合并（横向合并）
		cv::Mat outputImage = cv::Mat::zeros(frame.rows, frame.cols*2, frame.type());
		if(HeightMerge(frame,threshold,outputImage))
		{
			cv::imshow("Show",outputImage);
			cv::waitKey(0);
		}
		else
		{
			continue;
		}
		
		end = true;
	}

	return 0;
}




