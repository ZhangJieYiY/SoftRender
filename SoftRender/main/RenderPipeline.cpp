
#include "RenderPipeline.h"
#include <math.h>
#include <cstring>
#include <atlstr.h>
#include "../BaseDataClass/Matrix.h"
#include "../BaseDataClass/Object.h"
#include "../BaseDataClass/Vector4.h"
#include "../BaseDataClass/Vector3.h"
#include "../BaseDataClass/Triangle.h"
#include "../BaseDataClass/Vertex.h"
#include "../BaseDataClass/Camera.h"
#include "../util/MatrixState.h"
#include "../util/TexDataObject.h"
#include "../util/FPSUtil.h"
#include "../util/UserData.h"
//#define random(x) (rand()%x)
#define SWAP(a,b) {a=a+b; b=a-b; a=a-b;}

using namespace std;

RenderPipeline::RenderPipeline()
{
	mMatrix = new Matrix();//变换矩阵
	MVMatrix = new Matrix();//变换到摄像机空间的矩阵
	MPMatrix = new Matrix();//投影矩阵
	MVPMatrix = new Matrix();//综合矩阵
}

bool RenderPipeline::createRenderPipline(RasterizationStateInfo &_rasterizationStateInfo)
{
	rasterizationStateInfo = _rasterizationStateInfo;
	return true;
}

void RenderPipeline::clearDepthBuffer() {
	memset(&depthBuffer[0][0], 0, sizeof(double)*windowWidth*windowHeight);
}

void RenderPipeline::clearColorBuffer() {
	for (int i = 0; i < windowHeight; i++)
	{
		for (int j = 0; j < windowWidth; j++)
		{
			colorBufferForDraw[i][j] = clearColor;
		}
	}
}

void RenderPipeline::setColorBuffer(int height, int width, const Color &color) {
	if (height < windowHeight&&width < windowWidth&&height>0 && width>0) {
		colorBufferForDraw[height][width] = color;
		//colorBufferForDraw[height][width] = Color(random(255), random(255), random(255));
	}
}

bool RenderPipeline::setDepthBuffer(int height, int width, double depth) {
	if (height < windowHeight&&width < windowWidth&&height>0 && width>0) {
		if (depth > depthBuffer[height][width]) {
			depthBuffer[height][width] = depth;
			return true;
		}
		else {
			return false;
		}
	}
	else {
		return false;
	}
}

void RenderPipeline::setPixel(Point &p)
{
	TexDataObject *tdo = samplerData[0];
	switch (rasterizationStateInfo.colorMode) {
	case Ver_Color:
		break;
	case Tex_Sample:
		double s = p.S;
		double t = p.T;
		s = max(s, 0);
		s = min(s, 1);
		t = max(t, 0);
		t = min(t, 1);
		int row = (t*tdo->height);
		int col = (s*tdo->width);

		row = row < tdo->height ? row : row%tdo->height;
		col = col < tdo->width ? col : col%tdo->width;


		long valueIndex = row * tdo->width * 4 + col * 4;
		//unsigned char r = tdo->data[valueIndex + 0];
		//unsigned char g = tdo->data[valueIndex + 1];
		//unsigned char b = tdo->data[valueIndex + 2];
		p.color.R = tdo->data[valueIndex + 0];
		p.color.G = tdo->data[valueIndex + 1];
		p.color.B = tdo->data[valueIndex + 2];
		break;
	}

	p.lightResult = max(0.0, p.lightResult);
	p.lightResult = min(1.0, p.lightResult);

	if (!noLight)
		p.color = Color((unsigned char)(p.color.R*p.lightResult), (unsigned char)(p.color.G*p.lightResult), (unsigned char)(p.color.B*p.lightResult));

	if (depthTestAble)//开启了深度测试
	{
		if (setDepthBuffer(p.y, p.x, p.w))//深度值写入成功
		{
			//int c = 1 / p.w;
			//c /= 3;
			//setColorBuffer(p.y, p.x, Color(c,c,c));
			setColorBuffer(p.y, p.x, p.color);
		}
	}
	else {
		setColorBuffer(p.y, p.x, p.color);
	}
}

void RenderPipeline::draw(const Object &obj) {
	Matrix traM;
	if ((lightAble && !noLight) || rasterizationStateInfo.cullMode != NONE) {//如果需要光照提前计算出逆转置矩阵
		Matrix invM = Matrix();
		traM = Matrix();
		Matrix::invertM(invM, *mMatrix);//求变换矩阵的逆矩阵
		Matrix::transposeM(traM, invM);//求逆矩阵的转置矩阵
	}
	vector<Triangle*> camSpaceTri;
	Vertex newV[3];
	Vector4 vec4Temp;
	for (Triangle *tri : obj.triangleList)
	{
		if (backCullTest(*tri, traM))//背面剪裁返回true表示剪裁成功
		{
			continue;
		}
		for (int i = 0; i < 3; i++)
		{
			Vertex *ver = tri->verList[i];
			if (ver == nullptr)break;
			vec4Temp.x = ver->x;
			vec4Temp.y = ver->y;
			vec4Temp.z = ver->z;
			vec4Temp.w = 1;
			Matrix::multipVM(vec4Temp, vec4Temp, *MVMatrix);//变换到摄像机空间
			newV[i] = Vertex(vec4Temp.x, vec4Temp.y, vec4Temp.z, ver->nx, ver->ny, ver->nz, ver->s, ver->t, ver->color);
		}
		camSpaceTri.push_back(new Triangle(
			new Vertex(newV[0].x, newV[0].y, newV[0].z, newV[0].nx, newV[0].ny, newV[0].nz, newV[0].s, newV[0].t, newV[0].color),
			new Vertex(newV[1].x, newV[1].y, newV[1].z, newV[1].nx, newV[1].ny, newV[1].nz, newV[1].s, newV[1].t, newV[1].color),
			new Vertex(newV[2].x, newV[2].y, newV[2].z, newV[2].nx, newV[2].ny, newV[2].nz, newV[2].s, newV[2].t, newV[2].color)
		));
	}
	RenderListClip(camSpaceTri, (ClipPlaneFlag(CLIP_POLY_Z_PLANE)));

	Point p[3] = {};
	for (Triangle *tri : camSpaceTri)
	{
		if (tri->clipped)
		{
			continue;
		}
		for (int i = 0; i < 3; i++)
		{
			if (lightAble && !noLight)//计算光照
			{
				calLightResult(p[i], obj, *tri->verList[i], traM);
			}

			Vertex *ver = tri->verList[i];
			vec4Temp.x = ver->x;
			vec4Temp.y = ver->y;
			vec4Temp.z = ver->z;
			vec4Temp.w = 1;
			Matrix::multipVM(vec4Temp, vec4Temp, *MPMatrix);//透视矩阵
			vec4Temp.w = max(vec4Temp.w, 0.00000001);
			p[i].w = 1.0 / vec4Temp.w;
			vec4Temp = Vector4(//透视除法
				vec4Temp.x / vec4Temp.w,
				vec4Temp.y / vec4Temp.w,
				vec4Temp.z / vec4Temp.w,
				vec4Temp.w / vec4Temp.w
			);
			p[i].x = ((vec4Temp.x + 1) / 2 * windowWidth);
			p[i].y = ((1 - vec4Temp.y) / 2 * windowHeight);
			p[i].color = ver->color;
			p[i].S = ver->s;
			p[i].T = ver->t;
		}
		switch (rasterizationStateInfo.drawMode) {
		case MY_POINT:
			setPixel(p[0]);
			setPixel(p[1]);
			setPixel(p[2]);
			break;
		case MY_LINE:
			drawLine(p[0], p[1]);
			drawLine(p[1], p[2]);
			drawLine(p[2], p[0]);
			break;
		case MY_FILL:
			if (rasterizationStateInfo.colorMode == Ver_Color)
			{
				gouraudTriangleForVerColor(p[0], p[1], p[2]);
			}
			else if (rasterizationStateInfo.colorMode == Tex_Sample)
			{
				gouraudTriangleForTexSample(p[0], p[1], p[2]);
			}
			break;
		}
	}

	for (Triangle *tri : camSpaceTri) {
		if (tri != nullptr)
		{
			delete tri;
			tri = nullptr;
		}
	}
	camSpaceTri.clear();
	camSpaceTri.swap(vector<Triangle*>());

}

