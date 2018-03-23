#include "DirectInputClass.h"

DInputClass::DInputClass()
{
	m_pDirectInput = NULL;
	m_KeyboardDevice = NULL;
	ZeroMemory(m_keyBuffer, sizeof(char) * 256);
	m_MouseDevice = NULL;
	ZeroMemory(&m_MouseState, sizeof(m_MouseState));
}

HRESULT DInputClass::Init(HWND hWnd, HINSTANCE hInstance, DWORD keyboardCoopFlags, DWORD mouseCoopFlags)
{
	HRESULT hr;
	//初始化一个IDirectInput8接口对象
	HR(DirectInput8Create(hInstance, DIRECTINPUT_VERSION,
		IID_IDirectInput8, (void**)&m_pDirectInput, NULL));

	//进行键盘设备的初始化
	HR(m_pDirectInput->CreateDevice(GUID_SysKeyboard, &m_KeyboardDevice, NULL));
	HR(m_KeyboardDevice->SetCooperativeLevel(hWnd, keyboardCoopFlags));
	HR(m_KeyboardDevice->SetDataFormat(&c_dfDIKeyboard));
	HR(m_KeyboardDevice->Acquire());
	HR(m_KeyboardDevice->Poll());

	//进行鼠标设备的初始化
	HR(m_pDirectInput->CreateDevice(GUID_SysMouse, &m_MouseDevice, NULL));
	HR(m_MouseDevice->SetCooperativeLevel(hWnd, mouseCoopFlags));
	//HR(m_MouseDevice->SetProperty(DIPROP_AXISMODE, DIPROPAXISMODE_ABS));
	HR(m_MouseDevice->SetDataFormat(&c_dfDIMouse));

	HR(m_MouseDevice->Acquire());
	HR(m_KeyboardDevice->Poll());

	return S_OK;
}


void DInputClass::GetInput()
{
	HRESULT hr = m_KeyboardDevice->GetDeviceState(sizeof(m_keyBuffer), (void**)&m_keyBuffer);
	//获取键盘输入消息
	if (hr)
	{
		m_KeyboardDevice->Acquire();
		m_KeyboardDevice->GetDeviceState(sizeof(m_keyBuffer), (LPVOID)m_keyBuffer);
	}

	hr = m_MouseDevice->GetDeviceState(sizeof(DIMOUSESTATE), (void**)&m_MouseState);
	//获取鼠标输入消息
	if (hr)
	{
		m_MouseDevice->Acquire();
		m_MouseDevice->GetDeviceState(sizeof(DIMOUSESTATE), (void**)&m_MouseState);
	}
}

bool DInputClass::IsKeyDown(int iKey)
{
	if (m_keyBuffer[iKey] & 0x80)
		return true;
	else
		return false;
}

bool DInputClass::IsMouseButtonDown(int button)
{
	return (m_MouseState.rgbButtons[button] & 0x80) != 0;
}

float DInputClass::MouseDX()
{
	return (float)m_MouseState.lX;
}

float DInputClass::MouseDY()
{
	return (float)m_MouseState.lY;
}

float DInputClass::MouseDZ()
{
	return (float)m_MouseState.lZ;
}

void DInputClass::SetPosition(HWND hWnd,POINT point)
{
	ClientToScreen(hWnd, &point);
	SetCursorPos(point.x, point.y);
}

void DInputClass::GetPosition(HWND hWnd,POINT& point)
{
	GetCursorPos(&point);
	ScreenToClient(hWnd, &point);
}


DInputClass::~DInputClass(void)
{
	if (m_KeyboardDevice != NULL)
		m_KeyboardDevice->Unacquire();
	if (m_MouseDevice != NULL)
		m_MouseDevice->Unacquire();
	SAFE_RELEASE(m_KeyboardDevice);
	SAFE_RELEASE(m_MouseDevice);
	SAFE_RELEASE(m_pDirectInput);
}