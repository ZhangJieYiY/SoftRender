#pragma once
class Object;
#include <string>
using namespace std;
class LoadObj
{
public:
	static Object* loadFromFile(const string& fname);
};
