#include "Bullet.h"

Bullet::Bullet()
{
	P = NULL;
	v = a = time = 0;
	vr = vx = vy = ar = ax = ay = r = rc = 0.0;
	exist = false;
	D3DXMatrixIdentity(&matWorld);
	vPos.x = vPos.y = 0;
}

Bullet::~Bullet()
{
}

BOOL Bullet::SetBullet(stTexture *P, int x, int y, int v, float vr, int a, float ar, float r, float rc, int time)
{
	this->P=P;
	vPos.x = x;
	vPos.y = y;
	this->a= a;
	this->v = v;
	this->vr = vr;
	vy = v*sin(vr*PI / 180);
	vx = v*cos(vr*PI / 180);
	this->ar = ar;
	this->r = r;
	this->rc = rc;
	ay = a*sin(vr*PI / 180);
	ax = a*cos(vr*PI / 180);
	this->time = time;

	D3DXMatrixTransformation2D(
		&(matWorld),
		nullptr, 0.0f,
		&D3DXVECTOR2(r, r),
		&D3DXVECTOR2((float)((P->width)*r) / 2, (float)((P->height)*r) / 2),
		vr,
		&(vPos)
		);

	exist = true;
	
	return true;
}
BOOL Bullet::SetBullet(int type,int id, int x, int y, int v, float vr, int a, float ar, float r, float rc, int time)
{
	TLibClass *lib = TLibClass::GetLib();
	this->P = lib->GetTemp(type, id);
	vPos.x = x;
	vPos.y = y;
	this->a = a;
	this->v = v;
	this->vr = vr;
	vy = v*sin(vr*PI / 180);
	vx = v*cos(vr*PI / 180);
	this->ar = ar;
	this->r = r;
	this->rc = rc;
	ay = a*sin(vr*PI / 180);
	ax = a*cos(vr*PI / 180);
	this->time = time;

	D3DXMatrixTransformation2D(
		&(matWorld),
		nullptr, 0.0f,
		&D3DXVECTOR2(r, r),
		&D3DXVECTOR2((float)((P->width)*r) / 2, (float)((P->height)*r) / 2),
		vr,
		&(vPos)
		);

	exist = true;

	return true;
}

void Bullet::BulletNextFrame(int width,int height)
{
	if (exist = true) {
		if (time > 0) {
			time--;
		}
		else {
			vPos.x += vx;
			vPos.y += vy;
			if (vPos.x < 0 - P->width || vPos.y<0 - P->height || vPos.x>width || vPos.y>height)exist = false;
			if (a != 0) {
				vx += ax;
				vy += ay;
				vr = tanf(vy / vx);
			}
			r += rc;

			D3DXMatrixTransformation2D(
				&(matWorld),
				nullptr, 0.0f,
				&D3DXVECTOR2(r, r),
				&D3DXVECTOR2((float)((P->width)*r) / 2, (float)((P->height)*r) / 2),
				vr,
				&(vPos)
				);
		}
	}

}

void Bullet::ClearBullet()
{
	exist = false;
}
