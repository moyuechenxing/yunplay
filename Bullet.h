#pragma once
#include"Temp.h"
#include <math.h>

#define PI 3.14159265

class Bullet {
private:
	stTexture *P;
	int v;
	float vr;//�ٶȷ���
	float vx;
	float vy;
	int a;
	float ar;//���ٶȷ���
	float ax;
	float ay;
	float r;//���ű���
	float rc;
	BOOL exist;
	D3DXMATRIX matWorld;
	D3DXVECTOR2 vPos;
	int time;
	//�ӵ����ȼ�����ײ����
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

