#pragma once
using namespace std;
class Vector3
{
public:
	Vector3();
	Vector3(double _vecData[3]);
	Vector3(double _x, double _y, double _z);
	double x, y, z;
	~Vector3();

	Vector3 operator+ (const Vector3 &_v);
	Vector3 operator- (const Vector3 &_v);
	Vector3 operator* (float f);
	double length()const;
	static double dotProduct(const Vector3 &_v1,const Vector3 &_v2);//点积
	static Vector3 crossProduct(const Vector3 & _v1, const Vector3 & _v2);//叉积
	static Vector3 normalize(const Vector3 &_v);
	static Vector3 reflect(const Vector3 inDirection, const Vector3 inNormal);//求反射向量
};

