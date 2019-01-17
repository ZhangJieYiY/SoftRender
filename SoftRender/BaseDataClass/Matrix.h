#pragma once

class Vector4;
class Matrix
{
public:
	Matrix();
	Matrix(double _matData[16]);
	~Matrix();

	Matrix operator+ (const Matrix &_m);
	Matrix operator- (const Matrix &_m);
	void setMatrixData(double _dataIn[16]);
	void translateM(float x, float y, float z);
	void rotateMXYZ(float x_angle, float y_angle, float z_angle);
	void scaleM(float x, float y, float z);
	static void multipMM(Matrix &result, const Matrix &m1, const Matrix &m2);//两个矩阵相乘
	static void multipVM(Vector4 &result, const Vector4 &_v, const Matrix &m);//向量与矩阵相乘
	static void invertM(Matrix &result, const Matrix &m);//逆矩阵
	static void transposeM(Matrix &result, const Matrix &m);//转置矩阵
	static void setLookatM(Matrix &_VM, float cx, float cy, float cz, float tx, float ty, float tz, float upX, float upY, float upZ);//设置观察矩阵 视图矩阵
	static void setPerspectiveM(Matrix &_PM,float left,float right,float top,float bottom,float near,float far);//设置透视投影矩阵
	//矩阵与矩阵的乘法    返回一个矩阵对象
	void setIdentity();

	double matData[16] = { 0 };
private:
	void setRotateMX(float angle);
	void setRotateMY(float angle);
	void setRotateMZ(float angle);

};