bool RenderPipeline::backCullTest(const Triangle &tri, const Matrix &traM) {
	if (tri.verList.size()==0)return true;
	if (rasterizationStateInfo.cullMode != NONE)//需要剪裁
	{
		/*背面剪裁*/
		Vector4 v4T = Vector4(tri.verList[0]->x, tri.verList[0]->y, tri.verList[0]->z, 1);
		Matrix::multipVM(v4T, v4T, *mMatrix);//变换矩阵
		Vector3 viewRay = Vector3(currCamera->pos->x - v4T.x, currCamera->pos->y - v4T.y, currCamera->pos->z - v4T.z);//视线向量
		viewRay = Vector3::normalize(viewRay);
		Vector3 faceNormal = Vector3(0, 0, 0);//面法向量
		Vector3 _10 = Vector3(tri.verList[1]->x - tri.verList[0]->x, tri.verList[1]->y - tri.verList[0]->y, tri.verList[1]->z - tri.verList[0]->z);
		Vector3 _20 = Vector3(tri.verList[2]->x - tri.verList[0]->x, tri.verList[2]->y - tri.verList[0]->y, tri.verList[2]->z - tri.verList[0]->z);
		faceNormal = Vector3::crossProduct(_10, _20);

		Vector4 vFNormal4 = Vector4(faceNormal.x, faceNormal.y, faceNormal.z, 1);
		Matrix::multipVM(vFNormal4, vFNormal4, traM);//将面法向量转换到世界空间中
		faceNormal = Vector3(vFNormal4.x, vFNormal4.y, vFNormal4.z);//视线向量
		faceNormal = Vector3::normalize(faceNormal);

		double dot = Vector3::dotProduct(viewRay, faceNormal);
		if (rasterizationStateInfo.cullMode == BACK)
		{
			if (dot < 0)return true;
		}
		else if (rasterizationStateInfo.cullMode == FRONT)
		{
			if (dot > 0)return true;
		}
		return false;
	}
	else {
		return false;
	}
}

void RenderPipeline::calLightResult(Point &p, const Object &obj, const Vertex &ver, const Matrix &traM)
{
	double lightResult = 0.0;
	//环境光计算
	double ambient = obj.ka*ambientLight;//环境光结果
										  //漫反射计算
	Vector3 I = Vector3(-lightDirection.x, -lightDirection.y, -lightDirection.z);//入射光向量
	Vector3 normal = Vector3(ver.nx, ver.ny, ver.nz);
	Vector4 newNormal4 = Vector4(normal);
	Matrix::multipVM(newNormal4, newNormal4, traM);//对向量进行变换
	Vector3 N = Vector3(newNormal4.x, newNormal4.y, newNormal4.z);//求得变换后的矩阵
	I = Vector3::normalize(I);
	N = Vector3::normalize(N);
	double diffuse = obj.kd*max(0, Vector3::dotProduct(I, N))*directionalLight;//漫反射结果
																				//镜面光计算
	Vector4 vPos4 = Vector4(ver.x, ver.y, ver.z, 1);
	Matrix::multipVM(vPos4, vPos4, *mMatrix);//将点变换到世界空间中
	Vector3 E = Vector3(currCamera->pos->x - vPos4.x, currCamera->pos->y - vPos4.y, currCamera->pos->z - vPos4.z);//视线向量
	E = Vector3::normalize(E);//
	Vector3 H = E + I;
	H = Vector3(H);//入射向量与视线向量的半角向量
	double specular = obj.ks*pow(max(0, Vector3::dotProduct(I, E)), 50);
	lightResult += ambient + diffuse + specular;
	p.lightResult = lightResult;
}

void RenderPipeline::present(HDC hdc) {
	BITMAPINFO bmi;
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biHeight = -windowHeight;
	bmi.bmiHeader.biWidth = windowWidth;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 24;
	bmi.bmiHeader.biCompression = BI_RGB;
	SetDIBitsToDevice(
		hdc, 0, 0,//矩形区域左上角位置坐标
		windowWidth, windowHeight,//位图尺寸
		0, 0,//位图在矩形区域的位置坐标
		0,//从什么位置开始显示，哪一行开始
		windowHeight,//扫描线数量
		colorBufferForDraw,
		&bmi,
		DIB_RGB_COLORS
	);
	//printf("FPS:%d",FPSUtil::currFPS);
	CString str;
	str.Format("%d", FPSUtil::currFPS);
	TextOut(hdc, 20, 20, "FPS:", 4);
	TextOut(hdc, 50, 20, str, str.GetLength());
}

