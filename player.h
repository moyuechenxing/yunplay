#pragma once
#include"Bullet.h"

#define DOWN_A 1
#define DOWN_D 2
#define DOWN_W 3
#define DOWN_S 4

#define FAST 0
#define SLOW 1


class Player {
protected:
	stTexture *PS;
	stTexture *PF;
	stTexture *P;
	D3DXMATRIX matWorld;
	D3DXVECTOR2 vPos;
	float startTime;
	BOOL exist;
	static Bullet *p_Bullet;
	static int pBulletNum;
	static int BulletType;
	int vs = 2;
	int vf = 5;
public:

	Player();
	~Player();
	BOOL SetPlayer(stTexture *PS, stTexture * PF, Bullet *b, int x, int y);
	BOOL SetPlayer(int pstype, int pftype, int id, Bullet *b, int x, int y);
	bool SetBullet(int type, int id, int x, int y, int v, float vr, int a, float ar, float r, float rc, int time);
	static Bullet *GetBullet(int id) {
		if (id<0 || id>pBulletNum)return NULL;
		else return &p_Bullet[id];
	}
	static int GetBulletNum() { return pBulletNum; }
	BOOL ClearPlayer() { exist = false; }
	BOOL GetExist() { return exist; }
	stTexture *GetTemp() { return P; }
	D3DXMATRIX GetMatWorld() { return matWorld; }
	D3DXVECTOR2 GetVPos() { return vPos; }
	virtual void Run();
	void SlowMove(int a);
	void FastMove(int a);
};//µÐ»ú

