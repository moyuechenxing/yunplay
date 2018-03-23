#pragma once
#include"Plane.h"
#include"player.h"


class D3DSPRITEClass {
protected:
	int nPlaneNum;
	LPDIRECT3DDEVICE9 m_pDevice;
	LPD3DXSPRITE m_pSprite;		//精灵指针   
	Plane ** m_Plane;
	Player* m_Player;
	int state;  //阶段
	int m_nWindowWidth;    //窗口宽度  
	int m_nWindowHeight;   //窗口高度  
public:
	D3DSPRITEClass(LPDIRECT3DDEVICE9 device, LPD3DXSPRITE sprite,int x, int y);
	~D3DSPRITEClass(){ ClearUp(); };
	bool AddPlane();
	bool AddPlayer();
	int GetWindowWidth() { return m_nWindowWidth; }
	int GetWindowHeight() { return m_nWindowHeight; }
	void SetWindowSize(int w, int h) { m_nWindowWidth = w; m_nWindowHeight = h; }  //设置窗口宽度和高度的函数  
	LPDIRECT3DDEVICE9 GetD3dDevice() { return m_pDevice; } //返回D3D设备对象的函数  
	LPD3DXSPRITE GetSprite() { return m_pSprite; }
	int GetPlaneNum() { return nPlaneNum; }
	Plane *GetPlane(int id) { return &*m_Plane[id]; }
	Player *GetPlayer() { return m_Player; }
	void ClearUp(); //资源清理函数  
	void Next() {
		int x = m_Player->GetVPos().x;
		int y = m_Player->GetVPos().y;
		for (int i = 0; i < nPlaneNum; i++) {
			m_Plane[i]->Run(x, y);			
		}
		for (int i = 0; i < Plane::GetBulletNum(); i++) {
			Plane::GetBullet(i)->BulletNextFrame(m_nWindowWidth,m_nWindowHeight);
		}
		for (int i = 0; i < Player::GetBulletNum(); i++) {
			Player::GetBullet(i)->BulletNextFrame(m_nWindowWidth, m_nWindowHeight);
		}
	}
	void Shoot() {
		m_Player->Run();
	}
	void SlowMove(int a) {
		m_Player->SlowMove(a);
	}
	void FastMove(int a) {
		m_Player->FastMove(a);
	}

};
void Play(D3DSPRITEClass *S);

void Check(D3DSPRITEClass *S);

