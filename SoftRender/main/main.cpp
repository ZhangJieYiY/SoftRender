// Win32Project2.cpp : 定义控制台应用程序的入口点。
//

#include <windows.h>
#include "RenderPipeline.h"
#include "../BaseDataClass/Camera.h"
#include "../BaseDataClass/Vector3.h"
#include "../BaseDataClass/Vector4.h"
#include "../BaseDataClass/Matrix.h"
#include "../BaseDataClass/Object.h"
#include "../BaseDataClass/Vertex.h"
#include "../BaseDataClass/Triangle.h"
#include "../util/FPSUtil.h"
#include "../util/FileUtil.h"
#include "../util/LoadObj.h"
#include "../util/MatrixState.h"
#include "../util/UserData.h"

#define MAX_LOADSTRING 100
// 全局变量: 
HINSTANCE hInst;                                // 当前实例
char szTitle[MAX_LOADSTRING] = "渲染管线模拟";	// 标题栏文本
char szWindowClass[MAX_LOADSTRING] = "渲染管线模拟";// 主窗口类名
int windowX = 100;								// 窗体位置X坐标
int windowY = 100;								// 窗体位置Y坐标
HWND hwnd;
int preX = 0;
int preY = 0;
int disX = 0;
int disY = 0;
bool mouseLeftDown = false;//鼠标是否按下
bool ifDrawTeapot = true;//是否绘制茶壶
bool rotateFlag = false;//是否旋转的标志位
float sensitivity = 0.3f;//鼠标拖动灵敏度  影响pitch yaw
Camera *mainCamera = new Camera();//场景主摄像机

RenderPipeline *myRenderpipeline = new RenderPipeline();

void createConsole();//创建控制台
void repaint();//重绘窗口
void initWindow(HINSTANCE hInstance);//初始化窗口
void shutdown();//应用程序关闭，释放资源
LRESULT CALLBACK  WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);//事件处理回调方法

float angle = 0;
void prepare();//执行准备工作的方法

Object *cube;
Object *teapot;
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	hInst = hInstance;
	//createConsole();
	initWindow(hInstance);
	RECT *wr = new RECT();
	wr->left = 0;
	wr->top = 0;
	wr->right = windowWidth;
	wr->bottom = windowHeight;
	FPSUtil::init();
	MSG msg;
	// 主消息循环: 
	while (true)
	{
		FPSUtil::calFPS();
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);

		}
		else
		{
			repaint();
			InvalidateRect(hwnd, wr, false);
			UpdateWindow(hwnd);
		}
	}
	shutdown();
	return (int)msg.wParam;
}


