#pragma once
#include"Bullet.h"

#define TYPEA 1
#define TYPEB 2
#define TYPEC 3


class Plane {
protected:
	stTexture *P;
	D3DXMATRIX matWorld;
	D3DXVECTOR2 vPos;
	float startTime;
	BOOL exist;
	BOOL paint;
	int state;
	static Bullet *m_Bullet;
	static int nBulletNum;
	int time;
	int blood;
public:

	Plane();
	~Plane();
	BOOL SetPlane(stTexture *P, Bullet *b, int x, int y, int start_time);
	BOOL SetPlane(int ptype, int id, Bullet *b, int x, int y, int start_time);
	bool SetBullet(int type, int id, int x, int y, int v, float vr, int a, float ar, float r, float rc, int time);
	static Bullet *GetBullet(int id) {
		if (id<0 || id>nBulletNum)return NULL;
		else return &m_Bullet[id];
	}
	static int GetBulletNum() { return nBulletNum; }
	BOOL ClearPlane() { exist = false; }
	BOOL GetPaint() { return paint;}
	stTexture *GetTemp() { return P; }
	D3DXMATRIX GetMatWorld() { return matWorld; }
	D3DXVECTOR2 GetVPos() { return vPos; }
	int GetBlood() { return blood; }
	virtual void Run(int x, int y) {};
};//µÐ»ú

class PlaneA :public Plane {
private:
	int v;
	float vr;
public:
	void SetWay(int v, float vr);
	void Run(int x, int y);
};

class PlaneB :public Plane {
private:
	int m = 0;
public:
	void Run(int x, int y);
};


class PlaneC :public Plane {
private:
	int m=0;
	int moveendtime;
	float vx, vy;
public:
	void SetEnd(int x, int y, int t);
	void Run(int x, int y);
};

class PlaneD :public Plane {
private:
	int m = 0;
public:
	void Run(int x, int y);
};