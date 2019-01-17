#pragma once
//全局变量
class Object;
#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif

const int windowWidth = 500;						// 窗体宽度
const int windowHeight = 500;						// 窗体高度

#define PI 3.1415926535898f

enum DRAW_MODE {//绘制方式
	MY_POINT = 0,//点绘制
	MY_LINE = 1,//线绘制
	MY_FILL = 2//三角形填充
};

enum GET_COLOR_MODE {
	Ver_Color = 0,
	Tex_Sample = 1
};

enum WINDING_MODE//卷绕方式
{
	CCW = 0,//逆时针
	CW = 1//顺时针
};

enum CULL_MODE//背面剪裁方式
{
	NONE = 0,
	BACK = 1,
	FRONT = 2,
	ALL = 3
};

struct Color {
	unsigned char B;
	unsigned char G;
	unsigned char R; //无符号1字节八位0~255
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
	double x=0, y=0;//屏幕XY坐标
	double w=0;//深度值
	double S = 0, T = 0;//纹理坐标值
	Color color;//点颜色值
	double lightResult=0.0;
	Point() {}

	Point(int _x, int _y) :x(_x), y(_y) {}
	Point(int _x, int _y, Color _color) :x(_x), y(_y), color(_color) {}
	Point(int _x, int _y, double _w, double _s, double _t) :x(_x), y(_y), w(_w), S(_s), T(_t) {}
	Point(int _x, int _y, double _w, Color _color) :x(_x), y(_y), w(_w), color(_color) {}

};


struct RasterizationStateInfo {//光栅化阶段信息
	DRAW_MODE drawMode;//绘制方式
	GET_COLOR_MODE colorMode ;//颜色获取方式
	WINDING_MODE windingMode;//卷绕方式
	CULL_MODE cullMode;//背面剪裁方式

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

