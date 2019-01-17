#include "Vertex.h"

Vertex::Vertex()
	: x(0), y(0), z(0)
	, nx(0), ny(1), nz(0)
	, s(0), t(0)
{}

Vertex::Vertex(double _x, double _y, double _z, double _nx, double _ny, double _nz, Color _color)
	: x(_x), y(_y), z(_z)
	, nx(_nx), ny(_ny), nz(_nz)
	, color(_color)
{}


Vertex::Vertex(double _x, double _y, double _z, double _nx, double _ny, double _nz, double _s, double _t)
	: x(_x), y(_y), z(_z)
	, nx(_nx), ny(_ny), nz(_nz)
	, s(_s),t(_t)
{}


Vertex::Vertex(double _x, double _y, double _z, double _nx, double _ny, double _nz, double _s, double _t, Color _color) 
	: x(_x), y(_y), z(_z)
	, nx(_nx), ny(_ny), nz(_nz)
	, s(_s), t(_t)
	, color(_color)
{}


Vertex::~Vertex()
{
}
