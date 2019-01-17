
#include "Matrix.h"
#include "Vector3.h"
#include "Vector4.h"
#include <math.h>
#include "../util/Constant.h"
#include <assert.h>

Matrix::Matrix()
{}

Matrix::Matrix(double _matData[16])
{
	for (int i = 0; i < 16; i++)
	{
		matData[i] = _matData[i];
	}
}

void Matrix::setMatrixData(double _matData[16])
{
	for (int i = 0; i < 16; i++)
	{
		matData[i] = _matData[i];
	}
}


Matrix Matrix::operator+(const Matrix & _m)
{
	double m[16];
	for (int i = 0; i < 16; i++)
	{
		m[i] = matData[i] + _m.matData[i];
	}
	return Matrix(m);
}

Matrix Matrix::operator-(const Matrix & _m)
{
	double m[16];
	for (int i = 0;i < 16; i++)
	{
		m[i] = matData[i] - _m.matData[i];
	}
	return Matrix(m);
}


//利用矩阵对点 或向量进行变换
void Matrix::multipVM(Vector4 &result, const Vector4 & _v, const Matrix & m)
{
	double v[4];
	v[0] = _v.x * m.matData[0] + _v.y* m.matData[4] + _v.z * m.matData[8] + _v.w * m.matData[12];
	v[1] = _v.x * m.matData[1] + _v.y* m.matData[5] + _v.z * m.matData[9] + _v.w * m.matData[13];
	v[2] = _v.x * m.matData[2] + _v.y* m.matData[6] + _v.z * m.matData[10] + _v.w * m.matData[14];
	v[3] = _v.x * m.matData[3] + _v.y* m.matData[7] + _v.z * m.matData[11] + _v.w * m.matData[15];
	result = Vector4(v);
}

/*
0  1  2  3
4  5  6  7
8  9  10 11
12 13 14 15
*/

