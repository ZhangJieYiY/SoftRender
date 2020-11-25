#include "Vector3.h"
#include <math.h>

Vector3::Vector3()
	:x(0), y(0), z(0)
{}

Vector3::Vector3(double _vecData[3])
	:x(_vecData[0]), y(_vecData[1]), z(_vecData[2])
{}

Vector3::Vector3(double _x, double _y, double _z)
	:x(_x),y(_y),z(_z)
{}


Vector3 Vector3::operator+(const Vector3 & _v)
{
	double v[3];
	v[0] = x + _v.x;
	v[1] = y + _v.y;
	v[2] = z + _v.z;
	return Vector3(v);
}


Vector3 Vector3::operator-(const Vector3 & _v)
{
	double v[3];
	v[0] = x - _v.x;
	v[1] = y - _v.y;
	v[2] = z - _v.z;
	return Vector3(v);
}

Vector3 Vector3::operator*(float f)
{
	double v[3];
	v[0] = x *f;
	v[1] = y *f;
	v[2] = z *f;
	return Vector3(v);
}


double Vector3::dotProduct(const Vector3 & _v1, const Vector3 & _v2)
{
	double result = 0;
	result += _v1.x*_v2.x;
	result += _v1.y*_v2.y;
	result += _v1.z*_v2.z;
	return result;
}

Vector3 Vector3::crossProduct(const Vector3 & _v1, const Vector3 & _v2)
{
	Vector3 result=Vector3(
		_v1.y * _v2.z - _v1.z * _v2.y,
		_v1.z * _v2.x - _v1.x * _v2.z,
		_v1.x * _v2.y - _v1.y * _v2.x
	);
	return result;
}

double Vector3::length() const{
	return sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
}

Vector3 Vector3::normalize(const Vector3 &_v)
{
	double model = _v.length(); 
	Vector3 out;
	if (model == 0) {
		out = Vector3(0, 0, 0);
	}
	else {
		out = Vector3(_v.x / model, _v.y / model, _v.z / model);
	}
	return out;
}

Vector3 Vector3::reflect(Vector3 inDirection,Vector3 inNormal)
{
	Vector3 Out;
	Out = inDirection - inNormal * 2 * (float)(dotProduct(inDirection, inNormal));
	return Out;
	
}

Vector3::~Vector3()
{

}
