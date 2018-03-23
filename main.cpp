#define  _CRT_SECURE_NO_WARNINGS

#include<tchar.h>
#include<time.h>
#include<WinSock2.h>
#include"GUI.h"
#include"game.h"
#include"DirectInputClass.h"
#include<windowsx.h>
#include<dinput.h>
#include"ffmpeg.h"
#include "win_t.h"



#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"x86/pthreadVC2.lib")

Bullet *Plane::m_Bullet = NULL;
Bullet *Player::p_Bullet = NULL;
int Plane::nBulletNum = 0;
int Player::pBulletNum = 0;
TLibClass *TLibClass::Lib = NULL;

#define WINDOW_WIDTH 1366
#define WINDOW_HEIGHT 768
#define WINDOW_TITLE L"���ڲ���"



struct CUSTOMVERTEX
{
	FLOAT _x, _y, _z;
	FLOAT _u, _v;
	CUSTOMVERTEX(FLOAT x, FLOAT y, FLOAT z, FLOAT u, FLOAT v)
		: _x(x), _y(y), _z(z), _u(u), _v(v) {}
};
#define D3DFVF_CUSTOMVERTEX  (D3DFVF_XYZ | D3DFVF_TEX1)

LPDIRECT3DDEVICE9 g_pd3dDevice = NULL;
LPD3DXFONT g_pTextFPS = NULL;
LPD3DXFONT g_pTextHelper = NULL;
LPD3DXFONT g_pTextInfor = NULL;  // ������Ϣ��2D�ı�
float g_FPS = 0.0f;
wchar_t g_strFPS[50] = { 0 };
D3DGUIClass *g_MainGUI = NULL;
D3DGUIClass *g_StartGUI = NULL;
D3DGUIClass *g_OptionGUI = NULL;
bool g_LMBDown = false;
int g_MouseX = 0, g_MouseY = 0;
int g_MainGUIFontID = -1;
int g_StartGUIFontID = -1;
int g_OptionGUIFontID = -1;
int g_currentGUI = GUI_MAIN_SCREEN;
POINT g_point;
DInputClass* g_pDInput = NULL; //DInputClassָ��
//------------------------------------------------------------------
D3DSPRITEClass *T1;
LPD3DXSPRITE g_pSprite = nullptr;   //����ָ�� 
//------------------------------------------------------------------
LARGE_INTEGER nFreq;
LARGE_INTEGER nBeginTime;
LARGE_INTEGER nEndTime;
double ntime;

IDirect3DSurface9 *pSurface;
IDirect3DSurface9 *pBackBuffer;

byte *pDese;
dpipe_t* dpipe;
dpipe_t* dpipe2;

//--------------------------------------------------------------------------------
int pa = 0;
int pb = 0;
int pc = 0;

static int vsource_framerate_n = -1;
static int vsource_framerate_d = -1;
static int vsource_reconfigured = 0;



LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
HRESULT Direct3D_Init(HWND hwnd);
HRESULT Objects_Init(HWND hwnd);
VOID Direct3D_Render(HWND hwnd);
VOID Direct3D_Update(HWND hwnd);
VOID Direct3D_CleanUp();
float Get_FPS();
void HelpText_Render(HWND hwnd);
void GUICallback(int id, int state);
void *thread_1(void *arg);
void *thread_2(void *arg);
void *thread_3(void *arg);
void CaptureScreen();
unsigned char clip_value(unsigned char x, unsigned char min_val, unsigned char  max_val);


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lParam, int nShowCmd) {
	/*dpipe = dpipe_create(8, WINDOW_WIDTH*WINDOW_HEIGHT * 3 / 2 + 16);*/
	dpipe = dpipe_create(8, WINDOW_WIDTH*WINDOW_HEIGHT *4 );
	dpipe2 = dpipe_create(8, WINDOW_WIDTH*WINDOW_HEIGHT * 3/2);


	pthread_t id_1,id_2,id_3;
	



	WNDCLASSEX wndClass = { 0 };
	wndClass.cbSize = sizeof(WNDCLASSEX);//���ýṹ����ֽ�����С
	wndClass.style = CS_HREDRAW | CS_VREDRAW;//���ô�����ʽ
	wndClass.lpfnWndProc = WndProc;//ָ�򴰿ڹ��̺�����ָ��
	wndClass.cbClsExtra = 0;//�����฽���ڴ�
	wndClass.cbWndExtra = 0;//���ڸ����ڴ�
	wndClass.hInstance = hInstance;//�������ڹ��̵ĳ����ʵ�����
	wndClass.hIcon = (HICON)::LoadImage(NULL, L"icon.ico", IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = L"FirstWindow";

	if (!RegisterClassEx(&wndClass))//ע�ᴰ����
		return -1;

	HWND hwnd = CreateWindow(L"FirstWindow", WINDOW_TITLE, WS_OVERLAPPED, CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_WIDTH, WINDOW_HEIGHT, NULL, NULL, hInstance, NULL);

	if (!(S_OK == Direct3D_Init(hwnd)))
		MessageBox(hwnd, L"ʧ��", L"��Ϣ����", 0);


	//pthread_create(&id_1, NULL, thread_1, NULL);
	//pthread_create(&id_2, NULL, thread_2, NULL);
	//pthread_create(&id_3, NULL, thread_3, NULL);


	MoveWindow(hwnd, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, true);
	ShowWindow(hwnd, nShowCmd);
	UpdateWindow(hwnd);

	//Input��ʼ��
	g_pDInput = new DInputClass();
	g_pDInput->Init(hwnd, hInstance, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	

	PlaySound(L"bg.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);

	if(!(QueryPerformanceFrequency(&nFreq)))MessageBox(hwnd, L"��֧��΢��", L"��Ϣ����", 0);
	QueryPerformanceCounter(&nBeginTime);
	QueryPerformanceCounter(&nEndTime);
	MSG msg = { 0 };

	

	while (msg.message != WM_QUIT) {	
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
			Direct3D_Update(hwnd);
			Direct3D_Render(hwnd);
	}

	UnregisterClass(L"FirstWindow", wndClass.hInstance);//ע������

	return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_PAINT:
		Direct3D_Render(hwnd);
		ValidateRect(hwnd, NULL);
		break;

	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) {
			if (g_currentGUI == GUI_START_SCREEN)g_currentGUI = GUI_OPTION_SCREEN;
			else if(g_currentGUI == GUI_MAIN_SCREEN)
				DestroyWindow(hwnd);
		}
		break;
	case WM_DESTROY:
		Direct3D_CleanUp();
		PostQuitMessage(0);
		break;
	case WM_KEYUP:
		if (wParam == VK_ESCAPE) {
			if (g_currentGUI == GUI_START_SCREEN)g_currentGUI = GUI_OPTION_SCREEN;
			else if (g_currentGUI == GUI_MAIN_SCREEN)PostQuitMessage(0);
		}
		break;
	/*case WM_LBUTTONDOWN:
		g_LMBDown = true;
		break;
	case WM_LBUTTONUP:
		g_LMBDown = false;
		break;
	case WM_MOUSEMOVE:
		g_MouseX = GET_X_LPARAM(lParam);
		g_MouseY = GET_Y_LPARAM(lParam);
		break;*/

	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}

HRESULT Direct3D_Init(HWND hwnd) {
	//���ӿڶ���
	LPDIRECT3D9 pD3D = NULL;//�ӿڶ��󴴽�
	if (NULL == (pD3D = Direct3DCreate9(D3D_SDK_VERSION)))//��ʼ���ӿڶ��󣬲����а汾Э��
		return E_FAIL;

	//��ȡӲ���豸��Ϣ
	D3DCAPS9 caps;
	int vp = 0;
	if (FAILED(pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps))) {
		return E_FAIL;
	}
	if (caps.DevCaps&D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;//֧��Ӳ����������
	else
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	//��D3DPREVENT_PARAMETERS�ṹ��
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.BackBufferWidth = WINDOW_WIDTH;
	d3dpp.BackBufferHeight = WINDOW_HEIGHT;
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
	d3dpp.BackBufferCount = 2;
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality = 0;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = hwnd;
	d3dpp.Windowed = true;
	d3dpp.EnableAutoDepthStencil = true;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
	d3dpp.Flags = 0;
	d3dpp.FullScreen_RefreshRateInHz = 0;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	//���豸�ӿ�
	if (FAILED(pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, vp, &d3dpp, &g_pd3dDevice)))
		return E_FAIL;

	SAFE_RELEASE(pD3D);

	if (!(S_OK == Objects_Init(hwnd)))return E_FAIL;
	return S_OK;
}

HRESULT Objects_Init(HWND hwnd) {
	
	
	//��������
	D3DXCreateFont(g_pd3dDevice, 36, 0, 0, 1000, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, _T("Calibri"), &g_pTextFPS);
	D3DXCreateFont(g_pd3dDevice, 23, 0, 0, 1000, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, _T("΢���ź�"), &g_pTextHelper);
	D3DXCreateFont(g_pd3dDevice, 26, 0, 0, 1000, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, _T("����"), &g_pTextInfor);
	
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_NONE);
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_NONE);
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

	//GUI����
	g_MainGUI = new D3DGUIClass(g_pd3dDevice, WINDOW_WIDTH, WINDOW_HEIGHT);
	g_StartGUI = new D3DGUIClass(g_pd3dDevice, WINDOW_WIDTH, WINDOW_HEIGHT);
	g_OptionGUI = new D3DGUIClass(g_pd3dDevice, WINDOW_WIDTH, WINDOW_HEIGHT);

	if (!g_MainGUI)return false;

	if (!g_MainGUI->AddBackground(L"bg.jpg"))return false;
	if (!g_StartGUI->AddBackground(L"bg.jpg"))return false;
	if (!g_OptionGUI->AddBackground(L"bg.jpg"))return false;

	if (!g_MainGUI->CreateTextFont(L"΢���ź�", 28, &g_MainGUIFontID))return false;
	if (!g_StartGUI->CreateTextFont(L"΢���ź�", 28, &g_StartGUIFontID))return false;
	if (!g_OptionGUI->CreateTextFont(L"΢���ź�", 28, &g_OptionGUIFontID))return false;


	if (!g_MainGUI->AddStaticText(STATIC_ID_2, L"Game For Test",
		300, 10, D3DCOLOR_XRGB(5, 255, 255), g_MainGUIFontID)) return false;

	if (!g_MainGUI->AddButton(BUTTON_START_ID, 650, 340, L"start1.png",L"start2.png", L"start1.png")) return false;
	if (!g_MainGUI->AddButton(BUTTON_OPTION_ID, 650, 440, L"option1.png", L"option2.png", L"option1.png")) return false;
	if (!g_MainGUI->AddButton(BUTTON_QUIT_ID, 650, 550, L"quit1.png", L"quit2.png", L"quit1.png")) return false;

	if (!g_StartGUI->AddButton(BUTTON_BACK_ID, 1050, 500, L"quit1.png", L"quit2.png", L"quit1.png")) return false;

	if (!g_OptionGUI->AddButton(BUTTON_BACK_ID, 1050, 500, L"quit1.png", L"quit2.png", L"quit1.png")) return false;
	if (!g_OptionGUI->AddStaticText(STATIC_ID_2, L"Here are OPTION",
		500, 60, D3DCOLOR_XRGB(5, 255, 255), g_OptionGUIFontID)) return false;

	//-----------------------------------------------------------------------------------------------------------------------

	D3DXCreateSprite(g_pd3dDevice, &g_pSprite);
	TLibClass *a = TLibClass::GetLib();
	a->SetDevice(g_pd3dDevice);
	a->AddTexture(L"1.png", 1);
	a->AddTexture(L"2.png", 2);
	for (int i=0; i < 16; i++) {
		a->AddTemp(1, 32, 32, i, 16, BULLET1);//����
	}
	for (int i=0; i < 16; i++) {
		a->AddTemp(1, 32, 32, 176+i, 16, BULLET2);//�㵯
	}
	for (int i=0; i < 16; i++) {
		a->AddTemp(1, 32, 32, 208 + i, 16, BULLET3);//С��
	}
	for (int i=0; i < 16; i++) {
		a->AddTemp(1, 32, 32, 192 + i, 16, BULLET4);//����
	}

	a->AddTemp(2, 25, 45, 0, 16, PLANE);
	T1 = new D3DSPRITEClass(g_pd3dDevice, g_pSprite, WINDOW_WIDTH, WINDOW_HEIGHT);
	T1->AddPlane();
	T1->AddPlayer();


	return S_OK;
}
	
