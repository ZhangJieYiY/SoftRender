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

TexDataObject* FileUtil::loadbmp_decode_file(const char *filename/*, unsigned char **imageData, unsigned int *width, unsigned int *height, unsigned int components*/)
{
	FILE *f = fopen(filename, "rb");

	if (!f)
	{
		printf("could not find file %s", filename);
		return nullptr;
	}

	unsigned char bmp_file_header[14];
	unsigned char bmp_info_header[40];
	unsigned char bmp_pad[3];

	int w, h;
	unsigned char *data = NULL;

	unsigned int x, y, i, padding;

	memset(bmp_file_header, 0, sizeof(bmp_file_header));
	memset(bmp_info_header, 0, sizeof(bmp_info_header));

	if (fread(bmp_file_header, sizeof(bmp_file_header), 1, f) == 0)
	{
		fclose(f);
		{
			printf("bmp file format invalid");
			return nullptr;
		}
	}

	if (fread(bmp_info_header, sizeof(bmp_info_header), 1, f) == 0)
	{
		fclose(f);
		{
			printf("bmp file FORMAT invalid");
			return nullptr;
		}
	}

	if ((bmp_file_header[0] != 'B') || (bmp_file_header[1] != 'M'))
	{
		fclose(f);
		{
			printf("bmp file SIGNATURE invalid");
			return nullptr;
		}
		fclose(f);
	}

	if ((bmp_info_header[14] != 24) && (bmp_info_header[14] != 32))
	{
		fclose(f);
		{
			printf("bmp file BITS_PER_PIXEL invalid");
			return nullptr;
		}
	}

	w = (bmp_info_header[4] + (bmp_info_header[5] << 8) + (bmp_info_header[6] << 16) + (bmp_info_header[7] << 24));
	h = (bmp_info_header[8] + (bmp_info_header[9] << 8) + (bmp_info_header[10] << 16) + (bmp_info_header[11] << 24));

	if ((w > 0) && (h > 0))
	{
		data = (unsigned char*)malloc(w * h * 4);

		if (!data)
		{
			fclose(f);
			{
				printf("bmp file OUT_OF_MEMORY");
				return nullptr;
			}
		}

		for (y = (h - 1); y != -1; y--)
		{
			for (x = 0; x < w; x++)
			{
				i = (x + y * w) * 4;

				if (fread(data + i, 3, 1, f) == 0)
				{
					free(data);

					fclose(f);
					{
						printf("bmp file FORMAT invalid");
						return nullptr;
					}
				}

				data[i] ^= data[i + 2] ^= data[i] ^= data[i + 2]; // BGR -> RGB
				data[i + 3] = 255;
			}

			padding = ((4 - (w * 3) % 4) % 4);

			if (fread(bmp_pad, 1, padding, f) != padding)
			{
				free(data);
				fclose(f);
				{
					printf("bmp file FORMAT invalid");
					return nullptr;
				}
			}
		}
	}

	//(*width) = w;
	//(*height) = h;
	//(*imageData) = data;

	fclose(f);

	TexDataObject* ctdo = new TexDataObject(w, h, data, w*h * 4);
	return ctdo;
}
