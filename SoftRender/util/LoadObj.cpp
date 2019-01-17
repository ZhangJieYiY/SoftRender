#include "LoadObj.h"
#include <math.h>
#include <vector>
#include "FileUtil.h"
#include "../BaseDataClass/Object.h"
using namespace std;
bool tryParseDouble(const char *s, const char *s_end, double *result)
{
	if (s >= s_end)
	{
		return false;
	}
	double mantissa = 0.0;
	int exponent = 0;
	char sign = '+';
	char exp_sign = '+';
	char const *curr = s;
	int read = 0;
	bool end_not_reached = false;
	if (*curr == '+' || *curr == '-')
	{
		sign = *curr;
		curr++;
	}
	else if (isdigit(*curr)) { /* Pass through. */ }
	else
	{
		goto fail;
	}
	while ((end_not_reached = (curr != s_end)) && isdigit(*curr))
	{
		mantissa *= 10;
		mantissa += static_cast<int>(*curr - 0x30);
		curr++;	read++;
	}
	if (read == 0)
		goto fail;
	if (!end_not_reached)
		goto assemble;
	if (*curr == '.')
	{
		curr++;
		read = 1;
		while ((end_not_reached = (curr != s_end)) && isdigit(*curr))
		{
			mantissa += static_cast<int>(*curr - 0x30) * pow(10.0, -read);
			read++; curr++;
		}
	}
	else if (*curr == 'e' || *curr == 'E') {}
	else
	{
		goto assemble;
	}
	if (!end_not_reached)
		goto assemble;
	if (*curr == 'e' || *curr == 'E')
	{
		curr++;
		if ((end_not_reached = (curr != s_end)) && (*curr == '+' || *curr == '-'))
		{
			exp_sign = *curr;
			curr++;
		}
		else if (isdigit(*curr)) { /* Pass through. */ }
		else
		{
			goto fail;
		}
		read = 0;
		while ((end_not_reached = (curr != s_end)) && isdigit(*curr))
		{
			exponent *= 10;
			exponent += static_cast<int>(*curr - 0x30);
			curr++;	read++;
		}
		exponent *= (exp_sign == '+' ? 1 : -1);
		if (read == 0)
			goto fail;
	}
assemble:
	*result = (sign == '+' ? 1 : -1) * ldexp(mantissa * pow(5.0, exponent), exponent);
	return true;
fail:
	return false;
}
float parseFloat(const char* token)
{
	token += strspn(token, " \t");
#ifdef TINY_OBJ_LOADER_OLD_FLOAT_PARSER
	float f = atof(token);
#else
	const char *end = token + strcspn(token, " \t\r");
	double val = 0.0;
	tryParseDouble(token, end, &val);
	float f = static_cast<float>(val);
#endif
	return f;
}
int parseInt(const char *token) {
	token += strspn(token, " \t");
	int i = atoi(token);
	return i;
}

size_t  splitString(const string& strSrc, const string& strDelims, vector<string>& strDest)
{
	string delims = strDelims;
	string STR;
	if (delims.empty()) delims = " **";

	string::size_type pos = 0;
	string::size_type LEN = strSrc.size();
	while (pos < LEN) {
		STR = "";
		while ((delims.find(strSrc[pos]) != std::string::npos) && (pos < LEN))
		{
			++pos;
		}
		if (pos == LEN) {
			return strDest.size();
		}
		while ((delims.find(strSrc[pos]) == std::string::npos) && (pos < LEN))
		{
			STR += strSrc[pos++];
		}
		if (!STR.empty())
		{
			strDest.push_back(STR);
		}
	}
	return strDest.size();
}