void GUICallback(int id, int state)
{
	switch (id)
	{
	case BUTTON_START_ID:
		//����ʼ��Ϸ����ť����ش���ʵ��
		if (state == UGP_BUTTON_DOWN) g_currentGUI = GUI_START_SCREEN;
		break;
	case BUTTON_OPTION_ID:
		//��ѡ���ť����ش���ʵ��
		if (state == UGP_BUTTON_DOWN) g_currentGUI = GUI_OPTION_SCREEN;
		break;
	case BUTTON_QUIT_ID:
		//���������˳���ť�����˳�����
		
		if (state == UGP_BUTTON_DOWN) 
				PostQuitMessage(0);
		break;
	case BUTTON_BACK_ID:
		if (state == UGP_BUTTON_DOWN)
			g_currentGUI = GUI_MAIN_SCREEN;
		break;
	}
}

VOID Direct3D_Update(HWND hwnd)
{
	//GUI��ʵ����ʱ����Ҫ������д����
	//ʹ��DirectInput���ȡ����
	
	switch (g_currentGUI) {
	case GUI_START_SCREEN:
		T1->Next();
		g_pDInput->GetInput();

		//g_pDInput->GetPosition(hwnd, g_point);
		if (g_pDInput->IsKeyDown(DIK_LSHIFT) || g_pDInput->IsKeyDown(DIK_RSHIFT)) {
			if (g_pDInput->IsKeyDown(DIK_A))T1->SlowMove(DOWN_A);
			if (g_pDInput->IsKeyDown(DIK_D))T1->SlowMove(DOWN_D);
			if (g_pDInput->IsKeyDown(DIK_W))T1->SlowMove(DOWN_W);
			if (g_pDInput->IsKeyDown(DIK_S))T1->SlowMove(DOWN_S);
		}
		else {
			if (g_pDInput->IsKeyDown(DIK_A))T1->FastMove(DOWN_A);
			if (g_pDInput->IsKeyDown(DIK_D))T1->FastMove(DOWN_D);
			if (g_pDInput->IsKeyDown(DIK_W))T1->FastMove(DOWN_W);
			if (g_pDInput->IsKeyDown(DIK_S))T1->FastMove(DOWN_S);
		}
		if (g_pDInput->IsKeyDown(DIK_SPACE))T1->Shoot();
		break;
	default:
		g_pDInput->GetInput();

		g_pDInput->GetPosition(hwnd, g_point);
		g_MouseX = g_point.x;
		g_MouseY = g_point.y;

		if (g_pDInput->IsMouseButtonDown(0)) {
			g_LMBDown = true;
		}
		else
			g_LMBDown = false;
		break;

		
	}

	

	QueryPerformanceCounter(&nEndTime);
	ntime = (double)(nEndTime.QuadPart - nBeginTime.QuadPart) / (double)nFreq.QuadPart;
	if (ntime < 0.016000)Sleep(16 - (int)(ntime * 1000));
	QueryPerformanceCounter(&nEndTime);
	ntime = (double)(nEndTime.QuadPart - nBeginTime.QuadPart) / (double)nFreq.QuadPart;
	while (ntime < 0.016667) {
		QueryPerformanceCounter(&nEndTime);
		ntime = (double)(nEndTime.QuadPart - nBeginTime.QuadPart) / (double)nFreq.QuadPart;
	}
	nBeginTime = nEndTime;

}

