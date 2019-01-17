#include "MatrixState.h"
#include <iostream>
#include "../BaseDataClass/Vector3.h"
#include "../BaseDataClass/Matrix.h"

Matrix* MatrixState::mMMatrix = new Matrix();
Matrix* MatrixState::mVMatrix = new Matrix();
Matrix* MatrixState::mPMatrix = new Matrix();
Matrix* MatrixState::mMVPMatrix = new Matrix();
Matrix* MatrixState::mMVMatrix = new Matrix();
double MatrixState::mStack[10][16];
int MatrixState::stackTop = -1;
void MatrixState::setInitStack()
{
	mMMatrix->setIdentity();
}
void MatrixState::pushMatrix()
{
	stackTop++;
	memcpy(&mStack[stackTop][0], &mMMatrix->matData[0],sizeof(double)*16);
}
void MatrixState::popMatrix()
{
	memcpy(&mMMatrix->matData[0], &mStack[stackTop][0],sizeof(double) * 16);
	stackTop--;
}
void MatrixState::translate(float x, float y, float z)
{
	mMMatrix->translateM(x, y, z);
}
void MatrixState::rotate(float x, float y, float z)
{
	mMMatrix->rotateMXYZ(x, y, z);
}
void MatrixState::scale(float x, float y, float z)
{
	mMMatrix->scaleM(x, y, z);
}

void MatrixState::setCamera(Matrix &vm, const Vector3 &pos,const Vector3 &target,const Vector3 &up) {
	Matrix::setLookatM
	(
		vm,
		pos.x,
		pos.y,
		pos.z,
		target.x,
		target.y,
		target.z,
		up.x,
		up.y,
		up.z
	); 
}
void MatrixState::setFrustumM
(
	Matrix &pm,
	float left,
	float right,
	float top,
	float bottom,
	float near,
	float far
){
	Matrix::setPerspectiveM(pm, left, right, top, bottom, near, far);
}
Matrix* MatrixState::getMMatrix() {
	return mMMatrix;
}
Matrix* MatrixState::getPMatrix() {
	return mPMatrix;
}
Matrix* MatrixState::getMVPatrix() {
	Matrix::multipMM(*mMVPMatrix, *mVMatrix, *mMMatrix);
	Matrix::multipMM(*mMVPMatrix, *mPMatrix, *mMVPMatrix);
	return mMVPMatrix;
}
Matrix* MatrixState::getMVatrix() {
	Matrix::multipMM(*mMVMatrix, *mVMatrix, *mMMatrix);
	return mMVMatrix;
}