// Win32Project2.cpp : �������̨Ӧ�ó������ڵ㡣
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
// ȫ�ֱ���: 
HINSTANCE hInst;                                // ��ǰʵ��
char szTitle[MAX_LOADSTRING] = "��Ⱦ����ģ��";	// �������ı�
char szWindowClass[MAX_LOADSTRING] = "��Ⱦ����ģ��";// ����������
int windowX = 100;								// ����λ��X����
int windowY = 100;								// ����λ��Y����
HWND hwnd;
int preX = 0;
int preY = 0;
int disX = 0;
int disY = 0;
bool mouseLeftDown = false;//����Ƿ���
bool ifDrawTeapot = true;//�Ƿ���Ʋ��
bool rotateFlag = false;//�Ƿ���ת�ı�־λ
float sensitivity = 0.3f;//����϶�������  Ӱ��pitch yaw
Camera *mainCamera = new Camera();//�����������

RenderPipeline *myRenderpipeline = new RenderPipeline();

void createConsole();//��������̨
void repaint();//�ػ洰��
void initWindow(HINSTANCE hInstance);//��ʼ������
void shutdown();//Ӧ�ó���رգ��ͷ���Դ
LRESULT CALLBACK  WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);//�¼�����ص�����

float angle = 0;
void prepare();//ִ��׼�������ķ���

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
	// ����Ϣѭ��: 
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
		prepare(); //׼������  ������Ⱦ���ߵ�һЩ״ֵ̬�Լ�������Ⱦ����
	case WM_PAINT:
		repaint();
		break;
	case WM_DESTROY://��������
		PostQuitMessage(0);
		break;
	case WM_KEYDOWN:
		int t;
		switch (wParam) {
		case 'W'://W    ����ģʽ�л�
			t = myRenderpipeline->rasterizationStateInfo.drawMode;
			t += 1;
			t %= 3;
			myRenderpipeline->rasterizationStateInfo.drawMode=(DRAW_MODE)t ;
			break;
		case 'B'://B    ��������л�
			t = myRenderpipeline->rasterizationStateInfo.cullMode;
			t += 1;
			t %= 3;
			myRenderpipeline->rasterizationStateInfo.cullMode = (CULL_MODE)t;
			break;
		case 'C'://C    ��ɫģʽ�л�
			t = myRenderpipeline->rasterizationStateInfo.colorMode;
			t += 1;
			t %= 2;
			myRenderpipeline->rasterizationStateInfo.colorMode = (GET_COLOR_MODE)t;
			break;
		case 'L'://L    �����Ƿ���
			myRenderpipeline->lightAble = !myRenderpipeline->lightAble;
			break;
		case 'R'://R    ��ת�Ƿ���
			rotateFlag = !rotateFlag;
			break;
		case 'D'://D    ��Ȳ����Ƿ���
			myRenderpipeline->depthTestAble = !myRenderpipeline->depthTestAble;
			break;
		case 'E'://E    ��ת�Ƿ���
			myRenderpipeline->noLight = !myRenderpipeline->noLight;
			break;
		case 'Z'://Z     ͸�ӽ���
			myRenderpipeline->perCorr = !myRenderpipeline->perCorr;
			break;
		case 'F'://F    ����Ƿ����
			ifDrawTeapot=!ifDrawTeapot;
			break;
		case VK_UP://forward		//����ĸ������ƶ�
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
	freopen_s(&stream, "CON", "r", stdin);//�ض���������
	freopen_s(&stream, "CON", "w", stdout);//�ض��������
	SetConsoleTitle("����̨����");
}
void initWindow(HINSTANCE hInstance) {
	//��ƴ����ࡪ������������ʼ������ṹ
	WNDCLASSEX win_class;
	win_class.cbSize = sizeof(WNDCLASSEX);

	win_class.style = CS_HREDRAW | CS_VREDRAW;//���ڳ���仯��Ҫ���»���
	win_class.lpfnWndProc = WndProc;//�����¼��ص�����
	win_class.cbClsExtra = 0;//Ϊ������ṹ֮�����Ķ�����ֽ�������ʼ��Ϊ0
	win_class.cbWndExtra = 0;//��¼����ʵ������Ķ����ֽ�������ʼ��Ϊ0
	win_class.hInstance = hInstance;//�����˸���ʵ���ľ������ʵ�������˴��ڹ���
	win_class.hIcon = LoadIcon(NULL, IDI_APPLICATION);//��ͼ��ľ��
	win_class.hCursor = LoadCursor(NULL, IDC_ARROW);//���ָ��ľ��
	win_class.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1); //������ˢ���
	win_class.lpszMenuName = NULL;//�����˵�����Դ��
	win_class.lpszClassName = szWindowClass;//ԭ�ͻ��ַ���
	win_class.hIconSm = LoadIcon(NULL, IDI_WINLOGO);//������Сͼ��ľ��

													// ע�ᴰ����
	if (!RegisterClassEx(&win_class))
	{
		// ע��ʧ�ܣ���ӡ������Ϣ
		printf("Unexpected error trying to start the application!\n");
		fflush(stdout);
		exit(1);
	}

	//��������
	RECT wr = { windowX, windowY, windowX+windowWidth, windowY+windowHeight };
	//�����������
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);
	hwnd = CreateWindowEx(
		0,						//���ڵ���չ���
		szWindowClass,			//ָ��ע��������ָ��
		szTitle,				//ָ�򴰿����Ƶ�ָ��
		WS_OVERLAPPEDWINDOW |	//���ڵķ��
		WS_VISIBLE | WS_SYSMENU,
		wr.left, wr.top,		//�������Ͻ�����
		wr.right - wr.left,		//������
		wr.bottom - wr.top,		//����߶�
		NULL,					//�����ڵľ��
		NULL,					//�˵��ľ�������Ӵ��ڵı�ʶ��
		hInstance,				//Ӧ�ó���ʵ���ľ�� 
		nullptr);				//ָ�򴰿ڵĴ�������
	if (!hwnd)
	{
		// ����ʧ�ܣ���ӡ������Ϣ
		printf("Cannot create a window in which to draw!\n");
		fflush(stdout);
		exit(1);
	}

	//���ô��ڵ��û�����Ϊ���崴����info���ݣ��ѱ���Ҫʱ���ԴӴ����ȡ
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

	//ָ���������
	myRenderpipeline->currCamera = mainCamera;
	myRenderpipeline->clearColorBuffer();//��ջ���
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