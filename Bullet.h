#pragma once
#include"Temp.h"
#include <math.h>

#define PI 3.14159265

class Bullet {
private:
	stTexture *P;
	int v;
	float vr;//速度方向
	float vx;
	float vy;
	int a;
	float ar;//加速度方向
	float ax;
	float ay;
	float r;//缩放倍数
	float rc;
	BOOL exist;
	D3DXMATRIX matWorld;
	D3DXVECTOR2 vPos;
	int time;
	//子弹优先级，碰撞计算
public:
	Bullet();
	~Bullet();
	BOOL SetBullet(stTexture *P, int x, int y, int v, float vr, int a, float ar, float r, float rc, int time);
	BOOL SetBullet(int type, int id, int x, int y, int v, float vr, int a, float ar, float r, float rc, int time);
	void BulletNextFrame(int width, int height);
	void ClearBullet();

	BOOL GetExist() { return exist; }
	stTexture *GetTemp() { return P; }
	D3DXMATRIX GetMatWorld() { return matWorld; }
	D3DXVECTOR2 GetVPos() { return vPos; }
	int GetTime() { return time; }

	
};

