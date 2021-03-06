#include "GetThreshold.h"

/********************************************************
 * @brief 检查是否为负数或大于255 
 * @param number 检查的数字
 * ******************************************************/
int Check(int number)
{
	if(number <0)
	{
		return 0;
	}
	else if(number >255)
	{
		return 255;
	}
	else
	{
		return number;
	}
}
/********************************************************
 * @brief 判断字符串是否为数字
 * @param str 输入的字符串
 * ******************************************************/
bool isNum(std::string &str)
{
	for(int i =0;i<str.size();i++)
	{
		if(!isdigit(str[i]))
		{
			return 0;
		}
	}
	return 1;
}

/********************************************************
 * @brief 打印进度条
 * @param NowValue 当前值
 * @param StartValue 开始值
 * @param EndValue 末尾值
 * ******************************************************/
void PrintProgressBar(int NowValue,int StartValue,int EndValue)
{ 
    int k = static_cast<int>((NowValue-StartValue)*100.0/(EndValue-StartValue));
    int num = static_cast<int>(k/5);
    std::string ProgressBar="|";
    while(num>0)
    {
            ProgressBar+="#";
			num--;
    }
    ProgressBar+="|";
    printf("\rCurrent progress:%s%d%%",ProgressBar.c_str(),k);
    fflush(stdout);
}

/********************************************************
 * @brief 根据得出的hsv最小值迭代寻找hsv最大值
 * @param frame 输入图像
 * @param ROI_Rect 框选的ROI
 * @param Judge_Rect 框选的判断区域
 * @param Hsv 存储最后结果的向量
 * @param hmin hmin 即hmax的初始迭代值
 * @param smin smin 即smax的初始迭代值
 * @param vmin vmin 即vmax的初始迭代值
 * ******************************************************/
bool FindHsvMax(cv::Mat &frame,cv::Rect &ROI_Rect,cv::Rect &Judge_Rect,std::vector<int> &Hsv,int hmin,int smin,int vmin,int offset)
{
    //对图像框选两个区域
    cv::Mat ROI =frame(ROI_Rect);
	cv::Mat JudgeArea =frame(Judge_Rect);

    //转hsv图
	cv::Mat ROI_hsv,Judge_hsv;
    cv::cvtColor(ROI,ROI_hsv,cv::COLOR_BGR2HSV);
	cv::cvtColor(JudgeArea,Judge_hsv,cv::COLOR_BGR2HSV);

    //ROI相对所需判断的区域的相对坐标
	int x = ROI_Rect.tl().x - Judge_Rect.tl().x;
	int y = ROI_Rect.tl().y - Judge_Rect.tl().y;
	if(x <0|| y<0||ROI_Rect.br().x - Judge_Rect.br().x>0||ROI_Rect.br().y - Judge_Rect.br().y>0)
	{
		std::cout<<"第二个框要包含第一个框！请重新框选"<<std::endl;
		return 0;
	}
    //二值化图像以及计数变量
    cv::Mat ROI_threshold,Judge_threshold;
    int cnt=0;
	int cnt_max =0;
	int resultH=0;
	int resultS=0;
	int resultV=0;
	clock_t start,end;
	start =clock();
	for(int h =hmin;h<Check(hmin+offset);h++)
	{
        //打印进度条
        PrintProgressBar(h,hmin,Check(hmin+offset)-1);
		
		for(int s =smin;s<Check(smin+offset);s++)
		{
			for(int v=vmin;v<Check(vmin+offset);v++)
			{
                //二值化
				cv::inRange(ROI_hsv,cv::Scalar(hmin,smin,vmin),cv::Scalar(h,s,v),ROI_threshold);
				cv::inRange(Judge_hsv,cv::Scalar(hmin,smin,vmin),cv::Scalar(h,s,v),Judge_threshold);
                              
                //遍历判断区域
				for(int i =0;i<Judge_threshold.rows;i++)
				{
					uchar* data = Judge_threshold.ptr<uchar>(i);  
					for (int j = 0; j < Judge_threshold.cols * 1; j++) 
					{
                        //如果在ROI中 判断255的个数
						if(j>=x &&j<=x+ROI_Rect.width&&i>=y && i<y+ROI_Rect.height)
						{
							if(data[j] ==255)
							{
								cnt++;
							}
						}
                        //如果不在ROI中 判断0的个数
						else
						{
							if(data[j] ==0)
							{
								cnt++;
							}
						}
					}
				}
                //用找寻最大值方法取代vector排序
				if(cnt>cnt_max)
				{
					cnt_max = cnt;
					resultH =h;
					resultS =s;
					resultV =v;
				}
				cnt =0;
			}
		}
	}
	end =clock();
    std::cout<<"\n耗时："<<(double)(end-start)/CLOCKS_PER_SEC<<" s"<<std::endl;
	Hsv[0] =resultH;
	Hsv[1] =resultS;
	Hsv[2] =resultV;
	return 1;
}