VOID Direct3D_Render(HWND hwnd) {
	//����
	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DCOLOR_XRGB(100, 255, 255), 1.0f, 0);

	g_pd3dDevice->BeginScene();


	//��ʽ����
	switch(g_currentGUI) {
	case GUI_MAIN_SCREEN:
		ProcessGUI(g_MainGUI, g_LMBDown, g_MouseX, g_MouseY, GUICallback);
		break;
	case GUI_START_SCREEN:
		//ProcessGUI(g_StartGUI, g_LMBDown, g_MouseX, g_MouseY, GUICallback);
		Play(T1);
		break;
	case GUI_OPTION_SCREEN:
		ProcessGUI(g_OptionGUI, g_LMBDown, g_MouseX, g_MouseY, GUICallback);
		break;
	}

	HelpText_Render(hwnd);


	//��������
	g_pd3dDevice->EndScene();
	//��ʾ��ת
	g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
}

void HelpText_Render(HWND hwnd)
{
	//����һ�����Σ����ڻ�ȡ�����ھ���
	RECT formatRect;
	GetClientRect(hwnd, &formatRect);

	//�ڴ������ϽǴ�����ʾÿ��֡��
	formatRect.top = 5;
	int charCount = swprintf_s(g_strFPS, 20, _T("FPS:%0.3f"), Get_FPS());
	g_pTextFPS->DrawText(NULL, g_strFPS, charCount, &formatRect, DT_TOP | DT_RIGHT, D3DCOLOR_RGBA(0, 239, 136, 255));

	formatRect.top = 50;
	int charC = swprintf_s(g_strFPS, 20, _T("%d   %d"), g_MouseX,g_MouseY);
	g_pTextFPS->DrawText(NULL, g_strFPS, charC, &formatRect, DT_TOP | DT_RIGHT, D3DCOLOR_RGBA(0, 239, 136, 255));

	formatRect.top = 100;
	int charCI = swprintf_s(g_strFPS, 20, _T("%d  %d  %d"), pa,pb,pc);
	g_pTextFPS->DrawText(NULL, g_strFPS, charCI, &formatRect, DT_TOP | DT_RIGHT, D3DCOLOR_RGBA(0, 239, 136, 255));

	formatRect.top = 200;
	int charNum = swprintf_s(g_strFPS, 20, _T("Num:%d"), Plane::GetBulletNum());
	g_pTextFPS->DrawText(NULL, g_strFPS, charNum, &formatRect, DT_TOP | DT_RIGHT, D3DCOLOR_RGBA(0, 239, 136, 255));
}

