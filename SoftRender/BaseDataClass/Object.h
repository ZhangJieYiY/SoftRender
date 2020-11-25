#pragma once
//class Triangle;
class TexDataObject;
class RenderPipeline;
#include "Triangle.h"
class Object
{
public:
	Object();
	Object(double *_vPosData,double *_vUVData ,double *_vNormalData,int _vCount);
	float kd, ka, ks;//漫反射  环境光 镜面光系数
	vector<Triangle*> triangleList;
	void addTriangle(Triangle *triangle);
	void drawSelf();
	vector<TexDataObject *>tdoList; 
	RenderPipeline *rp ;//用于绘制的管线对象指针

	~Object();
};

