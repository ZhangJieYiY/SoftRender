#include "FileUtil.h"
#include "TexDataObject.h"
#include <fstream>
#include <windows.h>
#include <assert.h>
string FileUtil::loadAssetStr(string fname)
{
	ifstream infile;
	infile.open(fname.data());
	assert(infile.is_open());
	string ss;
	string s;
	while (getline(infile, s))
	{
		ss += s;
		ss += '\n';
	}
	infile.close();
	return ss;
}
int fromBytesToInt(unsigned char* buff)
{
	int k = 0;
	unsigned char* temp = (unsigned char*)(&k);
	temp[0] = buff[0];
	temp[1] = buff[1];
	temp[2] = buff[2];
	temp[3] = buff[3];
	return k;
}
TexDataObject* FileUtil::loadCommonTexData(string fname)
{
	BYTE *buf = new BYTE[4];
	BYTE *buf2 = new BYTE[4];
	char    c_file[100];
	strcpy(c_file, fname.c_str());
	FILE * fpPhoto;
	fpPhoto = fopen(c_file, "rb");
	assert(fpPhoto != NULL);
	fread(buf, 4, 1, fpPhoto);
	int width = fromBytesToInt(buf);
	fread(buf2, 4, 1, fpPhoto);
	int height = fromBytesToInt(buf2);
	unsigned char* data = new unsigned char[width*height * 4];
	fread(data, width*height * 4, 1, fpPhoto);
	fclose(fpPhoto);
	TexDataObject* ctdo = new TexDataObject(width, height, data, width*height * 4);
	delete[]buf;
	delete[]buf2;
	return ctdo;
}
