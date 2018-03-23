#include "Plane.h"
#include "math.h"

Plane::Plane()
{
	P = NULL;
	exist = false;
	paint = false;
	D3DXMatrixIdentity(&matWorld);
}

Plane::~Plane()
{
}

BOOL Plane::SetPlane(stTexture * P, Bullet * b, int x, int y, int start_time)
{
	this->P = P;
	this->m_Bullet = b;
	vPos.x = x;
	vPos.y = y;
	time = start_time;
	state = 0;
	exist = true;
	D3DXMatrixTransformation2D(
		&(matWorld),
		nullptr, 0.0f,
		&D3DXVECTOR2(1, 1),
		&D3DXVECTOR2((float)((P->width)*1) / 2, (float)((P->height)*1) / 2),
		0.0,
		&(vPos)
		);
	return true;
}

BOOL Plane::SetPlane(int ptype, int id, Bullet * b, int x, int y, int start_time)
{
	TLibClass *a = TLibClass::GetLib();
	this->P = a->GetTemp(ptype,id);
	this->m_Bullet = b;
	vPos.x = x;
	vPos.y = y;
	time = start_time;
	state = 0;
	exist = true;
	D3DXMatrixTransformation2D(
		&(matWorld),
		nullptr, 0.0f,
		&D3DXVECTOR2(1, 1),
		&D3DXVECTOR2((float)((P->width) * 1) / 2, (float)((P->height) * 1) / 2),
		0.0,
		&(vPos)
		);
	return true;
}

bool Plane::SetBullet(int type, int id, int x, int y, int v, float vr, int a, float ar, float r, float rc, int time)
{
	int Bullet_ID;
		if (!m_Bullet)
		{
			m_Bullet = new Bullet[1];
			if (!m_Bullet) return false;
			memset(&m_Bullet[0], 0, sizeof(Bullet));
			Bullet_ID = 0;
		}
		else 
		{
			int m = 0;
			for (int i = 0; i < nBulletNum; i++) {
				if (m_Bullet[i].GetExist() == false) {
					Bullet_ID = i;
					m = 1;
					break;
				}
			}
			if (m == 0) {
				Bullet *temp;
				temp = new Bullet[nBulletNum + 1];
				if (!temp) return false;
				memset(temp, 0, sizeof(Bullet) *
					(nBulletNum + 1));
	
				memcpy(temp, m_Bullet,
					sizeof(Bullet) * nBulletNum);
	
				delete[] m_Bullet;
				m_Bullet = temp;
				Bullet_ID = nBulletNum;
				nBulletNum++;
			}
		}
	
		m_Bullet[Bullet_ID].SetBullet(type,id, x, y, v, vr, a, ar, r, rc, time);
		return true;
}




void PlaneA::SetWay(int v, float vr)
{
	this->v = v;
	this->vr = vr/180*PI;
}

void PlaneA :: Run(int x, int y)
{
	if (time < 0) {
		float t = ((float)::timeGetTime()) - startTime;
		if (t > 600) {
			float r = atan2f((y - vPos.y), (x - vPos.x))/PI*180;
			SetBullet(BULLET1, 3, vPos.x, vPos.y, 2, r - 30, 0, 0, 1, 0, 0);
			SetBullet(BULLET1, 3, vPos.x, vPos.y, 2, r, 0, 0, 1, 0, 0);
			SetBullet(BULLET1, 3, vPos.x, vPos.y, 2, r + 30, 0, 0, 1, 0, 0);
			startTime = (float)::timeGetTime();
		}

		vPos.x += v*cosf(vr);
		vPos.y += v*sinf(vr);

		D3DXMatrixTransformation2D(
			&(matWorld),
			nullptr, 0.0f,
			&D3DXVECTOR2(1, 1),
			&D3DXVECTOR2((float)((P->width) * 1) / 2, (float)((P->height) * 1) / 2),
			0.0,
			&(vPos)
		);
	}
	else if (time == 0) {
		startTime = (float)::timeGetTime();
		paint = true;	
		time--;
	}
	else {
		time--;
	}
}

