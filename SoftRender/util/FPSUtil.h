#pragma once
class FPSUtil
{
public:
	FPSUtil();
	static int currFPS;//当前帧率
	static int frameCount;//当前时间段内的帧数
	static long long lastTime;//上一次绘制时间
	static long long nowTime;//当前时间
	static void calFPS();//计算帧率
	static void controlFPS();//控制帧率
	static void init();
	~FPSUtil();
};