LRESULT CALLBACK  WndProc(HWND _hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg)
	{
	case WM_COMMAND:
		break;
	case WM_CREATE:
		prepare(); //准备工作  设置渲染管线的一些状态值以及创建渲染管线
	case WM_PAINT:
		repaint();
		break;
	case WM_DESTROY://窗口销毁
		PostQuitMessage(0);
		break;
	case WM_KEYDOWN:
		int t;
		switch (wParam) {
		case 'W'://W    绘制模式切换
			t = myRenderpipeline->rasterizationStateInfo.drawMode;
			t += 1;
			t %= 3;
			myRenderpipeline->rasterizationStateInfo.drawMode=(DRAW_MODE)t ;
			break;
		case 'B'://B    背面剪裁切换
			t = myRenderpipeline->rasterizationStateInfo.cullMode;
			t += 1;
			t %= 3;
			myRenderpipeline->rasterizationStateInfo.cullMode = (CULL_MODE)t;
			break;
		case 'C'://C    颜色模式切换
			t = myRenderpipeline->rasterizationStateInfo.colorMode;
			t += 1;
			t %= 2;
			myRenderpipeline->rasterizationStateInfo.colorMode = (GET_COLOR_MODE)t;
			break;
		case 'L'://L    光照是否开启
			myRenderpipeline->lightAble = !myRenderpipeline->lightAble;
			break;
		case 'R'://R    旋转是否开启
			rotateFlag = !rotateFlag;
			break;
		case 'D'://D    深度测试是否开启
			myRenderpipeline->depthTestAble = !myRenderpipeline->depthTestAble;
			break;
		case 'E'://E    旋转是否开启
			myRenderpipeline->noLight = !myRenderpipeline->noLight;
			break;
		case 'Z'://Z     透视矫正
			myRenderpipeline->perCorr = !myRenderpipeline->perCorr;
			break;
		case 'F'://F    茶壶是否绘制
			ifDrawTeapot=!ifDrawTeapot;
			break;
		case VK_UP://forward		//相机四个方向移动
			mainCamera->calCameraPosition(1,0);
			break;
		case VK_DOWN://-forward    
			mainCamera->calCameraPosition(-1,0);
			break;
		case VK_RIGHT://right  
			mainCamera->calCameraPosition(0,1);
			break;
		case VK_LEFT://-right
			mainCamera->calCameraPosition(0,-1);
			break;
		}
		break;
	case WM_LBUTTONDOWN:
		preX = LOWORD(lParam);
		preY = HIWORD(lParam);
		mouseLeftDown = true;
		break;
	case WM_LBUTTONUP:
		mouseLeftDown = false;
		break;
	case WM_MOUSEMOVE:
		if (mouseLeftDown) {
			disX = LOWORD(lParam) - preX;
			disY = HIWORD(lParam) - preY;
			preX = LOWORD(lParam);
			preY = HIWORD(lParam);
			mainCamera->calCameraDirction(disX * sensitivity, disY * sensitivity);
		}
	default:
		return DefWindowProc(_hwnd, uMsg, wParam, lParam);
	}
	return 0;
}