void RenderPipeline::gouraudTriangleForVerColor(const Point &p1, const Point &p2, const Point &p3) //gouraud法填充插值颜色深度与光照
{
	Point pt[3] = { p1,p2,p3 };

	//交换,使pt[v1]点为最上面的顶点(y值最小的点)
	int v1 = 0, v2 = 1, v3 = 2;
	if (pt[v1].y > pt[v2].y)
		SWAP(v1, v2);
	if (pt[v1].y > pt[v3].y)
		SWAP(v1, v3);
	double slope_start, slope_end;//三角形边的斜率
	double r1, g1, b1, r2, g2, b2, r3, g3, b3;
	double w1_reci, w2_reci, w3_reci;
	double l1, l2, l3;

	double slope_start_w_reci, slope_end_w_reci;//扫描线起始值所在的三角形边和结束值所在的三角形边对应的斜率(w值为其倒数的斜率)
	double slope_start_r, slope_start_g, slope_start_b;
	double slope_end_r, slope_end_g, slope_end_b;

	int x, y;
	int xs, xe;
	double dw_reci_start = 0, dw_reci_end = 0;//两根扫描线坐标变化值
	double dx_start = 0, dx_end = 0;
	double dr_start = 0, dg_start = 0, db_start = 0;
	double dr_end = 0, dg_end = 0, db_end = 0;

	double ws_reci = 0, we_reci = 0;//扫描线插值起止值
	double ls, le;
	double rs, re, gs, ge, bs, be;

	double dr_x, dg_x, db_x, dw_reci_x;//扫描线插值步进
	double r, g, b;
	double l;
	double w_reci;
	double dx, dy;

	Point interPt;//插值得到的点
	if (pt[v1].y == pt[v2].y || pt[v1].y == pt[v3].y)//平顶
	{
		//交换,使pt[v1]点为最左边的顶点
		if (pt[v1].y == pt[v2].y)
		{
			if (pt[v1].x > pt[v2].x)
				SWAP(v1, v2);
		}
		else
		{
			if (pt[v1].x > pt[v3].x)
				SWAP(v1, v3);
		}

		//交换,使平顶三角形的左上为pt[v1],右上为pt[v3],下顶点为pt[v2]
		if (pt[v3].y > pt[v2].y)
			SWAP(v2, v3);

		dy = max(pt[v2].y - pt[v3].y, 0.00000001);//计算出y跨度
		slope_start = (pt[v2].x - pt[v1].x)*1.0 / dy;//左边斜边的斜率
		slope_end = (pt[v2].x - pt[v3].x)*1.0 / dy;//右边斜边的斜率

		GetRGBValue(pt[v1].color, r1, g1, b1);
		GetRGBValue(pt[v2].color, r2, g2, b2);
		GetRGBValue(pt[v3].color, r3, g3, b3);
		w1_reci = pt[v1].w;
		w2_reci = pt[v2].w;
		w3_reci = pt[v3].w;
		l1 = pt[v1].lightResult;
		l2 = pt[v2].lightResult;
		l3 = pt[v3].lightResult;

		slope_start_r = (r2 - r1) / dy;
		slope_start_g = (g2 - g1) / dy;
		slope_start_b = (b2 - b1) / dy;
		slope_end_r = (r2 - r3) / dy;
		slope_end_g = (g2 - g3) / dy;
		slope_end_b = (b2 - b3) / dy;
		slope_start_w_reci = (w2_reci - w1_reci) / dy;
		slope_end_w_reci = (w2_reci - w3_reci) / dy;

		int yStart = ceil(pt[v1].y);
		int yEnd = ceil(pt[v2].y) - 1;
		/*垂直裁剪*/
		if (yStart > windowHeight)return;//起始值超出屏幕下边缘
		if (yStart < 0)yStart = 0;//高于屏幕上边缘，重新赋值，从上边缘开始计算
		if (yEnd < 0)return;//结束值高于屏幕上边缘
		if (yEnd > windowHeight)yEnd = windowHeight;//结束值超出屏幕下边缘，重新赋值，从上边缘结束计算


		dx_start = (yStart - pt[v1].y)*slope_start;
		dx_end = (yStart - pt[v3].y)*slope_end;
		dr_start = (yStart - pt[v1].y)*slope_start_r;
		dr_end = (yStart - pt[v3].y)*slope_end_r;
		dg_start = (yStart - pt[v1].y)*slope_start_g;
		dg_end = (yStart - pt[v3].y)*slope_end_g;
		db_start = (yStart - pt[v1].y)*slope_start_b;
		db_end = (yStart - pt[v3].y)*slope_end_b;
		dw_reci_start = (yStart - pt[v1].y)*slope_start_w_reci;
		dw_reci_end = (yStart - pt[v3].y)*slope_end_w_reci;
		for (y = yStart; y <= yEnd; y++)//从上往下一条条扫描
		{
			xs = (pt[v1].x + dx_start + 0.5);//插值开始的X坐标
			xe = (pt[v3].x + dx_end + 0.5);//插值结束的X坐标

			rs = r1 + dr_start;
			gs = g1 + dg_start;
			bs = b1 + db_start;
			re = r3 + dr_end;
			ge = g3 + dg_end;
			be = b3 + db_end;


			ws_reci = w1_reci + dw_reci_start;
			we_reci = w3_reci + dw_reci_end;
			dx = max(xe - xs, 0.00000000000001);
			dr_x = (re - rs)*1.0 / dx;
			dg_x = (ge - gs)*1.0 / dx;
			db_x = (be - bs)*1.0 / dx;
			dw_reci_x = (we_reci - ws_reci) / dx;

			r = rs;
			g = gs;
			b = bs;
			w_reci = ws_reci;

			//扫描线纹理坐标起始值计算
			ls = interAttriByW(pt[v1].x, pt[v1].y, pt[v2].x, pt[v2].y, xs, y, w1_reci, w2_reci, ws_reci, l1, l2);
			le = interAttriByW(pt[v2].x, pt[v2].y, pt[v3].x, pt[v3].y, xe, y, w2_reci, w3_reci, we_reci, l2, l3);

			for (x = xs; x <= xe; x++)//扫描线
			{
				interPt.x = x;
				interPt.y = y;
				interPt.w = w_reci;
				interPt.color.R = (unsigned char)r;
				interPt.color.G = (unsigned char)g;
				interPt.color.B = (unsigned char)b;
				if (lightAble)
				{
					l = interAttriByW(xs, y, xe, y, x, y, ws_reci, we_reci, w_reci, ls, le);
					interPt.lightResult = l;
				}
				setPixel(interPt);
				r += dr_x;
				g += dg_x;
				b += db_x;
				w_reci += dw_reci_x;
			}

			dx_start += slope_start;
			dx_end += slope_end;

			dr_start += slope_start_r;
			dg_start += slope_start_g;
			db_start += slope_start_b;
			dr_end += slope_end_r;
			dg_end += slope_end_g;
			db_end += slope_end_b;
			dw_reci_start += slope_start_w_reci;
			dw_reci_end += slope_end_w_reci;

		}
	}
	else if (pt[v2].y == pt[v3].y)//平底
	{
		//交换,使之最上顶点为pt[v1],平底边左端点为pt[v2],右端点为pt[v3]
		if (pt[v2].x > pt[v3].x)
			SWAP(v2, v3);

		dy = max(pt[v2].y - pt[v1].y, 0.00000001);
		slope_start = (pt[v2].x - pt[v1].x)*1.0 / dy;
		slope_end = (pt[v3].x - pt[v1].x)*1.0 / dy;

		GetRGBValue(pt[v1].color, r1, g1, b1);
		GetRGBValue(pt[v2].color, r2, g2, b2);
		GetRGBValue(pt[v3].color, r3, g3, b3);
		w1_reci = pt[v1].w;
		w2_reci = pt[v2].w;
		w3_reci = pt[v3].w;

		l1 = pt[v1].lightResult;
		l2 = pt[v2].lightResult;
		l3 = pt[v3].lightResult;


		slope_start_r = (r2 - r1) / dy;
		slope_start_g = (g2 - g1) / dy;
		slope_start_b = (b2 - b1) / dy;
		slope_end_r = (r3 - r1) / dy;
		slope_end_g = (g3 - g1) / dy;
		slope_end_b = (b3 - b1) / dy;
		slope_start_w_reci = (w2_reci - w1_reci) / dy;
		slope_end_w_reci = (w3_reci - w1_reci) / dy;

		//垂直裁剪
		int yStart = ceil(pt[v1].y);
		int yEnd = ceil(pt[v2].y) - 1;
		if (yStart > windowHeight)return;//起始值超出屏幕下边缘
		if (yStart < 0)yStart = 0;//高于屏幕上边缘，重新赋值，从上边缘开始计算
		if (yEnd < 0)return;//结束值高于屏幕上边缘
		if (yEnd > windowHeight)yEnd = windowHeight;//结束值超出屏幕下边缘，重新赋值，从上边缘结束计算


		dx_start = (yStart - pt[v1].y)*slope_start;
		dx_end = (yStart - pt[v1].y)*slope_end;
		dr_start = (yStart - pt[v1].y)*slope_start_r;
		dr_end = (yStart - pt[v1].y)*slope_end_r;
		dg_start = (yStart - pt[v1].y)*slope_start_g;
		dg_end = (yStart - pt[v1].y)*slope_end_g;
		db_start = (yStart - pt[v1].y)*slope_start_b;
		db_end = (yStart - pt[v1].y)*slope_end_b;
		dw_reci_start = (yStart - pt[v1].y)*slope_start_w_reci;
		dw_reci_end = (yStart - pt[v1].y)*slope_end_w_reci;
		for (y = yStart; y <= yEnd; y++)
		{
			xs = (pt[v1].x + dx_start + 0.5);
			xe = (pt[v1].x + dx_end + 0.5);
			rs = r1 + dr_start;
			gs = g1 + dg_start;
			bs = b1 + db_start;
			re = r1 + dr_end;
			ge = g1 + dg_end;
			be = b1 + db_end;


			ws_reci = w1_reci + dw_reci_start;
			we_reci = w1_reci + dw_reci_end;

			dx = max(xe - xs, 0.00000001);
			dr_x = (re - rs) / dx;
			dg_x = (ge - gs) / dx;
			db_x = (be - bs) / dx;
			dw_reci_x = (we_reci - ws_reci) / dx;

			r = rs;
			g = gs;
			b = bs;
			w_reci = ws_reci;


			//扫描线纹理坐标起始值计算
			ls = interAttriByW(pt[v1].x, pt[v1].y, pt[v2].x, pt[v2].y, xs, y, w1_reci, w2_reci, ws_reci, l1, l2);
			le = interAttriByW(pt[v1].x, pt[v1].y, pt[v3].x, pt[v3].y, xe, y, w1_reci, w3_reci, we_reci, l1, l3);
			for (x = xs; x <= xe; x++)//扫描线
			{
				interPt.x = x;
				interPt.y = y;
				interPt.w = w_reci;
				interPt.color.R = (unsigned char)r;
				interPt.color.G = (unsigned char)g;
				interPt.color.B = (unsigned char)b;
				if (lightAble)
				{
					l = interAttriByW(xs, y, xe, y, x, y, ws_reci, we_reci, w_reci, ls, le);
					interPt.lightResult = l;
				}
				setPixel(interPt);
				r += dr_x;
				g += dg_x;
				b += db_x;
				w_reci += dw_reci_x;
			}

			dx_start += slope_start;
			dx_end += slope_end;

			dr_start += slope_start_r;
			dg_start += slope_start_g;
			db_start += slope_start_b;
			dr_end += slope_end_r;
			dg_end += slope_end_g;
			db_end += slope_end_b;
			dw_reci_start += slope_start_w_reci;
			dw_reci_end += slope_end_w_reci;
		}
	}
	else  //任意三角形  目前p1已经是最上面的点了，只需要考虑p2和p3的上下关系
	{
		//然后按照pt[v2]  pt[v3]的上下位置关系进行区分
		//如果pt[v2]在上面就利用pt[v1]  pt[v2]  以及线段pt[v1] pt[v3]上y值等于pt[v2].y的点组成一个平底三角形
		if (pt[v2].y < pt[v3].y)
		{
			Point p1 = pt[v1];
			Point p2 = pt[v2];
			Point p3;
			Point p4 = pt[v3];

			//首先求出p3的x值  他在线段pt[v1] pt[v3]上
			p3.y = pt[v2].y;
			p3.x = ((p3.y - pt[v1].y)*((double)(pt[v3].x - pt[v1].x) / (double)(pt[v3].y - pt[v1].y)) + pt[v1].x);

			p3.color = interColor(pt[v1], pt[v3], p3);
			p3.w = interWReci(pt[v1].x, pt[v1].y, pt[v3].x, pt[v3].y, pt[v1].w, pt[v3].w, p3.x, p3.y);
			p3.lightResult = interAttriByW(pt[v1].x, pt[v1].y, pt[v3].x, pt[v3].y, p3.x, p3.y, pt[v1].w, pt[v3].w, p3.w, pt[v1].lightResult, pt[v3].lightResult);

			gouraudTriangleForVerColor(p1, p2, p3);//上三角形
			gouraudTriangleForVerColor(p2, p3, p4);//下三角形


		}
		else//如果p3在上面就利用p1  p3以及线段p1 p2上y值等于p3.y的点组成一个平底三角形
		{
			Point p1 = pt[v1];
			Point p2 = pt[v3];
			Point p3;
			Point p4 = pt[v2];

			//首先求出p3的x值  他在线段pt[v1] pt[v2]上
			p3.y = pt[v3].y;
			p3.x = ((p3.y - pt[v1].y)*((double)(pt[v2].x - pt[v1].x) / (double)(pt[v2].y - pt[v1].y)) + pt[v1].x);

			p3.color = interColor(pt[v1], pt[v2], p3);
			p3.w = interWReci(pt[v1].x, pt[v1].y, pt[v2].x, pt[v2].y, pt[v1].w, pt[v2].w, p3.x, p3.y);
			p3.lightResult = interAttriByW(pt[v1].x, pt[v1].y, pt[v2].x, pt[v2].y, p3.x, p3.y, pt[v1].w, pt[v2].w, p3.w, pt[v1].lightResult, pt[v2].lightResult);

			gouraudTriangleForVerColor(p1, p2, p3);
			gouraudTriangleForVerColor(p3, p2, p4);
		}
	}
}