/********************************************************
 * @brief 对hsvmin估计值附近offset范围进行迭代找到最优hsvmin
 * @param frame 输入图像
 * @param ROI_Rect 框选的ROI
 * @param Judge_Rect 框选的判断区域
 * @param Hsv 存储最后结果的向量
 * @param hmin hmin估计值
 * @param smin smin估计值
 * @param vmin vmin估计值
 * @param offset 偏移量
 * ******************************************************/
bool FindHsvMin(cv::Mat &frame,cv::Rect &ROI_Rect,cv::Rect &Judge_Rect,std::vector<int> &Hsv,int hmin,int smin,int vmin,int offset)
{
    //对图像框选两个区域
    cv::Mat ROI =frame(ROI_Rect);
	cv::Mat JudgeArea =frame(Judge_Rect);

    //转hsv图
	cv::Mat ROI_hsv,Judge_hsv;
    cv::cvtColor(ROI,ROI_hsv,cv::COLOR_BGR2HSV);
	cv::cvtColor(JudgeArea,Judge_hsv,cv::COLOR_BGR2HSV);

    //ROI相对所需判断的区域的相对坐标
	int x = ROI_Rect.tl().x - Judge_Rect.tl().x;
	int y = ROI_Rect.tl().y - Judge_Rect.tl().y;

	if(x <0|| y<0||ROI_Rect.br().x - Judge_Rect.br().x>0||ROI_Rect.br().y - Judge_Rect.br().y>0)
	{
		std::cout<<"第二个框要包含第一个框！请重新框选"<<std::endl;
		return 0;
	}
    //二值化图像以及计数变量
    cv::Mat ROI_threshold,Judge_threshold;
    int cnt=0;
	int cnt_max =0;
	int resultH=0;
	int resultS=0;
	int resultV=0;

	clock_t start,end;
	start =clock();
	for(int h =Check(hmin-offset);h<Check(hmin+offset);h++)
	{
        //打印进度条
        PrintProgressBar(h,Check(hmin-offset),Check(hmin+offset)-1);
		
		for(int s =Check(smin-offset);s<Check(smin+offset);s++)
		{
			for(int v=Check(vmin-offset);v<Check(vmin+offset);v++)
			{
                //二值化
				cv::inRange(ROI_hsv,cv::Scalar(h,s,v),cv::Scalar(255,255,255),ROI_threshold);
				cv::inRange(Judge_hsv,cv::Scalar(h,s,v),cv::Scalar(255,255,255),Judge_threshold);

                //遍历判断区域
				for(int i =0;i<Judge_threshold.rows;i++)
				{
					uchar* data = Judge_threshold.ptr<uchar>(i);  
					for (int j = 0; j < Judge_threshold.cols * 1; j++) 
					{
                        //如果在ROI中 判断255的个数
						if(j>=x &&j<=x+ROI_Rect.width&&i>=y && i<y+ROI_Rect.height)
						{
							if(data[j] ==255)
							{
								cnt++;
							}
						}
                        //如果不在ROI中 判断0的个数
						else
						{
							if(data[j] ==0)
							{
								cnt++;
							}
						}
					}
				}
                //用找寻最大值方法取代vector排序
				if(cnt>cnt_max)
				{
					cnt_max = cnt;
					resultH =h;
					resultS =s;
					resultV =v;
				}

				cnt =0;
			}
		}
	}
	end =clock();
    std::cout<<"\n耗时："<<(double)(end-start)/CLOCKS_PER_SEC<<" s"<<std::endl;

	Hsv[0] =resultH;
	Hsv[1] =resultS;
	Hsv[2] =resultV;

	return 1;
}


