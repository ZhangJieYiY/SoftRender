#pragma once
//ȫ�ֱ���
class Object;
#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif

const int windowWidth = 500;						// ������
const int windowHeight = 500;						// ����߶�

#define PI 3.1415926535898f

enum DRAW_MODE {//���Ʒ�ʽ
	MY_POINT = 0,//�����
	MY_LINE = 1,//�߻���
	MY_FILL = 2//���������
};

enum GET_COLOR_MODE {
	Ver_Color = 0,
	Tex_Sample = 1
};

enum WINDING_MODE//���Ʒ�ʽ
{
	CCW = 0,//��ʱ��
	CW = 1//˳ʱ��
};

enum CULL_MODE//������÷�ʽ
{
	NONE = 0,
	BACK = 1,
	FRONT = 2,
	ALL = 3
};

struct Color {
	unsigned char B;
	unsigned char G;
	unsigned char R; //�޷���1�ֽڰ�λ0~255
	Color(unsigned char _R = 255,unsigned char _G = 255, unsigned char _B = 255) {
		B = min(_B, 255);
		G = min(_G, 255);
		R = min(_R, 255);
	}

	Color operator*(float scalar)
	{
		Color((unsigned char)(R*scalar), (unsigned char)(G*scalar), (unsigned char)(B*scalar));
	}
};

struct Point {
	double x=0, y=0;//��ĻXY����
	double w=0;//���ֵ
	double S = 0, T = 0;//��������ֵ
	Color color;//����ɫֵ
	double lightResult=0.0;
	Point() {}

	Point(int _x, int _y) :x(_x), y(_y) {}
	Point(int _x, int _y, Color _color) :x(_x), y(_y), color(_color) {}
	Point(int _x, int _y, double _w, double _s, double _t) :x(_x), y(_y), w(_w), S(_s), T(_t) {}
	Point(int _x, int _y, double _w, Color _color) :x(_x), y(_y), w(_w), color(_color) {}

};


struct RasterizationStateInfo {//��դ���׶���Ϣ
	DRAW_MODE drawMode;//���Ʒ�ʽ
	GET_COLOR_MODE colorMode ;//��ɫ��ȡ��ʽ
	WINDING_MODE windingMode;//���Ʒ�ʽ
	CULL_MODE cullMode;//������÷�ʽ

	RasterizationStateInfo(DRAW_MODE _drawMode= MY_FILL, GET_COLOR_MODE _colorMode=Ver_Color , WINDING_MODE _windingMode= CCW, CULL_MODE _cullMode= NONE) {
		drawMode = _drawMode;
		colorMode = _colorMode;
		windingMode = _windingMode;
		cullMode = _cullMode;
	}
};
const Color clearColor = {
	0,127,127
};


enum ClipPlaneFlag {
	CLIP_POLY_X_PLANE=1,
	CLIP_POLY_Y_PLANE=2,
	CLIP_POLY_Z_PLANE=4
};

