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
	RasterizationStateInfo rasterizationStateInfo;//��դ���׶���Ϣ
	double depthBuffer[windowHeight][windowWidth];
	Color colorBufferForSet[windowHeight][windowWidth];
	Color colorBufferForTrans[windowHeight][windowWidth];
	Color colorBufferForDraw[windowHeight][windowWidth];
	bool depthTestAble = true;//��Ȳ����Ƿ���
	bool lightAble = true;//�Ƿ�������
	bool noLight = false;
	bool perCorr = true;

	void clearDepthBuffer();
	void clearColorBuffer();
	bool createRenderPipline(RasterizationStateInfo &_rasterizationStateInfo);//������Ⱦ����

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
	//һ�±���
	Matrix *mMatrix; //�任����
	Matrix *MVMatrix ;//�任��������ռ�ľ���
	Matrix *MPMatrix ;//ͶӰ����
	Matrix *MVPMatrix ;//�ۺϾ���
	void pushUniform(); //mvp����
	void pushConstant();//��ǰ�任����
	void bindSampler(const vector<TexDataObject *> &tdoList);//�󶨲�����
	vector<TexDataObject *> samplerData;
	void RenderPipeline::RenderListClip(vector<Triangle*>&renderlist, ClipPlaneFlag clip_flags);
	~RenderPipeline();


	inline Color interColor(const Point &p1, const Point &p2, const Point &p) {//����  ���ֵ��
		double ratio = distance(p1.x, p1.y, p.x, p.y) / distance(p1.x, p1.y, p2.x, p2.y);
		double r = p1.color.R + (p2.color.R - p1.color.R)*ratio;
		double g = p1.color.G + (p2.color.G - p1.color.G)*ratio;
		double b = p1.color.B + (p2.color.B - p1.color.B)*ratio;
		return  Color((unsigned char)r, (unsigned char)g, (unsigned char)b);
	}

	inline double interWReci(double x0, double y0, double x1, double y1, double w0, double w1, double interX, double interY) {//��ֵW�ĵ���
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