void PlaneB::Run(int x, int y)
{
	if (time < 0) {
		if (vPos.x > 350) {
			vPos.x -= 3;
			D3DXMatrixTransformation2D(
				&(matWorld),
				nullptr, 0.0f,
				&D3DXVECTOR2(1, 1),
				&D3DXVECTOR2((float)((P->width) * 1) / 2, (float)((P->height) * 1) / 2),
				0.0,
				&(vPos)
			);
		}
		else if (vPos.x == 350) {
			m += 20;
			float l = m / ( PI * 350);
			vPos.x = 350 - sinf(l) * 350;
			vPos.y = 350 - cosf(l) * 350;
			D3DXMatrixTransformation2D(
				&(matWorld),
				nullptr, 0.0f,
				&D3DXVECTOR2(1, 1),
				&D3DXVECTOR2((float)((P->width) * 1) / 2, (float)((P->height) * 1) / 2),
				0.0,
				&(vPos)
			);
			startTime = (float)::timeGetTime();
		}
		else if (vPos.x < 5) {
			vPos.y += 2;
			D3DXMatrixTransformation2D(
				&(matWorld),
				nullptr, 0.0f,
				&D3DXVECTOR2(1, 1),
				&D3DXVECTOR2((float)((P->width) * 1) / 2, (float)((P->height) * 1) / 2),
				0.0,
				&(vPos)
			);
		}
		else{
			m += 10;
			float l = m / ( PI * 350);
			vPos.x = 350 - sinf(l) * 350;
			vPos.y = 350 - cosf(l) * 350;
			D3DXMatrixTransformation2D(
				&(matWorld),
				nullptr, 0.0f,
				&D3DXVECTOR2(1, 1),
				&D3DXVECTOR2((float)((P->width) * 1) / 2, (float)((P->height) * 1) / 2),
				0.0,
				&(vPos)
			);

			float t = ((float)::timeGetTime()) - startTime;
			if (t > 300) {
				float r = atan2f((y - vPos.y), (x - vPos.x)) / PI * 180;
				for (int i = 0; i < 10; i++) {
					SetBullet(BULLET3, 5, vPos.x, vPos.y, 0, r, 1, r, 1, 0, i);
				}
				startTime = (float)::timeGetTime();
			}
		}
		
	}
	else if (time == 0) {
		paint = true;
		time--;
	}
	else {
		time--;
	}
}

void PlaneC::SetEnd(int x, int y, int t)
{
	moveendtime = 0-t;
	vx = (x - vPos.x) / t;
	vy = (y - vPos.y) / t;
}

void PlaneC::Run(int x, int y)
{
	if (time < 0&&time>moveendtime) {
		vPos.x += vx;
		vPos.y += vy;
		D3DXMatrixTransformation2D(
			&(matWorld),
			nullptr, 0.0f,
			&D3DXVECTOR2(1, 1),
			&D3DXVECTOR2((float)((P->width) * 1) / 2, (float)((P->height) * 1) / 2),
			0.0,
			&(vPos)
		);
		time--;
	}
	else if (time <= -30) {
		float t = ((float)::timeGetTime()) - startTime;
		if (t > 300) {
			for (int i = 0; i < 6; i++) {
				SetBullet(BULLET1, 5, vPos.x, vPos.y, 2, 60 * i + m * 5, 0, 0, 1, 0, 0);
			}
			for (int i = 0; i < 6; i++) {
				SetBullet(BULLET1, 7, vPos.x, vPos.y, 2, 60 * i + 30 - m * 5, 0, 0, 1, 0, 0);
			}
			m++;
			startTime = (float)::timeGetTime();
		}
	}
	else if (time == 0) {
		startTime = (float)::timeGetTime();
		paint = true;
		time--;
	}
	else {
		time--;
	}
}

void PlaneD::Run(int x, int y)
{
	if (time < 0) {
		if (vPos.x > 350) {
			vPos.x -= 3;
			D3DXMatrixTransformation2D(
				&(matWorld),
				nullptr, 0.0f,
				&D3DXVECTOR2(1, 1),
				&D3DXVECTOR2((float)((P->width) * 1) / 2, (float)((P->height) * 1) / 2),
				0.0,
				&(vPos)
			);
		}
		else if (vPos.x == 350) {
			m += 20;
			float l = m / (PI * 350);
			vPos.x = 350 - sinf(l) * 350;
			vPos.y = 350 + cosf(l) * 350;
			D3DXMatrixTransformation2D(
				&(matWorld),
				nullptr, 0.0f,
				&D3DXVECTOR2(1, 1),
				&D3DXVECTOR2((float)((P->width) * 1) / 2, (float)((P->height) * 1) / 2),
				0.0,
				&(vPos)
			);
			startTime = (float)::timeGetTime();
		}
		else if (vPos.x < 5) {
			vPos.y -= 2;
			D3DXMatrixTransformation2D(
				&(matWorld),
				nullptr, 0.0f,
				&D3DXVECTOR2(1, 1),
				&D3DXVECTOR2((float)((P->width) * 1) / 2, (float)((P->height) * 1) / 2),
				0.0,
				&(vPos)
			);
		}
		else {
			m += 10;
			float l = m / (PI * 350);
			vPos.x = 350 - sinf(l) * 350;
			vPos.y = 350 + cosf(l) * 350;
			D3DXMatrixTransformation2D(
				&(matWorld),
				nullptr, 0.0f,
				&D3DXVECTOR2(1, 1),
				&D3DXVECTOR2((float)((P->width) * 1) / 2, (float)((P->height) * 1) / 2),
				0.0,
				&(vPos)
			);

			float t = ((float)::timeGetTime()) - startTime;
			if (t > 300) {
				float r = atan2f((y - vPos.y), (x - vPos.x)) / PI * 180;
				for (int i = 0; i < 10; i++) {
					SetBullet(BULLET3, 5, vPos.x, vPos.y, 0, r, 1, r, 1, 0, i);
				}
				startTime = (float)::timeGetTime();
			}
		}

	}
	else if (time == 0) {
		paint = true;
		time--;
	}
	else {
		time--;
	}
}