void RenderPipeline::gouraudTriangleForTexSample(const Point &p1, const Point &p2, const Point &p3) //gouraud法填充三角形
{
	Point pt[3] = { p1,p2,p3 };


	//交换,使pt[v1]点为最上面的顶点(y值最小的点)
	int v1 = 0, v2 = 1, v3 = 2;
	if (pt[v1].y > pt[v2].y)
		SWAP(v1, v2);
	if (pt[v1].y > pt[v3].y)
		SWAP(v1, v3);
	double slope_start, slope_end;//三角形边的斜率
	double w1_reci, w2_reci, w3_reci;
	double s1, t1, s2, t2, s3, t3;
	double l1, l2, l3;

	double slope_start_w_reci, slope_end_w_reci;//扫描线起始值所在的三角形边和结束值所在的三角形边对应的斜率(w值为其倒数的斜率)

	int x, y;
	int xs, xe;
	double dw_reci_start = 0, dw_reci_end = 0;//两根扫描线坐标变化值
	double dx_start = 0, dx_end = 0;

	double ws_reci = 0, we_reci = 0;//扫描线插值起止值
	double ss, se;
	double ts, te;
	double ls, le;

	double dw_reci_x;//扫描线插值步进
	double s, t;
	double l;
	double w_reci;
	double dx, dy;

	Point interPt;//插值得到的点
	if (pt[v1].y == pt[v2].y || pt[v1].y == pt[v3].y)//平顶
	{
		//交换,使pt[v1]点为最左边的顶点
		if (pt[v1].y == pt[v2].y)
		{
			if (pt[v1].x > pt[v2].x)
				SWAP(v1, v2);
		}
		else
		{
			if (pt[v1].x > pt[v3].x)
				SWAP(v1, v3);
		}

		//交换,使平顶三角形的左上为pt[v1],右上为pt[v3],下顶点为pt[v2]
		if (pt[v3].y > pt[v2].y)
			SWAP(v2, v3);

		dy = max(pt[v2].y - pt[v3].y, 0.00000001);//计算出y跨度
		slope_start = (pt[v2].x - pt[v1].x)*1.0 / dy;//左边斜边的斜率
		slope_end = (pt[v2].x - pt[v3].x)*1.0 / dy;//右边斜边的斜率

		w1_reci = pt[v1].w;
		w2_reci = pt[v2].w;
		w3_reci = pt[v3].w;
		s1 = pt[v1].S;
		s2 = pt[v2].S;
		s3 = pt[v3].S;
		t1 = pt[v1].T;
		t2 = pt[v2].T;
		t3 = pt[v3].T;
		l1 = pt[v1].lightResult;
		l2 = pt[v2].lightResult;
		l3 = pt[v3].lightResult;

		slope_start_w_reci = (w2_reci - w1_reci) / dy;
		slope_end_w_reci = (w2_reci - w3_reci) / dy;

		int yStart = ceil(pt[v1].y);
		int yEnd = ceil(pt[v2].y) - 1;
		/*垂直裁剪*/
		if (yStart > windowHeight)return;//起始值超出屏幕下边缘
		if (yStart < 0)yStart = 0;//高于屏幕上边缘，重新赋值，从上边缘开始计算
		if (yEnd < 0)return;//结束值高于屏幕上边缘
		if (yEnd > windowHeight)yEnd = windowHeight;//结束值超出屏幕下边缘，重新赋值，从上边缘结束计算


		dx_start = (yStart - pt[v1].y)*slope_start;
		dx_end = (yStart - pt[v3].y)*slope_end;
		dw_reci_start = (yStart - pt[v1].y)*slope_start_w_reci;
		dw_reci_end = (yStart - pt[v3].y)*slope_end_w_reci;
		for (y = yStart; y <= yEnd; y++)//从上往下一条条扫描
		{
			xs = (pt[v1].x + dx_start + 0.5);//插值开始的X坐标
			xe = (pt[v3].x + dx_end + 0.5);//插值结束的X坐标

			ws_reci = w1_reci + dw_reci_start;
			we_reci = w3_reci + dw_reci_end;
			dx = max(xe - xs, 0.00000000000001);
			dw_reci_x = (we_reci - ws_reci) / dx;

			w_reci = ws_reci;

			//扫描线纹理坐标起始值计算
			ss = interAttriByW(pt[v1].x, pt[v1].y, pt[v2].x, pt[v2].y, xs, y, w1_reci, w2_reci, ws_reci, s1, s2);
			se = interAttriByW(pt[v2].x, pt[v2].y, pt[v3].x, pt[v3].y, xe, y, w2_reci, w3_reci, we_reci, s2, s3);
			ts = interAttriByW(pt[v1].x, pt[v1].y, pt[v2].x, pt[v2].y, xs, y, w1_reci, w2_reci, ws_reci, t1, t2);
			te = interAttriByW(pt[v2].x, pt[v2].y, pt[v3].x, pt[v3].y, xe, y, w2_reci, w3_reci, we_reci, t2, t3);
			ls = interAttriByW(pt[v1].x, pt[v1].y, pt[v2].x, pt[v2].y, xs, y, w1_reci, w2_reci, ws_reci, l1, l2);
			le = interAttriByW(pt[v2].x, pt[v2].y, pt[v3].x, pt[v3].y, xe, y, w2_reci, w3_reci, we_reci, l2, l3);

			for (x = xs; x <= xe; x++)//扫描线
			{
				s = interAttriByW(xs, y, xe, y, x, y, ws_reci, we_reci, w_reci, ss, se);
				t = interAttriByW(xs, y, xe, y, x, y, ws_reci, we_reci, w_reci, ts, te);
				interPt.x = x;
				interPt.y = y;
				interPt.w = w_reci;
				interPt.S = s;
				interPt.T = t;

				if (lightAble)
				{
					l = interAttriByW(xs, y, xe, y, x, y, ws_reci, we_reci, w_reci, ls, le);
					interPt.lightResult = l;
				}
				setPixel(interPt);
				w_reci += dw_reci_x;
			}

			dx_start += slope_start;
			dx_end += slope_end;
			dw_reci_start += slope_start_w_reci;
			dw_reci_end += slope_end_w_reci;

		}
	}
	else if (pt[v2].y == pt[v3].y)//平底
	{
		//交换,使之最上顶点为pt[v1],平底边左端点为pt[v2],右端点为pt[v3]
		if (pt[v2].x > pt[v3].x)
			SWAP(v2, v3);

		dy = max(pt[v2].y - pt[v1].y, 0.00000001);
		slope_start = (pt[v2].x - pt[v1].x)*1.0 / dy;
		slope_end = (pt[v3].x - pt[v1].x)*1.0 / dy;

		w1_reci = pt[v1].w;
		w2_reci = pt[v2].w;
		w3_reci = pt[v3].w;

		s1 = pt[v1].S;
		s2 = pt[v2].S;
		s3 = pt[v3].S;
		t1 = pt[v1].T;
		t2 = pt[v2].T;
		t3 = pt[v3].T;
		l1 = pt[v1].lightResult;
		l2 = pt[v2].lightResult;
		l3 = pt[v3].lightResult;

		slope_start_w_reci = (w2_reci - w1_reci) / dy;
		slope_end_w_reci = (w3_reci - w1_reci) / dy;

		//垂直裁剪
		int yStart = ceil(pt[v1].y);
		int yEnd = ceil(pt[v2].y) - 1;
		if (yStart > windowHeight)return;//起始值超出屏幕下边缘
		if (yStart < 0)yStart = 0;//高于屏幕上边缘，重新赋值，从上边缘开始计算
		if (yEnd < 0)return;//结束值高于屏幕上边缘
		if (yEnd > windowHeight)yEnd = windowHeight;//结束值超出屏幕下边缘，重新赋值，从上边缘结束计算


		dx_start = (yStart - pt[v1].y)*slope_start;
		dx_end = (yStart - pt[v1].y)*slope_end;
		dw_reci_start = (yStart - pt[v1].y)*slope_start_w_reci;
		dw_reci_end = (yStart - pt[v1].y)*slope_end_w_reci;
		for (y = yStart; y <= yEnd; y++)
		{
			xs = (pt[v1].x + dx_start + 0.5);
			xe = (pt[v1].x + dx_end + 0.5);

			ws_reci = w1_reci + dw_reci_start;
			we_reci = w1_reci + dw_reci_end;

			dx = max(xe - xs, 0.00000001);
			dw_reci_x = (we_reci - ws_reci) / dx;

			w_reci = ws_reci;


			//扫描线纹理坐标起始值计算
			ss = interAttriByW(pt[v1].x, pt[v1].y, pt[v2].x, pt[v2].y, xs, y, w1_reci, w2_reci, ws_reci, s1, s2);
			se = interAttriByW(pt[v1].x, pt[v1].y, pt[v3].x, pt[v3].y, xe, y, w1_reci, w3_reci, we_reci, s1, s3);
			ts = interAttriByW(pt[v1].x, pt[v1].y, pt[v2].x, pt[v2].y, xs, y, w1_reci, w2_reci, ws_reci, t1, t2);
			te = interAttriByW(pt[v1].x, pt[v1].y, pt[v3].x, pt[v3].y, xe, y, w1_reci, w3_reci, we_reci, t1, t3);
			ls = interAttriByW(pt[v1].x, pt[v1].y, pt[v2].x, pt[v2].y, xs, y, w1_reci, w2_reci, ws_reci, l1, l2);
			le = interAttriByW(pt[v1].x, pt[v1].y, pt[v3].x, pt[v3].y, xe, y, w1_reci, w3_reci, we_reci, l1, l3);
			for (x = xs; x <= xe; x++)//扫描线
			{
				s = interAttriByW(xs, y, xe, y, x, y, ws_reci, we_reci, w_reci, ss, se);
				t = interAttriByW(xs, y, xe, y, x, y, ws_reci, we_reci, w_reci, ts, te);
				interPt.x = x;
				interPt.y = y;
				interPt.w = w_reci;
				interPt.S = s;
				interPt.T = t;
				if (lightAble)
				{
					l = interAttriByW(xs, y, xe, y, x, y, ws_reci, we_reci, w_reci, ls, le);
					interPt.lightResult = l;
				}
				setPixel(interPt);
				w_reci += dw_reci_x;
			}

			dx_start += slope_start;
			dx_end += slope_end;

			dw_reci_start += slope_start_w_reci;
			dw_reci_end += slope_end_w_reci;
		}
	}
	else  //任意三角形  目前p1已经是最上面的点了，只需要考虑p2和p3的上下关系
	{
		//然后按照pt[v2]  pt[v3]的上下位置关系进行区分
		//如果pt[v2]在上面就利用pt[v1]  pt[v2]  以及线段pt[v1] pt[v3]上y值等于pt[v2].y的点组成一个平底三角形
		if (pt[v2].y < pt[v3].y)
		{
			Point p1 = pt[v1];
			Point p2 = pt[v2];
			Point p3;
			Point p4 = pt[v3];

			//首先求出p3的x值  他在线段pt[v1] pt[v3]上
			p3.y = pt[v2].y;
			p3.x = ((p3.y - pt[v1].y)*((double)(pt[v3].x - pt[v1].x) / (double)(pt[v3].y - pt[v1].y)) + pt[v1].x);

			p3.w = interWReci(pt[v1].x, pt[v1].y, pt[v3].x, pt[v3].y, pt[v1].w, pt[v3].w, p3.x, p3.y);
			p3.S = interAttriByW(pt[v1].x, pt[v1].y, pt[v3].x, pt[v3].y, p3.x, p3.y, pt[v1].w, pt[v3].w, p3.w, pt[v1].S, pt[v3].S);
			p3.T = interAttriByW(pt[v1].x, pt[v1].y, pt[v3].x, pt[v3].y, p3.x, p3.y, pt[v1].w, pt[v3].w, p3.w, pt[v1].T, pt[v3].T);
			p3.lightResult = interAttriByW(pt[v1].x, pt[v1].y, pt[v3].x, pt[v3].y, p3.x, p3.y, pt[v1].w, pt[v3].w, p3.w, pt[v1].lightResult, pt[v3].lightResult);

			gouraudTriangleForTexSample(p1, p2, p3);//上三角形
			gouraudTriangleForTexSample(p2, p3, p4);//下三角形


		}
		else//如果p3在上面就利用p1  p3以及线段p1 p2上y值等于p3.y的点组成一个平底三角形
		{
			Point p1 = pt[v1];
			Point p2 = pt[v3];
			Point p3;
			Point p4 = pt[v2];

			//首先求出p3的x值  他在线段pt[v1] pt[v2]上
			p3.y = pt[v3].y;
			p3.x = ((p3.y - pt[v1].y)*((double)(pt[v2].x - pt[v1].x) / (double)(pt[v2].y - pt[v1].y)) + pt[v1].x);

			p3.w = interWReci(pt[v1].x, pt[v1].y, pt[v2].x, pt[v2].y, pt[v1].w, pt[v2].w, p3.x, p3.y);
			p3.S = interAttriByW(pt[v1].x, pt[v1].y, pt[v2].x, pt[v2].y, p3.x, p3.y, pt[v1].w, pt[v2].w, p3.w, pt[v1].S, pt[v2].S);
			p3.T = interAttriByW(pt[v1].x, pt[v1].y, pt[v2].x, pt[v2].y, p3.x, p3.y, pt[v1].w, pt[v2].w, p3.w, pt[v1].T, pt[v2].T);
			p3.lightResult = interAttriByW(pt[v1].x, pt[v1].y, pt[v2].x, pt[v2].y, p3.x, p3.y, pt[v1].w, pt[v2].w, p3.w, pt[v1].lightResult, pt[v2].lightResult);

			gouraudTriangleForTexSample(p1, p2, p3);
			gouraudTriangleForTexSample(p3, p2, p4);
		}
	}
}