/********************************************************
 * @brief 输出图像中第几通道中前num的像素值的平均值 适用RGB HSV
 * @param frame 输入图像
 * @param channel 通道数
 * @param num 排前几的像素
 * ******************************************************/
int FindPixelMost(cv::Mat &frame,int channel,int num)
{
	//定义size ranges channels
    int hist_size[] = { 256 };
    float range[] = { 0,256 };
    const float* ranges[] = { range };
	int channels[] = {channel}; 

	//获得直方图
    cv::MatND Hist;
    calcHist(&frame, 1, channels, cv::Mat(), Hist, 1, hist_size, ranges, true, false);

	//迭代直方图数据
	int row = Hist.rows;
    int col = Hist.cols;
    float *data = (float*)Hist.data;
	std::vector<std::vector<int> > CountPixelVec;
    if (data) 
	{
        for (int i = 0; i < 256; ++i)
		{
			std::vector<int> CountPixel(2);
			CountPixel[0]=data[i];
			CountPixel[1] =i;
			CountPixelVec.push_back(CountPixel);
        }
    }
	//lambda 指定排序规则
	std::sort(CountPixelVec.begin(),CountPixelVec.end(),[](std::vector<int> a,std::vector<int> b){return a[0]>b[0];});//降序排序

	int sum =0;
	for(int i =0 ;i<num;i++)
	{
		sum+=CountPixelVec[i][1];
	}

	int result = static_cast<int>(sum/num);
	return result;
}

/********************************************************
 * @brief kmeans获得图像单通道集群值作为阈值预估值
 * @param frame 输入图像
 * @return 返回预估像素值
 * ******************************************************/
int KmeansGetThreshold(cv::Mat &frame)
{
	//图像的宽和高
	int width = frame.cols;
	int height = frame.rows;

	//将图像数据转化为一维的矩阵 uchar -> float kmeans要用到float
	cv::Mat points(width * height,1,CV_32F); //行 列
	for(int i =0;i<height;i++)
	{
		uchar* data3dims = frame.ptr<uchar>(i);
		for (int j = 0; j < width * 1; j++) 
		{
			float *data1dim =points.ptr<float>(i*width+j);  			
			data1dim[0] =static_cast<float>(data3dims[j]);
		}
	}

	//kmeans
	int clusterCount =10;
	cv::Mat labels; //存放结果标签
	cv::TermCriteria criteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 100, 0.01);//迭代终止条件
	kmeans(points, clusterCount, labels, criteria, 3, cv::KMEANS_RANDOM_CENTERS);

	//计算各个类别的数量
	std::vector<int> Count(clusterCount,0);
	for(int i =0;i<labels.rows;i++)
	{
		int* labeldata = labels.ptr<int>(i);
		for (int j = 0; j < labels.cols * 1; j++) 
		{
			//循环类别数 对应类别计数加一
			for(int k =0 ;k<clusterCount;k++)
			{
				if(labeldata[j] == k)
				{
					Count[k]++;
				}
			}
		}
	}

	//求出类别数量最多的集群
	int max =0;
	int labelMost =0;
	for(int i =0 ;i <clusterCount;i++)
	{
		if(Count[i]>max)
		{
			max =Count[i];
			labelMost =i;
		}	
	}

	//对类别最多的集群求平均值作为预估值
	float sum =0;
	int num =0;
	for(int i =0;i<labels.rows;i++)
	{
		int* labeldata = labels.ptr<int>(i);
		float * pointdata =points.ptr<float>(i);
		for (int j = 0; j < labels.cols * 1; j++) 
		{			
			if(labeldata[j]==labelMost)
			{
				sum+=pointdata[j];
				num++;
			}
		}
	}
	int result =static_cast<int>(sum/num);

	return result;
}

