#pragma once

class Vector3;
class Vertex;

class Vector4
{
public:
	Vector4();
	Vector4(double _vecData[4]);
	Vector4(double _x, double _y, double _z, double _w);
	Vector4(const Vector3 &_v3);
	Vector4(const Vertex &_ver);
	double x, y, z, w;

	Vector4 operator+ (const Vector4 &_v);
	Vector4 operator- (const Vector4 &_v);
	double length();
	static double dotProduct(const Vector4 &_v1, const Vector4 &_v2);//µã»ý
	static Vector4 crossProduct(const Vector4 &_v1, const Vector4 &_v2);//²æ»ý
	static Vector4 normalize(Vector4 _v);
	~Vector4();
};