//两个矩阵相乘
void Matrix::multipMM(Matrix & result, const Matrix & m1, const Matrix & m2)
{
	double m[16];


	m[0] = (float)(m1.matData[0] * m2.matData[0] + m1.matData[4] * m2.matData[1] + m1.matData[8] * m2.matData[2] + m1.matData[12] * m2.matData[3]);
	m[1] = (float)(m1.matData[1] * m2.matData[0] + m1.matData[5] * m2.matData[1] + m1.matData[9] * m2.matData[2] + m1.matData[13] * m2.matData[3]);
	m[2] = (float)(m1.matData[2] * m2.matData[0] + m1.matData[6] * m2.matData[1] + m1.matData[10] * m2.matData[2] + m1.matData[14] * m2.matData[3]);
	m[3] = (float)(m1.matData[3] * m2.matData[0] + m1.matData[7] * m2.matData[1] + m1.matData[11] * m2.matData[2] + m1.matData[15] * m2.matData[3]);
	m[4] = (float)(m1.matData[0] * m2.matData[4] + m1.matData[4] * m2.matData[5] + m1.matData[8] * m2.matData[6] + m1.matData[12] * m2.matData[7]);
	m[5] = (float)(m1.matData[1] * m2.matData[4] + m1.matData[5] * m2.matData[5] + m1.matData[9] * m2.matData[6] + m1.matData[13] * m2.matData[7]);
	m[6] = (float)(m1.matData[2] * m2.matData[4] + m1.matData[6] * m2.matData[5] + m1.matData[10] * m2.matData[6] + m1.matData[14] * m2.matData[7]);
	m[7] = (float)(m1.matData[3] * m2.matData[4] + m1.matData[7] * m2.matData[5] + m1.matData[11] * m2.matData[6] + m1.matData[15] * m2.matData[7]);
	m[8] = (float)(m1.matData[0] * m2.matData[8] + m1.matData[4] * m2.matData[9] + m1.matData[8] * m2.matData[10] + m1.matData[12] * m2.matData[11]);
	m[9] = (float)(m1.matData[1] * m2.matData[8] + m1.matData[5] * m2.matData[9] + m1.matData[9] * m2.matData[10] + m1.matData[13] * m2.matData[11]);
	m[10] = (float)(m1.matData[2] * m2.matData[8] + m1.matData[6] * m2.matData[9] + m1.matData[10] * m2.matData[10] + m1.matData[14] * m2.matData[11]);
	m[11] = (float)(m1.matData[3] * m2.matData[8] + m1.matData[7] * m2.matData[9] + m1.matData[11] * m2.matData[10] + m1.matData[15] * m2.matData[11]);
	m[12] = (float)(m1.matData[0] * m2.matData[12] + m1.matData[4] * m2.matData[13] + m1.matData[8] * m2.matData[14] + m1.matData[12] * m2.matData[15]);
	m[13] = (float)(m1.matData[1] * m2.matData[12] + m1.matData[5] * m2.matData[13] + m1.matData[9] * m2.matData[14] + m1.matData[13] * m2.matData[15]);
	m[14] = (float)(m1.matData[2] * m2.matData[12] + m1.matData[6] * m2.matData[13] + m1.matData[10] * m2.matData[14] + m1.matData[14] * m2.matData[15]);
	m[15] = (float)(m1.matData[3] * m2.matData[12] + m1.matData[7] * m2.matData[13] + m1.matData[11] * m2.matData[14] + m1.matData[15] * m2.matData[15]);


	//m[0] = m1.matData[0] * m2.matData[0] + m1.matData[1] * m2.matData[4] + m1.matData[2] * m2.matData[8] + m1.matData[3] * m2.matData[12];
	//m[1] = m1.matData[0] * m2.matData[1] + m1.matData[1] * m2.matData[5] + m1.matData[2] * m2.matData[9] + m1.matData[3] * m2.matData[13];
	//m[2] = m1.matData[0] * m2.matData[2] + m1.matData[1] * m2.matData[6] + m1.matData[2] * m2.matData[10] + m1.matData[3] * m2.matData[14];
	//m[3] = m1.matData[0] * m2.matData[3] + m1.matData[1] * m2.matData[7] + m1.matData[2] * m2.matData[11] + m1.matData[3] * m2.matData[15];

	//m[4] = m1.matData[4] * m2.matData[0] + m1.matData[5] * m2.matData[4] + m1.matData[6] * m2.matData[8] + m1.matData[7] * m2.matData[12];
	//m[5] = m1.matData[4] * m2.matData[1] + m1.matData[5] * m2.matData[5] + m1.matData[6] * m2.matData[9] + m1.matData[7] * m2.matData[13];
	//m[6] = m1.matData[4] * m2.matData[2] + m1.matData[5] * m2.matData[6] + m1.matData[6] * m2.matData[10] + m1.matData[7] * m2.matData[14];
	//m[7] = m1.matData[4] * m2.matData[3] + m1.matData[5] * m2.matData[7] + m1.matData[6] * m2.matData[11] + m1.matData[7] * m2.matData[15];

	//m[8] = m1.matData[8] * m2.matData[0] + m1.matData[9] * m2.matData[4] + m1.matData[10] * m2.matData[8] + m1.matData[11] * m2.matData[12];
	//m[9] = m1.matData[8] * m2.matData[1] + m1.matData[9] * m2.matData[4] + m1.matData[10] * m2.matData[9] + m1.matData[11] * m2.matData[13];
	//m[10] = m1.matData[8] * m2.matData[2] + m1.matData[9] * m2.matData[4] + m1.matData[10] * m2.matData[10] + m1.matData[11] * m2.matData[14];
	//m[11] = m1.matData[8] * m2.matData[3] + m1.matData[9] * m2.matData[4] + m1.matData[10] * m2.matData[11] + m1.matData[11] * m2.matData[15];

	//m[12] = m1.matData[12] * m2.matData[0] + m1.matData[13] * m2.matData[4] + m1.matData[14] * m2.matData[8] + m1.matData[15] * m2.matData[12];
	//m[13] = m1.matData[12] * m2.matData[1] + m1.matData[13] * m2.matData[5] + m1.matData[14] * m2.matData[9] + m1.matData[15] * m2.matData[13];
	//m[14] = m1.matData[12] * m2.matData[2] + m1.matData[13] * m2.matData[6] + m1.matData[14] * m2.matData[10] + m1.matData[15] * m2.matData[14];
	//m[15] = m1.matData[12] * m2.matData[3] + m1.matData[13] * m2.matData[7] + m1.matData[14] * m2.matData[11] + m1.matData[15] * m2.matData[15];
	result = Matrix(m);
}

