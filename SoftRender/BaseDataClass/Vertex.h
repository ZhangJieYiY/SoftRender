#pragma once
#include "../util/Constant.h"
class Vertex
{
public:
	Vertex();
	Vertex(double _x,double _y,double _z, double _nx, double _ny, double _nz, Color _color);
	Vertex(double _x,double _y,double _z, double _nx, double _ny, double _nz, double _s, double _t);
	Vertex(double _x,double _y,double _z, double _nx, double _ny, double _nz, double _s, double _t, Color _color);
	double x, y, z;//λ������
	Color color;//����ɫ
	double nx, ny, nz;//������
	double s, t;//S��T��������


	bool ifShow;
	


	~Vertex();
};

