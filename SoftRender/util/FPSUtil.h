#pragma once
class FPSUtil
{
public:
	FPSUtil();
	static int currFPS;//��ǰ֡��
	static int frameCount;//��ǰʱ����ڵ�֡��
	static long long lastTime;//��һ�λ���ʱ��
	static long long nowTime;//��ǰʱ��
	static void calFPS();//����֡��
	static void controlFPS();//����֡��
	static void init();
	~FPSUtil();
};