void RenderPipeline::drawLine(const Point &p0, const Point &p1)
{
	int x0 = p0.x;
	int y0 = p0.y;
	int x1 = p1.x;
	int y1 = p1.y;
	double w0 = p0.w;
	double w1 = p1.w;
	double s0 = p0.S;
	double s1 = p1.S;
	double t0 = p0.T;
	double t1 = p0.T;
	double l0 = p0.lightResult;
	double l1 = p0.lightResult;

	double delta_x, delta_y, x, y;
	double w = 0;
	int dx, dy, steps;
	dx = x1 - x0;
	dy = y1 - y0;
	if (abs(dx) > abs(dy)) {
		steps = abs(dx);
	}
	else {
		steps = abs(dy);
	}
	delta_x = (float)dx / (float)steps;
	delta_y = (float)dy / (float)steps;
	x = (float)x0;
	y = (float)y0;
	Point interPt;
	Point tempPt;
	switch (rasterizationStateInfo.colorMode)
	{
	case Ver_Color:
		for (int i = 1; i <= steps; i++)
		{
			tempPt.x = x;
			tempPt.y = y;

			w = interWReci(x0, y0, x1, y1, w0, w1, x, y);
			interPt.x = x;
			interPt.y = y;
			interPt.w = w;
			interPt.color = interColor(p0, p1, tempPt);
			interPt.lightResult = interAttriByW(x0, y0, x1, y1, x, y, w0, w1, w, l0, l1);

			setPixel(interPt);
			x += delta_x;
			y += delta_y;
		}
		break;
	case Tex_Sample:
		for (int i = 1; i <= steps; i++)
		{
			w = interWReci(x0, y0, x1, y1, w0, w1, x, y);
			interPt.x = x;
			interPt.y = y;
			interPt.w = w;
			interPt.S = interAttriByW(x0, y0, x1, y1, x, y, w0, w1, w, s0, s1);
			interPt.T = interAttriByW(x0, y0, x1, y1, x, y, w0, w1, w, t0, t1);
			interPt.lightResult = interAttriByW(x0, y0, x1, y1, x, y, w0, w1, w, l0, l1);

			setPixel(interPt);
			x += delta_x;
			y += delta_y;
		}
		break;
	default:
		break;
	}
}