/********************************************************
 * @brief 读取图像或视频流第一帧
 * @param frame 读取到的图像
 * @param path 图像路径
 * @param width 图像宽 自动指定320
 * ******************************************************/
bool ReadFrame(cv::Mat &frame,std::string &path,int width)
{
	//读取图像也可以用VideoCapture	
	cv::VideoCapture cap;
	std::string videoIndex="0";

	//如果输入不是设备0 检查路径
	if(path!=videoIndex)
	{
		//处理路径是否对应文件
		std::fstream file;
		file.open(path,std::ios::in);
		if(!file)
		{
			std::cout<<"\n（路径下没有文件 请检查路径）\n";
			return 0;
		}

		//处理文件是否为图像或者视频流 检查文件类型
		//好像不支持\S 用[^\t\n\r\f\v]替代
		std::regex r("^[^\t\n\r\f\v]+.(jpg|png|jpeg|avi|mp4)$" );
		if(!regex_match(path,r))
		{
			std::cout<<"\n（目前支持.jpg .png .jpeg .avi .mp4文件）\n";
			return 0;
		}
		
		cap.open(path);	
	}
	else
	{
		cap.open(0);	
	}
	
	if (!cap.isOpened())
	{
		return 0;
	}
	cap>>frame;

	if(frame.empty())
    {
        return 0;
    }

	//resize
    float k =frame.cols*1.0/width;
	int height =static_cast<int>(frame.rows/k);
	cv::resize(frame,frame,cv::Size(width,height));

	return 1;	       
}

/********************************************************
 * @brief 将同样尺寸的图像 按高合并（横向合并）
 * @param img1 输入图像1
 * @param img2 输入图像2
 * @param OutputImg 输出图像
 * ******************************************************/
bool HeightMerge(cv::Mat &img1,cv::Mat &img2,cv::Mat &OutputImg)
{
	//如果尺寸不一样就返回
	if(img1.rows != img2.rows || img1.cols != img2.cols)
	{
		std::cout<<"尺寸不一样";
		return 0;
	}
	if(img1.channels() ==1){cv::cvtColor(img1,img1,cv::COLOR_GRAY2BGR);}
	if(img2.channels() ==1){cv::cvtColor(img2,img2,cv::COLOR_GRAY2BGR);}
	
	//图像宽和高
	int width =img1.cols;
	int height =img1.rows;

	//图像和矩形框暂时变量
	cv::Mat TempMat;
	CvRect TempRect;

	//第一张图
	TempRect = cvRect(0, 0, width, height);
	TempMat = OutputImg(TempRect);
    img1.colRange(0,width).copyTo(TempMat);

	 // 第二张图
    TempRect = cvRect(width, 0,width, height);
    TempMat = OutputImg(TempRect);
    img2.colRange(0, width).copyTo(TempMat);

	return 1;
}

void print_message(void)
{
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
}

void AdjustThreshold(void)
{
	//打印使用要求
	print_message();
	
	//读取路径
	std::cout<<"(Ctrl+C退出)  File Path:(输入0打开摄像头)";
	std::string path;
	getline(std::cin,path);

	//读取视频流第一帧或者图像 自动按比缩放至宽为320 也可以自己指定ReadFrame的width参数
	cv::Mat frame;
	while(!ReadFrame(frame,path,320))
	{
		std::cout<<"文件名输入错误，请按要求输入\n";
		std::cout<<"(Ctrl+C退出)  File Path:(输入0打开摄像头)";
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
	int offset =40;//将偏移量设为40
	
	// 开始主循环
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
}
