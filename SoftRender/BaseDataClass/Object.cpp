#include "Object.h"

#include "../main/RenderPipeline.h"
#include "../BaseDataClass/Vertex.h"
#include "../BaseDataClass/Triangle.h"
#include "../BaseDataClass/Vector3.h"
#include "../util/TexDataObject.h"

Object::Object():ka(1.0f), kd(1.0f), ks(1.0f)
{
}

Object::Object(double *_vPosData, double *_vUVData, double *_vNormalData, int _vCount) {
	ka = 1.0;
	kd = 1.0;
	ks = 1.0;
	for (int i = 0; i < _vCount/3; i++) {//遍历所有三角形
		Vector3 pos1 = Vector3(_vPosData[i * 9 + 0], _vPosData[i * 9 + 1], _vPosData[i * 9 + 2]);
		Vector3 pos2 = Vector3(_vPosData[i * 9 + 3], _vPosData[i * 9 + 4], _vPosData[i * 9 + 5]);
		Vector3 pos3 = Vector3(_vPosData[i * 9 + 6], _vPosData[i * 9 + 7], _vPosData[i * 9 + 8]);

		Vector3 nor1 = Vector3(_vNormalData[i * 9 + 0], _vNormalData[i * 9 + 1], _vNormalData[i * 9 + 2]);
		Vector3 nor2 = Vector3(_vNormalData[i * 9 + 3], _vNormalData[i * 9 + 4], _vNormalData[i * 9 + 5]);
		Vector3 nor3 = Vector3(_vNormalData[i * 9 + 6], _vNormalData[i * 9 + 7], _vNormalData[i * 9 + 8]);

		Vertex *v1 = new Vertex(pos1.x, pos1.y, pos1.z, nor1.x, nor1.y, nor1.z, _vUVData[i * 6 + 0], _vUVData[i * 6 + 1]);
		Vertex *v2 = new Vertex(pos2.x, pos2.y, pos2.z, nor2.x, nor2.y, nor2.z, _vUVData[i * 6 + 2], _vUVData[i * 6 + 3]);
		Vertex *v3 = new Vertex(pos3.x, pos3.y, pos3.z, nor3.x, nor3.y, nor3.z, _vUVData[i * 6 + 4], _vUVData[i * 6 + 5]);
		Triangle *tri = new Triangle(v1,v2,v3);
		addTriangle(tri);
	}
}

void Object::addTriangle(Triangle *triangle) {
	triangleList.push_back(triangle);
}

void Object::drawSelf()
{
	rp->bindSampler(tdoList);
	rp->pushConstant();//推送常量
	rp->draw(*this);
}

Object::~Object()
{
	for (TexDataObject * tdo: tdoList)
	{
		if (tdo != nullptr) {
			delete[]tdo;
			tdo = nullptr;
		}
	}
	tdoList.clear();
	tdoList.swap(vector<TexDataObject *>());
}