/*平移变换矩阵
1 0 0 0 
0 1 0 0 
0 0 1 0 
x y z 1
*/

void Matrix::translateM( float x, float y, float z)
{
	Matrix rm = Matrix();
	rm.setIdentity();
	rm.matData[12] = x;
	rm.matData[13] = y;
	rm.matData[14] = z;//首先生成平移变换矩阵
	multipMM(*this, rm, *this);//再与原矩阵相乘得到最终变换矩阵

}

void Matrix::rotateMXYZ( float x_angle, float y_angle, float z_angle)
{
	Matrix rmX = Matrix();
	Matrix rmY = Matrix();
	Matrix rmZ = Matrix();
	rmX.setRotateMX(x_angle);//首先生成旋转变换矩阵
	rmY.setRotateMY(y_angle);//首先生成旋转变换矩阵
	rmZ.setRotateMZ(z_angle);//首先生成旋转变换矩阵
	multipMM(*this, rmX, *this);//再与原矩阵相乘得到最终变换矩阵
	multipMM(*this, rmY, *this);//再与原矩阵相乘得到最终变换矩阵
	multipMM(*this, rmZ, *this);//再与原矩阵相乘得到最终变换矩阵
}

void Matrix::setRotateMX( float angle)
{
	float radians = angle * PI / 180.0f;
	float s = (float)sin(radians);
	float c = (float)cos(radians);
	this->setIdentity();
	this->matData[0] = 1;	this->matData[1] = 0;	this->matData[2] = 0;	this->matData[3] = 0;
	this->matData[4] = 0;	this->matData[5] = c;	this->matData[6] = s;	this->matData[7] = 0;
	this->matData[8] = 0;	this->matData[9] = -s;	this->matData[10] = c;	this->matData[11] = 0;
	this->matData[12] = 0;	this->matData[13] = 0;	this->matData[14] = 0;	this->matData[15] = 1;
}

void Matrix::setRotateMY(float angle)
{
	float radians = angle * PI / 180.0f;
	float s = (float)sin(radians);
	float c = (float)cos(radians);
	this->setIdentity();
	this->matData[0] = c;	this->matData[1] = 0;	this->matData[2] = -s;	this->matData[3] = 0;
	this->matData[4] = 0;	this->matData[5] = 1;	this->matData[6] = 0;	this->matData[7] = 0;
	this->matData[8] = s;	this->matData[9] = 0;	this->matData[10] = c;	this->matData[11] = 0;
	this->matData[12] = 0;	this->matData[13] = 0;	this->matData[14] = 0;	this->matData[15] = 1;
}

void Matrix::setRotateMZ(float angle)
{
	float radians = angle * PI / 180.0f;
	float s = (float)sin(radians);
	float c = (float)cos(radians);
	this->setIdentity();
	this->matData[0] = c;	this->matData[1] = s;	this->matData[2] = 0;	this->matData[3] = 0;
	this->matData[4] = -s;	this->matData[5] = c;	this->matData[6] = 0;	this->matData[7] = 0;
	this->matData[8] = 0;	this->matData[9] = 0;	this->matData[10] = 1;	this->matData[11] = 0;
	this->matData[12] = 0;	this->matData[13] = 0;	this->matData[14] = 0;	this->matData[15] = 1;
}

void Matrix::scaleM( float x, float y, float z)//设置缩放矩阵
{
	Matrix rm = Matrix();
	rm.setIdentity();
	rm.matData[0] = x;
	rm.matData[5] = y;
	rm.matData[10] = z;
	rm.matData[15] = 1;//首先生成缩放矩阵
	multipMM(*this, rm, *this);//再与原矩阵相乘得到最终变换矩阵
}

void Matrix::setIdentity() {//设置单位矩阵
	for (int i = 0; i < 16; i++) {
		matData[0] = 0;
	}
	matData[0] = 1;
	matData[5] = 1;
	matData[10] = 1;
	matData[15] = 1;
}
/*
zaxis=normalize(target-center);
xaxis=normalize(cross(up,zaxis));
yaxis=cross(zaxis,xaxis);

xaxis.x           yaxis.x           zaxis.x          0
xaxis.y           yaxis.y           zaxis.y          0
xaxis.z           yaxis.z           zaxis.z          0
-dot(xaxis, eye)  -dot(yaxis, eye)  -dot(zaxis, eye) 1

*/

