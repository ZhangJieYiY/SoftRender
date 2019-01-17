
#include "../util/MatrixState.h"
#include "../util/Constant.h"
#include "Vector3.h"
#include "Matrix.h"
#include "Camera.h"
#include <math.h>
#define CameraSpeed 0.5f
#define MAX_PITCH_ANGLE 89.0f
#define CameraRadiu 50.0f


Camera::Camera()
{
	this->pos = new Vector3(0, 0, -50);
	this->tar = new Vector3(0, 0, 0);
	this->up = new Vector3(0, 1, 0);
	this->vm = new Matrix();
	this->pm = new Matrix();
	this->forward = new Vector3(0,0,1);
	this->right = new Vector3(1,0,0);
}

void Camera::calCameraDirction(float xoffset,float yoffset){

	this->pitch += yoffset;
	this->yaw += xoffset;
	// ��֤�Ƕ��ں���Χ��
	this->pitch = min(this->pitch, MAX_PITCH_ANGLE);
	this->pitch = max(this->pitch, -MAX_PITCH_ANGLE);
	if (yaw < 0.0f)
		yaw += 360.0f;
	if (yaw > 360.0f)
		yaw -= 360.0f;


	//���¼���forward����
	forward->x = -sin(yaw*PI / 180.0f) * cos(pitch*PI / 180.0f);
	forward->y = sin(pitch*PI / 180.0f);
	forward->z = cos(yaw*PI / 180.0f) * cos(pitch*PI / 180.0f);
	*forward = Vector3::normalize(*forward);

	//���¼���right����
	right->x = cos(yaw*PI / 180.0f);
	right->y = 0;
	right->z = sin(yaw*PI / 180.0f);
	*right = Vector3::normalize(*right);

	*tar = *pos + *forward*CameraRadiu;
	MatrixState::setCamera(*vm, *pos, *tar, *up);
}

void Camera::calCameraPosition(float forward_scalar, float right_scalar) {

	//���¼������λ��

	
	Vector3 moveDir = (*forward) *forward_scalar + (*right)*right_scalar;//�ƶ�����
	moveDir = Vector3::normalize(moveDir);
	*pos = *pos + moveDir*CameraSpeed;//���λ��
	*tar = *pos + *forward*CameraRadiu;//�۲�Ŀ���
	MatrixState::setCamera(*vm, *pos, *tar, *up);//�������
}

void Camera::setFrustum(float left, float right,  float top, float bottom, float near, float far)
{
	l = left;
	r = right;
	t = top;
	b = bottom;
	n = near;
	f = far;
	MatrixState::setFrustumM(*pm,l,r,t,b,n,f);
}

Camera::~Camera()
{
	delete pos;
	delete tar;
	delete up;
	delete vm;
	delete pm;
	delete forward;
	delete right;
}
