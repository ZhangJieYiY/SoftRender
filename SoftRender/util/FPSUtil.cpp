#include "FPSUtil.h"
#include <ctime> 
#include <windows.h>
using namespace std;

int FPSUtil::currFPS;//��ǰ֡��
int FPSUtil::frameCount;//��ǰʱ����ڵ�֡��
long long FPSUtil::lastTime;//��һ�λ���ʱ��
long long FPSUtil::nowTime;//��ǰʱ��

FPSUtil::FPSUtil()
{
}

void FPSUtil::init() {
	lastTime = GetTickCount();
}
void FPSUtil::calFPS()
{
	frameCount++;
	nowTime = GetTickCount();//��ȡ��ǰʱ��
	long long D_value = nowTime - lastTime;
	if (D_value > 1000)//����ʱ����Ƿ�ﵽһ��
	{
		currFPS = (int)((float)frameCount / (float)(D_value / 1000));//֡������ʱ��
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
