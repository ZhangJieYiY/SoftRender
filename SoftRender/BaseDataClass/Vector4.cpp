#include "Vector4.h"
#include "Vector3.h"
#include "Vertex.h"
#include <math.h>
Vector4::Vector4() 
	:x(0), y(0), z(0), w(0) 
{}

Vector4::Vector4(double _vecData[4]) 
	:x(_vecData[0]), y(_vecData[1]), z(_vecData[2]), w(_vecData[3])
{}

Vector4::Vector4(double _x, double _y, double _z, double _w) 
	: x(_x), y(_y), z(_z), w(_w)
{}

Vector4::Vector4(const Vector3 &_v3) 
	: x(_v3.x), y(_v3.y), z(_v3.z), w(1)
{}

Vector4::Vector4(const Vertex &_ver)
	: x(_ver.x), y(_ver.y), z(_ver.z), w(1)
{

}


Vector4 Vector4::operator+ (const Vector4 &_v) {
	double v[4];
	v[0] = x + _v.x;
	v[1] = y + _v.y;
	v[2] = z + _v.z;
	v[3] = w + _v.w;
	return Vector4(v);
}

Vector4 Vector4::operator- (const Vector4 &_v) {
	double v[4];
	v[0] = x - _v.x;
	v[1] = y - _v.y;
	v[2] = z - _v.z;
	v[3] = w - _v.w;
	return Vector4(v);
}

double Vector4::dotProduct(const Vector4 &_v1, const Vector4 &_v2) {//µã»ý
	double result = 0;
	result += _v1.x*_v2.x;
	result += _v1.y*_v2.y;
	result += _v1.z*_v2.z;
	return result;
}

Vector4 Vector4::crossProduct(const Vector4 &_v1, const Vector4 &_v2){//²æ»ý
	Vector4 result;
	result = Vector4(Vector3(
			_v1.y * _v2.z - _v1.z * _v2.y,
			_v1.z * _v2.x - _v1.x * _v2.z,
			_v1.x * _v2.y - _v1.y * _v2.x
		));
	return result;
}

inline double Vector4::length() {
	return sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
}

Vector4 Vector4::normalize(Vector4 _v) {
	double model = _v.length();
	Vector4 out = Vector4(_v.x / model, _v.y / model, _v.z / model, _v.w);
	return out;
}

Vector4::~Vector4()
{
}