double RenderPipeline::interAttriByW(double x0, double y0, double x1, double y1, double x, double y, double w0, double w1, double w, double attri0, double attri1) {
	double t = distance(x0, y0, x, y) / distance(x1, y1, x0, y0);
	double result;
	if (perCorr){
		result = (attri0 * w0 + t*(attri1 * w1 - attri0 * w0)) / w;
	}
	else {
		result = attri0 + (attri1 - attri0)*t;
	}
	return result;
}

void RenderPipeline::pushUniform() {//mvp矩阵
}

void RenderPipeline::pushConstant() {//当前变换矩阵  和mvp矩阵

	memcpy(MVMatrix->matData, MatrixState::getMVatrix()->matData, sizeof(double) * 16);
	memcpy(MVPMatrix->matData, MatrixState::getMVPatrix()->matData, sizeof(double) * 16);
	memcpy(mMatrix->matData, MatrixState::getMMatrix()->matData, sizeof(double) * 16);
	memcpy(MPMatrix->matData, MatrixState::getPMatrix()->matData, sizeof(double) * 16);
}

void RenderPipeline::bindSampler(const vector<TexDataObject *>&tdoList) {//绑定采样器
	samplerData.clear();
	samplerData.insert(samplerData.begin(), tdoList.begin(), tdoList.end());
}

