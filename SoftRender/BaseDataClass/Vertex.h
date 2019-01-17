#pragma once
#include "../util/Constant.h"
class Vertex
{
public:
	Vertex();
	Vertex(double _x,double _y,double _z, double _nx, double _ny, double _nz, Color _color);
	Vertex(double _x,double _y,double _z, double _nx, double _ny, double _nz, double _s, double _t);
	Vertex(double _x,double _y,double _z, double _nx, double _ny, double _nz, double _s, double _t, Color _color);
	double x, y, z;//位置坐标
	Color color;//点颜色
	double nx, ny, nz;//法向量
	double s, t;//S，T纹理坐标


	bool ifShow;
	


	~Vertex();
};