Object* LoadObj::loadFromFile(const string& vname) {//��ȡobj �ļ��������ɻ������������ķ���
	Object * lo; //ָ�����ɵĻ�������������ָ��
	vector<float> alv;
	vector<float> alvResult;
	vector<float> alt;
	vector<float> altResult;
	vector<float> aln; //���ԭʼ���������ݵ��б�
	vector<float>alnResult; //��Ž�����������ݵ��б�
	std::string resultStr = FileUtil::loadAssetStr(vname);
	vector<string> lines;
	splitString(resultStr, "\n", lines);
	vector<string> splitStrs;
	vector<string> splitStrsF;
	string tempContents;
	for (int i = 0; i<lines.size(); i++)
	{
		tempContents = lines[i];
		if (tempContents.compare("") == 0)
		{
			continue;
		}
		string delims = "[ ]+";
		splitStrs.clear();
		splitString(tempContents, delims, splitStrs);
		if (splitStrs[0] == "v")
		{
			alv.push_back(parseFloat(splitStrs[1].c_str()));
			alv.push_back(parseFloat(splitStrs[2].c_str()));
			alv.push_back(parseFloat(splitStrs[3].c_str()));
		}
		else if (splitStrs[0] == "vt")
		{
			alt.push_back(parseFloat(splitStrs[1].c_str()));
			alt.push_back(1 - parseFloat(splitStrs[2].c_str()));
		}
		else if (splitStrs[0] == "vn")
		{ //������Ϊvn ��ͷ��Ϊ������������
			aln.push_back(parseFloat(splitStrs[1].c_str())); //��������X ��������ԭʼ�������б�
			aln.push_back(parseFloat(splitStrs[2].c_str())); //��������Y ��������ԭʼ�������б�
			aln.push_back(parseFloat(splitStrs[3].c_str())); //��������Z ��������ԭʼ�������б�
		}
		else if (splitStrs[0] == "f")
		{ //������Ϊf ��ͷ��Ϊ��������
			int index[3];
			string delimsF = "/";
			splitStrsF.clear();
			splitString(splitStrs[1].c_str(), delimsF, splitStrsF);
			index[0] = parseInt(splitStrsF[0].c_str()) - 1;
			alvResult.push_back(alv[3 * index[0]]);
			alvResult.push_back(alv[3 * index[0] + 1]);
			alvResult.push_back(alv[3 * index[0] + 2]);
			int indexTex = parseInt(splitStrsF[1].c_str()) - 1;
			altResult.push_back(alt[indexTex * 2]);
			altResult.push_back(alt[indexTex * 2 + 1]);
			int indexN = parseInt(splitStrsF[2].c_str()) - 1;//��ȡ���������1 ������ķ��������
			alnResult.push_back(aln[3 * indexN]); //����1 �����㷨������x �����������������б�
			alnResult.push_back(aln[3 * indexN + 1]); //����1 �����㷨������y �����������������б�
			alnResult.push_back(aln[3 * indexN + 2]); //����1 �����㷨������z �����������������б�
			splitStrsF.clear();
			splitString(splitStrs[2].c_str(), delimsF, splitStrsF);
			index[1] = parseInt(splitStrsF[0].c_str()) - 1;
			alvResult.push_back(alv[3 * index[1]]);
			alvResult.push_back(alv[3 * index[1] + 1]);
			alvResult.push_back(alv[3 * index[1] + 2]);
			indexTex = parseInt(splitStrsF[1].c_str()) - 1;
			altResult.push_back(alt[indexTex * 2]);
			altResult.push_back(alt[indexTex * 2 + 1]);
			indexN = parseInt(splitStrsF[2].c_str()) - 1;//��ȡ���������2 ������ķ��������
			alnResult.push_back(aln[3 * indexN]); //����2 �����㷨������x �����������������б�
			alnResult.push_back(aln[3 * indexN + 1]); //����2 �����㷨������y �����������������б�
			alnResult.push_back(aln[3 * indexN + 2]); //����2 �����㷨������z �����������������б�
			splitStrsF.clear();
			splitString(splitStrs[3].c_str(), delimsF, splitStrsF);
			index[2] = parseInt(splitStrsF[0].c_str()) - 1;
			alvResult.push_back(alv[3 * index[2]]);
			alvResult.push_back(alv[3 * index[2] + 1]);
			alvResult.push_back(alv[3 * index[2] + 2]);
			indexTex = parseInt(splitStrsF[1].c_str()) - 1;
			altResult.push_back(alt[indexTex * 2]);
			altResult.push_back(alt[indexTex * 2 + 1]);
			indexN = parseInt(splitStrsF[2].c_str()) - 1;//��ȡ���������3 ������ķ��������
			alnResult.push_back(aln[3 * indexN]); //����3 �����㷨������x �����������������б�
			alnResult.push_back(aln[3 * indexN + 1]); //����3 �����㷨������y �����������������б�
			alnResult.push_back(aln[3 * indexN + 2]); //����3 �����㷨������z �����������������б�
		}
		splitStrs.clear();
	}
	int vCount = (int)alvResult.size() / 3; //���㶥������
	double* vPosData = new double[vCount * 3]; //����������������
	double* vUVData = new double[vCount * 2]; //����������������
	double* vNormalData = new double[vCount * 3]; //����������������
	int indexTemp = 0; //��������
	for (int i = 0; i<vCount; i++) { //����ÿ������
		vPosData[i * 3 + 0] = alvResult[i * 3 + 0];
		vPosData[i * 3 + 1] = alvResult[i * 3 + 1];
		vPosData[i * 3 + 2] = alvResult[i * 3 + 2];
		vUVData[i * 2 + 0] = altResult[i * 2 + 0];
		vUVData[i * 2 + 1] = altResult[i * 2 + 1];
		vNormalData[i * 3 + 0] = alnResult[i * 3 + 0]; //��������x ����ת�浽��������������
		vNormalData[i * 3 + 1] = alnResult[i * 3 + 1]; //��������y ����ת�浽��������������
		vNormalData[i * 3 + 2] = alnResult[i * 3 + 2]; //��������z ����ת�浽��������������
	}
	lo = new Object(vPosData, vUVData, vNormalData, vCount);//�����������������
	return lo; //����ָ���������������ָ��
}