void Matrix::setLookatM(Matrix &_VM, float cx, float cy, float cz, float tx, float ty, float tz, float upX, float upY, float upZ) {//设置观察矩阵view matrix VM
	Vector3 center = Vector3(cx, cy, cz);
	Vector3 target = Vector3(tx, ty, tz);
	Vector3 up = Vector3(upX, upY, upZ);

	Vector3 zaxis = Vector3::normalize(target - center);
	Vector3 xaxis = Vector3::normalize(Vector3::crossProduct(up, zaxis));
	Vector3 yaxis = Vector3::crossProduct(zaxis, xaxis);
	if (isnan(xaxis.x) && isnan(xaxis.y) && isnan(xaxis.z))
	{
		xaxis.x = 0.000001;
		xaxis.y = 0.000001;
		xaxis.z = 0.000001;
	}
	if (isnan(yaxis.x) && isnan(yaxis.y) && isnan(yaxis.z))
	{
		yaxis.x = 0.000001;
		yaxis.y = 0.000001;
		yaxis.z = 0.000001;
	}
	double VMData[16] =
	{
		xaxis.x, yaxis.x, zaxis.x, 0,
		xaxis.y, yaxis.y, zaxis.y, 0,
		xaxis.z, yaxis.z, zaxis.z, 0,
		-Vector3::dotProduct(xaxis, center) ,-Vector3::dotProduct(yaxis, center), -Vector3::dotProduct(zaxis, center),  1
	};
	_VM.setMatrixData(VMData);
}

/*
2*n/w  0       0              0
0       2*n/h  0              0
0       0       f/(f-n)       1
0       0       n*f/(n-f)	  0

*/
void Matrix::setPerspectiveM(Matrix &_PM, float left, float right, float top, float bottom, float near, float far) {//设置透视投影矩阵
	double PMData[16] =
	{
		2 * near / (right - left),	0 ,							(right + left) / (right - left),				0,
		0,							2 * near / (top - bottom) ,	(top + bottom) / (top - bottom),				0,
		0,							0,							(far) / (far - near),							1,
		0,							0,							near * far / (near - far),						0
	};
	_PM.setMatrixData(PMData);
}

