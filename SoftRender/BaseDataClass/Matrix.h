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
	static void multipMM(Matrix &result, const Matrix &m1, const Matrix &m2);//�����������
	static void multipVM(Vector4 &result, const Vector4 &_v, const Matrix &m);//������������
	static void invertM(Matrix &result, const Matrix &m);//�����
	static void transposeM(Matrix &result, const Matrix &m);//ת�þ���
	static void setLookatM(Matrix &_VM, float cx, float cy, float cz, float tx, float ty, float tz, float upX, float upY, float upZ);//���ù۲���� ��ͼ����
	static void setPerspectiveM(Matrix &_PM,float left,float right,float top,float bottom,float near,float far);//����͸��ͶӰ����
	//���������ĳ˷�    ����һ���������
	void setIdentity();

	double matData[16] = { 0 };
private:
	void setRotateMX(float angle);
	void setRotateMY(float angle);
	void setRotateMZ(float angle);

};

