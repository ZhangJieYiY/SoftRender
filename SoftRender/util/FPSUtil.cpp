#include "FPSUtil.h"
#include <ctime> 
#include <windows.h>
using namespace std;

int FPSUtil::currFPS;//当前帧率
int FPSUtil::frameCount;//当前时间段内的帧数
long long FPSUtil::lastTime;//上一次绘制时间
long long FPSUtil::nowTime;//当前时间

FPSUtil::FPSUtil()
{
}

void FPSUtil::init() {
	lastTime = GetTickCount();
}
void FPSUtil::calFPS()
{
	frameCount++;
	nowTime = GetTickCount();//获取当前时间
	long long D_value = nowTime - lastTime;
	if (D_value > 1000)//计算时间差是否达到一秒
	{
		currFPS = (int)((float)frameCount / (float)(D_value / 1000));//帧数除以时间
		frameCount = 0;
		lastTime = nowTime;
	}
}

void FPSUtil::controlFPS()
{
}


FPSUtil::~FPSUtil()
{
}