void Matrix::invertM(Matrix &result, const Matrix &m)
{
	double src0 = m.matData[0];
	double src4 = m.matData[1];
	double src8 = m.matData[2];
	double src12 = m.matData[3];
	double src1 = m.matData[4];
	double src5 = m.matData[5];
	double src9 = m.matData[6];
	double src13 = m.matData[7];
	double src2 = m.matData[8];
	double src6 = m.matData[9];
	double src10 = m.matData[10];
	double src14 = m.matData[11];
	double src3 = m.matData[12];
	double src7 = m.matData[13];
	double src11 = m.matData[14];
	double src15 = m.matData[15];
	double atmp0 = src10 * src15;
	double atmp1 = src11 * src14;
	double atmp2 = src9  * src15;
	double atmp3 = src11 * src13;
	double atmp4 = src9  * src14;
	double atmp5 = src10 * src13;
	double atmp6 = src8  * src15;
	double atmp7 = src11 * src12;
	double atmp8 = src8  * src14;
	double atmp9 = src10 * src12;
	double atmp10 = src8  * src13;
	double atmp11 = src9  * src12;
	double dst0 = (atmp0 * src5 + atmp3 * src6 + atmp4  * src7)
		- (atmp1 * src5 + atmp2 * src6 + atmp5  * src7);
	double dst1 = (atmp1 * src4 + atmp6 * src6 + atmp9  * src7)
		- (atmp0 * src4 + atmp7 * src6 + atmp8  * src7);
	double dst2 = (atmp2 * src4 + atmp7 * src5 + atmp10 * src7)
		- (atmp3 * src4 + atmp6 * src5 + atmp11 * src7);
	double dst3 = (atmp5 * src4 + atmp8 * src5 + atmp11 * src6)
		- (atmp4 * src4 + atmp9 * src5 + atmp10 * src6);
	double dst4 = (atmp1 * src1 + atmp2 * src2 + atmp5  * src3)
		- (atmp0 * src1 + atmp3 * src2 + atmp4  * src3);
	double dst5 = (atmp0 * src0 + atmp7 * src2 + atmp8  * src3)
		- (atmp1 * src0 + atmp6 * src2 + atmp9  * src3);
	double dst6 = (atmp3 * src0 + atmp6 * src1 + atmp11 * src3)
		- (atmp2 * src0 + atmp7 * src1 + atmp10 * src3);
	double dst7 = (atmp4 * src0 + atmp9 * src1 + atmp10 * src2)
		- (atmp5 * src0 + atmp8 * src1 + atmp11 * src2);
	double btmp0 = src2 * src7;
	double btmp1 = src3 * src6;
	double btmp2 = src1 * src7;
	double btmp3 = src3 * src5;
	double btmp4 = src1 * src6;
	double btmp5 = src2 * src5;
	double btmp6 = src0 * src7;
	double btmp7 = src3 * src4;
	double btmp8 = src0 * src6;
	double btmp9 = src2 * src4;
	double btmp10 = src0 * src5;
	double btmp11 = src1 * src4;
	double dst8 = (btmp0  * src13 + btmp3  * src14 + btmp4  * src15)
		- (btmp1  * src13 + btmp2  * src14 + btmp5  * src15);
	double dst9 = (btmp1  * src12 + btmp6  * src14 + btmp9  * src15)
		- (btmp0  * src12 + btmp7  * src14 + btmp8  * src15);
	double dst10 = (btmp2  * src12 + btmp7  * src13 + btmp10 * src15)
		- (btmp3  * src12 + btmp6  * src13 + btmp11 * src15);
	double dst11 = (btmp5  * src12 + btmp8  * src13 + btmp11 * src14)
		- (btmp4  * src12 + btmp9  * src13 + btmp10 * src14);
	double dst12 = (btmp2  * src10 + btmp5  * src11 + btmp1  * src9)
		- (btmp4  * src11 + btmp0  * src9 + btmp3  * src10);
	double dst13 = (btmp8  * src11 + btmp0  * src8 + btmp7  * src10)
		- (btmp6  * src10 + btmp9  * src11 + btmp1  * src8);
	double dst14 = (btmp6  * src9 + btmp11 * src11 + btmp3  * src8)
		- (btmp10 * src11 + btmp2  * src8 + btmp7  * src9);
	double dst15 = (btmp10 * src10 + btmp4  * src8 + btmp9  * src9)
		- (btmp8  * src9 + btmp11 * src10 + btmp5  * src8);
	double det =
		src0 * dst0 + src1 * dst1 + src2 * dst2 + src3 * dst3;
	assert(det != 0.0);
	double invdet = 1.0 / det;
	result.matData[0] = dst0  * invdet;
	result.matData[1 ] = dst1  * invdet;
	result.matData[2 ] = dst2  * invdet;
	result.matData[3 ] = dst3  * invdet;
	result.matData[4 ] = dst4  * invdet;
	result.matData[5 ] = dst5  * invdet;
	result.matData[6 ] = dst6  * invdet;
	result.matData[7 ] = dst7  * invdet;
	result.matData[8 ] = dst8  * invdet;
	result.matData[9 ] = dst9  * invdet;
	result.matData[10 ] = dst10 * invdet;
	result.matData[11 ] = dst11 * invdet;
	result.matData[12 ] = dst12 * invdet;
	result.matData[13 ] = dst13 * invdet;
	result.matData[14 ] = dst14 * invdet;
	result.matData[15 ] = dst15 * invdet;
}


void Matrix::transposeM(Matrix &result, const Matrix &m)
{
	for (int i = 0; i < 4; i++)
	{
		result.matData[i + 0] = m.matData[i * 4 + 0];
		result.matData[i + 4] = m.matData[i * 4 + 1];
		result.matData[i + 8] = m.matData[i * 4 + 2];
		result.matData[i + 12] = m.matData[i * 4 + 3];
	}
}

Matrix::~Matrix()
{

}
