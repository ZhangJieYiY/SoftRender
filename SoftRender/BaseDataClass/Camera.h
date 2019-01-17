#pragma once
class Vector3;
class Matrix;
class Camera
{
public:
	Camera();
	float pitch = 0.0f, yaw = 0.0f;
	Vector3 *pos, *tar, *up;
	Matrix *vm;
	Matrix *pm;
	Vector3 *forward,*right;
	float l=0.0f, r = 0.0f, t = 0.0f, b = 0.0f, n = 0.0f, f = 0.0f;
	void calCameraDirction(float, float);
	void calCameraPosition(float, float);
	void setFrustum(float left,float right,float top,float bottom,float _near,float _far);
	~Camera();
};

