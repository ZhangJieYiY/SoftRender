#pragma once
#include "../util/Constant.h"
#include "../BaseDataClass/Vertex.h"
#include "../BaseDataClass/Vector3.h"
#include <vector>
#include <Windows.h>
class Object;
class Matrix;
class Triangle;
class TexDataObject;
class Camera;

using namespace std;
class RenderPipeline {
public:
	RenderPipeline();

	Camera *currCamera;
	RasterizationStateInfo rasterizationStateInfo;//光栅化阶段信息
	double depthBuffer[windowHeight][windowWidth];
	Color colorBufferForSet[windowHeight][windowWidth];
	Color colorBufferForTrans[windowHeight][windowWidth];
	Color colorBufferForDraw[windowHeight][windowWidth];
	bool depthTestAble = true;//深度测试是否开启
	bool lightAble = true;//是否开启光照
	bool noLight = false;
	bool perCorr = true;

	void clearDepthBuffer();
	void clearColorBuffer();
	bool createRenderPipline(RasterizationStateInfo &_rasterizationStateInfo);//创建渲染管线

	void draw(const Object &obj);
	void present(HDC hdc);
	void calLightResult(Point &p, const Object &obj, const Vertex &ver, const Matrix &traM);
	bool backCullTest(const Triangle &tri,const Matrix &traM);
	void drawLine(const Point &p0, const Point &p1);
	void gouraudTriangleForVerColor(const Point &p1, const Point &p2, const Point &p3);
	void gouraudTriangleForTexSample(const Point &p1, const Point &p2, const Point &p3);
	double interAttriByW(double x0, double y0, double x1, double y1, double x, double y, double w0, double w1, double w, double attri0, double attri1);
	void setColorBuffer(int height, int width, const Color &color);
	bool setDepthBuffer(int height, int width, double depth);
	void setPixel(Point &p);
	//一致变量
	Matrix *mMatrix; //变换矩阵
	Matrix *MVMatrix ;//变换到摄像机空间的矩阵
	Matrix *MPMatrix ;//投影矩阵
	Matrix *MVPMatrix ;//综合矩阵
	void pushUniform(); //mvp矩阵
	void pushConstant();//当前变换矩阵
	void bindSampler(const vector<TexDataObject *> &tdoList);//绑定采样器
	vector<TexDataObject *> samplerData;
	void RenderPipeline::RenderListClip(vector<Triangle*>&renderlist, ClipPlaneFlag clip_flags);
	~RenderPipeline();


	inline Color interColor(const Point &p1, const Point &p2, const Point &p) {//两端  需插值的
		double ratio = distance(p1.x, p1.y, p.x, p.y) / distance(p1.x, p1.y, p2.x, p2.y);
		double r = p1.color.R + (p2.color.R - p1.color.R)*ratio;
		double g = p1.color.G + (p2.color.G - p1.color.G)*ratio;
		double b = p1.color.B + (p2.color.B - p1.color.B)*ratio;
		return  Color((unsigned char)r, (unsigned char)g, (unsigned char)b);
	}

	inline double interWReci(double x0, double y0, double x1, double y1, double w0, double w1, double interX, double interY) {//插值W的倒数
		double ratio = distance(x0, y0, interX, interY) / distance(x1, y1, x0, y0);
		double result = w0 + (w1 - w0)*ratio;
		return result;
	}

	inline double distance(double x0, double y0, double x1, double y1) {
		double distance;
		if (isnan(x0) || isnan(y0) || isnan(x1) || isnan(y1)) distance = 0;
		distance = sqrt(pow((x1 - x0), 2) + pow((y1 - y0), 2));
		return distance == 0 ? 0.00000000001 : abs(distance);
	}

	inline void GetRGBValue(const Color &color, double &r, double &g, double &b)
	{
		r = color.R;
		g = color.G;
		b = color.B;
	}

	inline void GetSTValue(const Point &p, double &s, double &t)
	{
		s = p.S;
		t = p.T;
	}

	inline void VertexCreate(Vertex *v1, Vertex *v2, Vector3 &v) {
		v.x = (v2->x - v1->x);
		v.y = (v2->y - v1->y);
		v.z = (v2->z - v1->z);
	}

};
