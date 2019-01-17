#pragma once
#include<vector>

class Vertex;
using namespace std;
class Triangle
{

public:
	Triangle();
	Triangle(Vertex *_v1, Vertex *_v2, Vertex *_v3);
	bool clipped=false;
	~Triangle();
	vector<Vertex*> verList;//∂•µ„¡–±Ì
};

