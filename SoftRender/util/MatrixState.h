#pragma once
class Camera;
class Matrix;
class Vector3;
class MatrixState
{
public:
	static Matrix* mMMatrix;
	static Matrix* mVMatrix;
	static Matrix* mPMatrix;
	static Matrix* mMVPMatrix;
	static Matrix* mMVMatrix;
	static double mStack[10][16];
	static int stackTop;
	static void setInitStack();
	static void pushMatrix();
	static void popMatrix();
	static void translate(float x, float y, float z);
	static void rotate(float x, float y, float z);
	static void scale(float x, float y, float z);
	static void setCamera(Matrix &vm, const Vector3 &pos, const Vector3 &target, const Vector3 &up);
	static void setFrustumM(Matrix &pm, float left, float right, float top, float bottom, float near, float far);
	static Matrix* getMMatrix();
	static Matrix* getMVPMatrix();
	static Matrix* getMVMatrix();
	static Matrix* getPMatrix();
};