void createConsole() {
	AllocConsole();
	FILE* stream;
	freopen_s(&stream, "CON", "r", stdin);//重定向输入流
	freopen_s(&stream, "CON", "w", stdout);//重定向输出流
	SetConsoleTitle("控制台窗口");
}
void initWindow(HINSTANCE hInstance) {
	//设计窗口类——————初始化窗体结构
	WNDCLASSEX win_class;
	win_class.cbSize = sizeof(WNDCLASSEX);

	win_class.style = CS_HREDRAW | CS_VREDRAW;//窗口长宽变化需要重新绘制
	win_class.lpfnWndProc = WndProc;//窗口事件回调函数
	win_class.cbClsExtra = 0;//为窗口类结构之外分配的额外的字节数，初始化为0
	win_class.cbWndExtra = 0;//记录窗口实例分配的额外字节数，初始化为0
	win_class.hInstance = hInstance;//包含了该类实例的句柄，该实例包含了窗口过程
	win_class.hIcon = LoadIcon(NULL, IDI_APPLICATION);//类图标的句柄
	win_class.hCursor = LoadCursor(NULL, IDC_ARROW);//鼠标指针的句柄
	win_class.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1); //背景画刷句柄
	win_class.lpszMenuName = NULL;//描述菜单的资源名
	win_class.lpszClassName = szWindowClass;//原型或字符串
	win_class.hIconSm = LoadIcon(NULL, IDI_WINLOGO);//任务栏小图标的句柄

													// 注册窗口类
	if (!RegisterClassEx(&win_class))
	{
		// 注册失败，打印错误信息
		printf("Unexpected error trying to start the application!\n");
		fflush(stdout);
		exit(1);
	}

	//创建窗口
	RECT wr = { windowX, windowY, windowX+windowWidth, windowY+windowHeight };
	//调整窗体矩形
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);
	hwnd = CreateWindowEx(
		0,						//窗口的扩展风格
		szWindowClass,			//指向注册类名的指针
		szTitle,				//指向窗口名称的指针
		WS_OVERLAPPEDWINDOW |	//窗口的风格
		WS_VISIBLE | WS_SYSMENU,
		wr.left, wr.top,		//窗体左上角坐标
		wr.right - wr.left,		//窗体宽度
		wr.bottom - wr.top,		//窗体高度
		NULL,					//父窗口的句柄
		NULL,					//菜单的句柄或是子窗口的标识符
		hInstance,				//应用程序实例的句柄 
		nullptr);				//指向窗口的创建数据
	if (!hwnd)
	{
		// 生成失败，打印错误信息
		printf("Cannot create a window in which to draw!\n");
		fflush(stdout);
		exit(1);
	}

	//设置窗口的用户数据为窗体创建的info数据，已被需要时可以从窗体获取
	ShowWindow(hwnd, SW_SHOWNA);
	UpdateWindow(hwnd);
}
void prepare() {

	MatrixState::setInitStack();
	mainCamera->calCameraPosition(0,0);
	float ratio = (float)windowWidth / (float)windowHeight;
	float scalar = 1.0f;
	mainCamera->setFrustum(-ratio * scalar, ratio*scalar, 1.0*scalar, -1.0*scalar, 1.0f, 200);


	RasterizationStateInfo rsi{
		MY_FILL,
		Tex_Sample,
		CCW,
		BACK
	};
	myRenderpipeline->createRenderPipline(rsi);


	////up
	//Vertex *up_v1 = new Vertex(-1, 0, 1, -1, 0, 1, 0, 0, Color(255, 0, 0));
	//Vertex *up_v2 = new Vertex(-1, 0, -1, -1, 0, -1, 0, 1, Color(255, 255, 0));
	//Vertex *up_v3 = new Vertex(1, 0, -1, 1, 0, -1, 1, 1, Color(0, 0, 255));
	//Vertex *up_v4 = new Vertex(1, 0, 1, 1, 0, 1, 1, 0, Color(0, 255, 0));
	//Triangle *triu1 = new Triangle(up_v1, up_v3, up_v2);
	//Triangle *triu2 = new Triangle(up_v1, up_v4, up_v3);

	//cube = new Object();
	//cube->addTriangle(triu1);
	//cube->addTriangle(triu2);
	//cube->tdoList.push_back( FileUtil::loadCommonTexData("../Win32Project2/Resource/floor.bntex"));
	//cube->rp = myRenderpipeline;

	teapot = LoadObj::loadFromFile("../Win32Project2/Resource/ch_t.obj"); 
	teapot->tdoList.push_back( FileUtil::loadCommonTexData("../Win32Project2/Resource/ghxp.bntex"));
	teapot->rp = myRenderpipeline;
}
void repaint() {

	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hwnd, &ps);

	//指定主摄像机
	myRenderpipeline->currCamera = mainCamera;
	myRenderpipeline->clearColorBuffer();//清空缓存
	myRenderpipeline->clearDepthBuffer();
	myRenderpipeline->pushUniform();//
	if (rotateFlag) {
		angle += 0.5f;
		if (angle > 360)
		{
			angle -= 360;
		}
	}
	memcpy(MatrixState::mVMatrix, mainCamera->vm, sizeof(double) * 16);
	memcpy(MatrixState::mPMatrix, mainCamera->pm, sizeof(double) * 16);

	MatrixState::pushMatrix();
	MatrixState::rotate(0, angle, 0);
	MatrixState::translate(0, 0, 0);
	if (ifDrawTeapot)
	{
		MatrixState::pushMatrix();
		MatrixState::scale(0.6f, 0.6f, 0.6f);
		teapot->dawSelf();
		MatrixState::popMatrix();
	}
	MatrixState::popMatrix();

	myRenderpipeline->present(hdc);
	EndPaint(hwnd, &ps);
}
void shutdown() {
	delete myRenderpipeline;
	delete mainCamera;
	delete MatrixState::mMMatrix;
	delete MatrixState::mVMatrix;
	delete MatrixState::mPMatrix;
	delete MatrixState::mMVPMatrix;
	delete MatrixState::mMVMatrix;
}