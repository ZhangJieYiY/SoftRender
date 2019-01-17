
#include "Triangle.h"
#include "Vertex.h"
Triangle::Triangle(Vertex * _v1, Vertex * _v2, Vertex * _v3)
{
	verList.resize(3);				 
	verList[0] = _v1;
	verList[1] = _v2;
	verList[2] = _v3;
}


Triangle::Triangle() {

}


Triangle::~Triangle()
{
	size_t length = verList.size();
	for(int i=0;i<length;i++){
		Vertex* v = verList[i];
		if (v != nullptr)
		{
			delete v;

			v = nullptr;
		}
	}
	verList.clear();
	verList.swap(vector<Vertex*>());

}