void RenderPipeline::RenderListClip(vector<Triangle*>&renderlist, ClipPlaneFlag clip_flags) // 基于视景体的物体裁剪
{
#define CLIP_CODE_GZ   0x0001    
#define CLIP_CODE_LZ   0x0002    
#define CLIP_CODE_IZ   0x0004    

#define CLIP_CODE_GX   0x0001    
#define CLIP_CODE_LX   0x0002    
#define CLIP_CODE_IX   0x0004    

#define CLIP_CODE_GY   0x0001   
#define CLIP_CODE_LY   0x0002    
#define CLIP_CODE_IY   0x0004    
#define CLIP_CODE_NULL 0x0000

	int vertex_ccodes[3]; // 三个顶点裁剪编码
	int num_verts_in;
	int v0, v1, v2;

	double z_factor, z_test;

	double xi, yi, x01i, y01i, x02i, y02i, t1, t2, ui, vi, u01i, v01i, u02i, v02i, nx01i, ny01i, nz01i, nx02i, ny02i, nz02i;


	double  r0, g0, b0, r1, g1, b1, r2, g2, b2, clip_red, clip_green, clip_blue;

	double nx0, nx1, nx2, ny0, ny1, ny2, nz0, nz1, nz2;

	Vector3 v=Vector3();

	Triangle* temp_tri;
	size_t size = renderlist.size();
	for (int poly = 0; poly < size; poly++)
	{
		Triangle* curr_tri = renderlist[poly];

		//根据左右裁剪面判断
		if (clip_flags & CLIP_POLY_X_PLANE)
		{
			z_factor =( currCamera->f/ currCamera->n)*currCamera->r / currCamera->f;

			z_test = z_factor * curr_tri->verList[0]->z;

			if (curr_tri->verList[0]->x > z_test)
			{
				vertex_ccodes[0] = CLIP_CODE_GX;
			}
			else if (curr_tri->verList[0]->x < -z_test)
			{
				vertex_ccodes[0] = CLIP_CODE_LX;
			}
			else
			{
				vertex_ccodes[0] = CLIP_CODE_IX;
			}


			z_test = z_factor * curr_tri->verList[1]->z;

			if (curr_tri->verList[1]->x > z_test)
			{
				vertex_ccodes[1] = CLIP_CODE_GX;
			}
			else if (curr_tri->verList[1]->x < -z_test)
			{
				vertex_ccodes[1] = CLIP_CODE_LX;
			}
			else
			{
				vertex_ccodes[1] = CLIP_CODE_IX;
			}

			z_test = z_factor * curr_tri->verList[2]->z;

			if (curr_tri->verList[2]->x > z_test)
			{
				vertex_ccodes[2] = CLIP_CODE_GX;
			}
			else if (curr_tri->verList[2]->x < -z_test)
			{
				vertex_ccodes[2] = CLIP_CODE_LX;
			}
			else
			{
				vertex_ccodes[1] = CLIP_CODE_IX;
			}
			// 三个顶点都在左右裁剪面以外
			if (((vertex_ccodes[0] == CLIP_CODE_GX) && (vertex_ccodes[1] == CLIP_CODE_GX) && (vertex_ccodes[2] == CLIP_CODE_GX)) || ((vertex_ccodes[0] == CLIP_CODE_LX) && (vertex_ccodes[1] == CLIP_CODE_LX) && (vertex_ccodes[2] == CLIP_CODE_LX)))
			{
				curr_tri->clipped = true;
				continue;
			}

		}
		//根据上下裁剪面判断
		if (clip_flags & CLIP_POLY_Y_PLANE)
		{
			z_factor = (currCamera->f / currCamera->n)*currCamera->t / currCamera->f;

			z_test = z_factor * curr_tri->verList[0]->z;

			if (curr_tri->verList[0]->y > z_test)
			{
				vertex_ccodes[0] = CLIP_CODE_GY;
			}
			else if (curr_tri->verList[0]->y < -z_test)
			{
				vertex_ccodes[0] = CLIP_CODE_LY;
			}
			else
			{
				vertex_ccodes[0] = CLIP_CODE_IY;
			}
			z_test = z_factor * curr_tri->verList[1]->z;

			if (curr_tri->verList[1]->y > z_test)
			{
				vertex_ccodes[1] = CLIP_CODE_GY;
			}
			else if (curr_tri->verList[1]->y < -z_test)
			{
				vertex_ccodes[1] = CLIP_CODE_LY;
			}
			else
			{
				vertex_ccodes[1] = CLIP_CODE_IY;
			}

			z_test = z_factor * curr_tri->verList[2]->z;

			if (curr_tri->verList[2]->y > z_test)
			{
				vertex_ccodes[2] = CLIP_CODE_GY;
			}
			else if (curr_tri->verList[2]->y < -z_test)
			{
				vertex_ccodes[2] = CLIP_CODE_LY;
			}
			else
			{
				vertex_ccodes[2] = CLIP_CODE_IY;
			}

			// 三个顶点都在上下裁剪面以外
			if (((vertex_ccodes[0] == CLIP_CODE_GY) && (vertex_ccodes[1] == CLIP_CODE_GY) && (vertex_ccodes[2] == CLIP_CODE_GY)) || ((vertex_ccodes[0] == CLIP_CODE_LY) && (vertex_ccodes[1] == CLIP_CODE_LY) && (vertex_ccodes[2] == CLIP_CODE_LY)))
			{
				curr_tri->clipped = true;
				continue;
			}

		}
		// 根据近远裁剪面裁剪
		if (clip_flags & CLIP_POLY_Z_PLANE)
		{
			num_verts_in = 0; //统计在近裁剪面以内的顶点数,根据顶点数判断裁剪类型

			if (curr_tri->verList[0]->z > currCamera->f)
			{
				vertex_ccodes[0] = CLIP_CODE_GZ;
			}
			else if (curr_tri->verList[0]->z < currCamera->n)
			{
				vertex_ccodes[0] = CLIP_CODE_LZ;
			}
			else
			{
				vertex_ccodes[0] = CLIP_CODE_IZ;
				num_verts_in++;
			}
			if (curr_tri->verList[1]->z > currCamera->f)
			{
				vertex_ccodes[1] = CLIP_CODE_GZ;
			}
			else if (curr_tri->verList[1]->z < currCamera->n)
			{
				vertex_ccodes[1] = CLIP_CODE_LZ;
			}
			else
			{
				vertex_ccodes[1] = CLIP_CODE_IZ;
				num_verts_in++;
			}

			if (curr_tri->verList[2]->z > currCamera->f)
			{
				vertex_ccodes[2] = CLIP_CODE_GZ;
			}
			else if (curr_tri->verList[2]->z < currCamera->n)
			{
				vertex_ccodes[2] = CLIP_CODE_LZ;
			}
			else
			{
				vertex_ccodes[2] = CLIP_CODE_IZ;
				num_verts_in++;
			}
			// 三个顶点都位于近远裁剪面以外
			if (((vertex_ccodes[0] == CLIP_CODE_GZ) && (vertex_ccodes[1] == CLIP_CODE_GZ) && (vertex_ccodes[2] == CLIP_CODE_GZ)) || ((vertex_ccodes[0] == CLIP_CODE_LZ) && (vertex_ccodes[1] == CLIP_CODE_LZ) && (vertex_ccodes[2] == CLIP_CODE_LZ)))
			{
				curr_tri->clipped = true;
				continue;
			}

			// 至少有一个顶点位于近裁剪面以外
			if (((vertex_ccodes[0] | vertex_ccodes[1] | vertex_ccodes[2]) & CLIP_CODE_LZ))
			{
				// 只有一个顶点在近裁剪面以内,不需要增加新的多边形
				if (num_verts_in == 1)
				{
					if (vertex_ccodes[0] == CLIP_CODE_IZ)
					{
						v0 = 0;
						v1 = 1;
						v2 = 2;
					}
					else if (vertex_ccodes[1] == CLIP_CODE_IZ)
					{
						v0 = 1;
						v1 = 2;
						v2 = 0;
					}
					else
					{
						v0 = 2;
						v1 = 0;
						v2 = 1;
					}

					//当前三角形的v0不变,计算新的v1，v2
					// 右裁剪点坐标
					VertexCreate(curr_tri->verList[v0] ,curr_tri->verList[v1], v);
					t1 = ((currCamera->n - curr_tri->verList[v0]->z) / v.z);
					xi = curr_tri->verList[v0]->x + v.x * t1;
					yi = curr_tri->verList[v0]->y + v.y * t1;
					curr_tri->verList[v1]->x = xi;
					curr_tri->verList[v1]->y = yi;
					curr_tri->verList[v1]->z = currCamera->n;
					// 右裁剪点的纹理坐标
					ui = curr_tri->verList[v0]->s + (curr_tri->verList[v1]->s - curr_tri->verList[v0]->s) * t1;
					vi = curr_tri->verList[v0]->t + (curr_tri->verList[v1]->t - curr_tri->verList[v0]->t) * t1;
					curr_tri->verList[v1]->s = ui;
					curr_tri->verList[v1]->t = vi;
					GetRGBValue(curr_tri->verList[v0]->color, r0, g0, b0);
					GetRGBValue(curr_tri->verList[v1]->color, r1, g1, b1);
					GetRGBValue(curr_tri->verList[v2]->color, r2, g2, b2);
					// 右裁剪点的颜色值
					clip_red = r0 + (r1 - r0) * t1;
					clip_green = g0 + (g1 - g0) * t1;
					clip_blue = b0 + (b1 - b0) * t1;
					curr_tri->verList[v1]->color = Color(clip_red, clip_green, clip_blue);
					//右裁剪点的法向量
					nx0 = curr_tri->verList[v0]->nx;
					ny0 = curr_tri->verList[v0]->ny;
					nz0 = curr_tri->verList[v0]->nz;

					nx1 = curr_tri->verList[v1]->nx;
					ny1 = curr_tri->verList[v1]->ny;
					nz1 = curr_tri->verList[v1]->nz;

					nx2 = curr_tri->verList[v2]->nx;
					ny2 = curr_tri->verList[v2]->ny;
					nz2 = curr_tri->verList[v2]->nz;

					curr_tri->verList[v1]->nx = nx0 + (nx1 - nx0) * t1;
					curr_tri->verList[v1]->ny = ny0 + (ny1 - ny0) * t1;
					curr_tri->verList[v1]->nz = nz0 + (nz1 - nz0) * t1;


					// 左裁剪点的坐标
					VertexCreate(curr_tri->verList[v0], curr_tri->verList[v2], v);

					t2 = ((currCamera->n - curr_tri->verList[v0]->z) / v.z);
					xi = curr_tri->verList[v0]->x + v.x * t2;
					yi = curr_tri->verList[v0]->y + v.y * t2;
					curr_tri->verList[v2]->x = xi;
					curr_tri->verList[v2]->y = yi;
					curr_tri->verList[v2]->z = currCamera->n;
					// 左裁剪点的纹理坐标
					ui = curr_tri->verList[v0]->s + (curr_tri->verList[v2]->s - curr_tri->verList[v0]->s) * t2;
					vi = curr_tri->verList[v0]->t + (curr_tri->verList[v2]->t - curr_tri->verList[v0]->t) * t2;
					curr_tri->verList[v2]->s = ui;
					curr_tri->verList[v2]->t= vi;
					// 左裁剪点的颜色值
					clip_red = r0 + (r2 - r0) * t2;
					clip_green = g0 + (g2 - g0) * t2;
					clip_blue = b0 + (b2 - b0) * t2;
					curr_tri->verList[v2]->color = Color(clip_red, clip_green, clip_blue);
					//左裁剪点的法向量
					curr_tri->verList[v2]->nx = nx0 + (nx2 - nx0) * t2;
					curr_tri->verList[v2]->ny = ny0 + (ny2 - ny0) * t2;
					curr_tri->verList[v2]->nz = nz0 + (nz2 - nz0) * t2;

				}
				// 有二个顶点在近裁剪面以内,需要增加新的多边形
				else if (num_verts_in == 2)
				{

					Vertex *tv1 = curr_tri->verList[0];
					Vertex *tv2 = curr_tri->verList[1];
					Vertex *tv3 = curr_tri->verList[2];
					Vertex* _v1 = new Vertex(tv1->x, tv1->y, tv1->z, tv1->nx, tv1->ny, tv1->nz, tv1->s, tv1->t, tv1->color);
					Vertex* _v2 = new Vertex(tv2->x, tv2->y, tv2->z, tv2->nx, tv2->ny, tv2->nz, tv2->s, tv2->t, tv2->color);
					Vertex* _v3 = new Vertex(tv3->x, tv3->y, tv3->z, tv3->nx, tv3->ny, tv3->nz, tv3->s, tv3->t, tv3->color);
					temp_tri = new Triangle (_v1,_v2,_v3);

					if (vertex_ccodes[0] == CLIP_CODE_LZ)
					{
						v0 = 0;
						v1 = 1;
						v2 = 2;
					}
					else if (vertex_ccodes[1] == CLIP_CODE_LZ)
					{
						v0 = 1;
						v1 = 2;
						v2 = 0;
					}
					else
					{
						v0 = 2;
						v1 = 0;
						v2 = 1;
					}
					// 左裁剪点坐标
					VertexCreate(curr_tri->verList[v0], curr_tri->verList[v1], v);
					t1 = ((currCamera->n - curr_tri->verList[v0]->z) / v.z);
					x01i = curr_tri->verList[v0]->x + v.x * t1;
					y01i = curr_tri->verList[v0]->y + v.y * t1;
					// 右裁剪点坐标
					VertexCreate(curr_tri->verList[v0], curr_tri->verList[v2], v);


					t2 = ((currCamera->n - curr_tri->verList[v0]->z) / v.z);
					x02i = curr_tri->verList[v0]->x + v.x * t2;
					y02i = curr_tri->verList[v0]->y + v.y * t2;

					GetRGBValue(curr_tri->verList[v0]->color,r0, g0, b0);
					GetRGBValue(curr_tri->verList[v1]->color,r1, g1, b1);
					GetRGBValue(curr_tri->verList[v2]->color,r2, g2, b2);
					// 裁剪点的颜色值
					clip_red = r0 + (r1 - r0) * t1;
					clip_green = g0 + (g1 - g0) * t1;
					clip_blue = b0 + (b1 - b0) * t1;
					curr_tri->verList[v0]->color = Color((unsigned char) clip_red, (unsigned char)clip_green, (unsigned char)clip_blue);
					temp_tri->verList[v1]->color = Color((unsigned char)clip_red, (unsigned char)clip_green, (unsigned char)clip_blue);
					// 裁剪点的颜色值
					clip_red = r0 + (r2 - r0) * t2;
					clip_green = g0 + (g2 - g0) * t2;
					clip_blue = b0 + (b2 - b0) * t2;
					temp_tri->verList[v0]->color = Color((unsigned char)clip_red, (unsigned char)clip_green, (unsigned char)clip_blue);
														  

					// 裁剪点的顶点坐标
					curr_tri->verList[v0]->x = x01i;
					curr_tri->verList[v0]->y = y01i;
					curr_tri->verList[v0]->z = currCamera->n;

					// 新多边形顶点坐标
					temp_tri->verList[v1]->x = x01i;
					temp_tri->verList[v1]->y = y01i;
					temp_tri->verList[v1]->z = currCamera->n;

					temp_tri->verList[v0]->x = x02i;
					temp_tri->verList[v0]->y = y02i;
					temp_tri->verList[v0]->z = currCamera->n;

					// 左裁剪点纹理坐标
					u01i = curr_tri->verList[v0]->s + (curr_tri->verList[v1]->s - curr_tri->verList[v0]->s) * t1;
					v01i = curr_tri->verList[v0]->t + (curr_tri->verList[v1]->t - curr_tri->verList[v0]->t) * t1;
					// 右裁剪点纹理坐标
					u02i = curr_tri->verList[v0]->s + (curr_tri->verList[v2]->s - curr_tri->verList[v0]->s) * t2;
					v02i = curr_tri->verList[v0]->t + (curr_tri->verList[v2]->t - curr_tri->verList[v0]->t) * t2;

					// 左裁剪点法向量
					nx01i = curr_tri->verList[v0]->nx + (curr_tri->verList[v1]->nx - curr_tri->verList[v0]->nx) * t1;
					ny01i = curr_tri->verList[v0]->ny + (curr_tri->verList[v1]->ny - curr_tri->verList[v0]->ny) * t1;
					nz01i = curr_tri->verList[v0]->nz + (curr_tri->verList[v1]->nz - curr_tri->verList[v0]->nz) * t1;
					curr_tri->verList[v0]->nx = nx01i;
					curr_tri->verList[v0]->ny = ny01i;
					curr_tri->verList[v0]->nz = nz01i;
					// 右裁剪点纹理坐标
					nx02i = curr_tri->verList[v0]->nx + (curr_tri->verList[v2]->nx - curr_tri->verList[v0]->nx) * t2;
					ny02i = curr_tri->verList[v0]->ny + (curr_tri->verList[v2]->ny - curr_tri->verList[v0]->ny) * t2;
					nz02i = curr_tri->verList[v0]->nz + (curr_tri->verList[v2]->nz - curr_tri->verList[v0]->nz) * t2;
					temp_tri->verList[v1]->nx = nx02i;
					temp_tri->verList[v1]->ny = ny02i;
					temp_tri->verList[v1]->nz = nz02i;

					curr_tri->verList[v0]->s = u01i;
					curr_tri->verList[v0]->t = v01i;
					// 新多边形纹理坐标和法向量
					temp_tri->verList[v0]->s = u02i;
					temp_tri->verList[v0]->t = v02i;
					temp_tri->verList[v0]->nx = nx02i;
					temp_tri->verList[v0]->ny = ny02i;
					temp_tri->verList[v0]->nz = nz02i;
					temp_tri->verList[v1]->s = u01i;
					temp_tri->verList[v1]->t = v01i;
					temp_tri->verList[v1]->nx = nx01i;
					temp_tri->verList[v1]->ny = ny01i;
					temp_tri->verList[v1]->nz = nz01i;

					renderlist.push_back(temp_tri);//新增三角形

				}
			}
		}
	}
}


RenderPipeline::~RenderPipeline()
{
	delete mMatrix;
	delete MVPMatrix;
	delete MVMatrix;
	delete MPMatrix;

}