VOID Direct3D_CleanUp() {
	SAFE_RELEASE(g_pd3dDevice);
	SAFE_RELEASE(g_pTextFPS);
	SAFE_RELEASE(g_pd3dDevice);
	SAFE_DELETE(g_MainGUI);
	SAFE_DELETE(g_OptionGUI);
	SAFE_DELETE(g_StartGUI);
}

float Get_FPS() {
	static float fps = 0;
	static int frameCount = 0;//֡��
	static float currentTime = 0.0f;//��ǰʱ��
	static float lastTime = 0.0f;//����ʱ��

	frameCount++;
	currentTime = timeGetTime()*0.001f;
	if (currentTime - lastTime > 1.0f) {
		fps = (float)frameCount / (currentTime - lastTime);
		lastTime = currentTime;
		frameCount = 0;
	}
	return fps;
}

void CaptureScreen() {
	static struct timeval initialTv, lastTv, captureTv;
	static int token;
	static int frame_interval;
	// reset framerate setup
	vsource_framerate_n = VIDEO_FPS;
	vsource_framerate_d = 1;
	vsource_reconfigured = 0;

	frame_interval = 1000000 / VIDEO_FPS;	// in the unif of us
	frame_interval++;

	gettimeofday(&initialTv, NULL);
	lastTv = initialTv;
	token = frame_interval;
	gettimeofday(&lastTv, NULL);//����ffpmeg��ʼ
	while (1) {
		gettimeofday(&captureTv, NULL);
		token += tvdiff_us(&captureTv, &lastTv);
		if (token > (frame_interval << 1)) {
			token = (frame_interval << 1);
		}
		lastTv = captureTv;
		//
		if (token < frame_interval) {
			Sleep(1);
			continue;
		}

		token -= frame_interval;

		g_pd3dDevice->CreateOffscreenPlainSurface(WINDOW_WIDTH, WINDOW_HEIGHT, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &pSurface, NULL);
		g_pd3dDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);
		g_pd3dDevice->GetRenderTargetData(pBackBuffer, pSurface);
		D3DLOCKED_RECT d3d_rect;
		/*D3DXSaveSurfaceToFile(L"mm.bmp", D3DXIFF_BMP, pSurface, NULL, NULL);*/
		pSurface->LockRect(&d3d_rect, NULL, D3DLOCK_DONOTWAIT);
		byte * pImage = (byte *)d3d_rect.pBits;			//��С����:bgra
		gettimeofday(&captureTv, NULL);



		//pDese = (byte *)malloc(sizeof(byte)*WINDOW_HEIGHT*WINDOW_WIDTH * 4);
		dpipe_buffer_t *data = NULL;
		data = dpipe_get(dpipe);
		//data->pointer = yuvBuf;
		pDese = (byte *)data->pointer;
		data->imgpts = tvdiff_us(&captureTv, &initialTv) / frame_interval;
		data->timestamp = captureTv;
		byte *p = pDese;


		for (int i = 0; i < WINDOW_HEIGHT; i++) {
			memcpy(pDese, pImage, WINDOW_WIDTH * 4);
			pDese += (WINDOW_WIDTH * 4);
			pImage += d3d_rect.Pitch;
		}
		pDese = p;
		pSurface->UnlockRect();
		pSurface->Release();

		dpipe_store(dpipe, data);

		pa++;

		//--------------------------------------------------------------------------------------------------------------------------




		/*int ta, tb, tc;
		int t1 = timeGetTime();
		int t2 = timeGetTime();*/
		/*t1 = timeGetTime();
		pDese = p;
		tplay(pDese);
		t2 = timeGetTime();
		ta = t2 - t1;*/


		/*t1 = timeGetTime();
		pDese = p;
		wa(pDese);
		t2 = timeGetTime();
		tb = t2 - t1;*/



		/*t1 = timeGetTime();
		pDese = p;

		unsigned char *yuvBuf = (unsigned char *)malloc(WINDOW_HEIGHT*WINDOW_WIDTH * 3 / 2);
		unsigned char*ptrY, *ptrU, *ptrV, *ptrRGB;
		memset(yuvBuf, 0, WINDOW_HEIGHT*WINDOW_WIDTH * 3 / 2);
		ptrY = yuvBuf;
		ptrU = yuvBuf + WINDOW_HEIGHT*WINDOW_WIDTH;
		ptrV = ptrU + (WINDOW_HEIGHT*WINDOW_WIDTH * 1 / 4);
		unsigned char y, u, v, r, g, b;
		for (int j = 0; j<WINDOW_HEIGHT; j++) {
			ptrRGB = pDese + WINDOW_WIDTH*j * 4;
			for (int i = 0; i<WINDOW_WIDTH; i++) {
				b = *(ptrRGB++);
				g = *(ptrRGB++);
				r = *(ptrRGB++);
				ptrRGB++;
				y = (unsigned char)((66 * r + 129 * g + 25 * b + 128) >> 8) + 16;
				u = (unsigned char)((-38 * r - 74 * g + 112 * b + 128) >> 8) + 128;
				v = (unsigned char)((112 * r - 94 * g - 18 * b + 128) >> 8) + 128;
				*(ptrY++) = clip_value(y, 0, 255);
				if (j % 2 == 0 && i % 2 == 0) {
					*(ptrU++) = clip_value(u, 0, 255);
				}
				else {
					if (i % 2 == 0) {
						*(ptrV++) = clip_value(v, 0, 255);
					}
				}
			}
		}*/




		/*FILE *fp1 = fopen("output.yuv", "wb+");
		fwrite(yuvBuf, 1, WINDOW_WIDTH*WINDOW_HEIGHT * 3 / 2, fp1);
		fclose(fp1);*/

		/*t2 = timeGetTime();
		tc = t2 - t1;
		printf("%d  %d  %d", ta, tb, tc);*/
	}
}

void *thread_1(void *arg)
{
	/*play(WINDOW_WIDTH,WINDOW_HEIGHT);*/
	/*tplay();*/
	/*while (1) {
		wa();
	}*/
	wb();
	return arg;
}
void *thread_2(void *arg) {
	playm();
	return arg;
}

void *thread_3(void *arg) {
	CaptureScreen();
	return arg;
}

unsigned char clip_value(unsigned char x, unsigned char min_val, unsigned char  max_val) {
	if (x>max_val) {
		return max_val;
	}
	else if (x<min_val) {
		return min_val;
	}
	else {
		return x;
	}
}