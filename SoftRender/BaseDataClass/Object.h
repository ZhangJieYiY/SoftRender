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
	float kd, ka, ks;//������  ������ �����ϵ��
	vector<Triangle*> triangleList;
	void addTriangle(Triangle *triangle);
	void drawSelf();
	vector<TexDataObject *>tdoList; 
	RenderPipeline *rp ;//���ڻ��ƵĹ��߶���ָ��

	~Object();
};

