#pragma once
#include <string>
class TexDataObject;
using namespace std;
class FileUtil
{
public:
	static string loadAssetStr(string fname);
	static TexDataObject* loadCommonTexData(string fname);
	static TexDataObject* loadbmp_decode_file(const char *filename/*, unsigned char **imageData, unsigned int *width, unsigned int *height, unsigned int components*/);
};

