#pragma once
#include <string>
class TexDataObject;
using namespace std;
class FileUtil
{
public:
	static string loadAssetStr(string fname);
	static TexDataObject* loadCommonTexData(string fname);
};

