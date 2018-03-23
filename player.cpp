#include "player.h"

Player::Player()
{
	PS = NULL;
	PF = NULL;
	P = NULL;
	exist = false;
	D3DXMatrixIdentity(&matWorld);
}

Player::~Player()
{
}

BOOL Player::SetPlayer(stTexture * PS, stTexture * PF, Bullet * b, int x, int y)
{
	this->PS = PS;
	this->PF = PF;
	P = PF;
	this->p_Bullet = b;
	vPos.x = x;
	vPos.y = y;
	startTime = (float)::timeGetTime();
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

BOOL Player::SetPlayer(int pstype, int pftype, int id, Bullet * b, int x, int y)
{
	TLibClass *a = TLibClass::GetLib();
	this->PS = a->GetTemp(pstype, id);
	this->PF = a->GetTemp(pftype, id);
	P = PF;
	this->p_Bullet = b;
	vPos.x = x;
	vPos.y = y;
	startTime = (float)::timeGetTime();
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

bool Player::SetBullet(int type, int id, int x, int y, int v, float vr, int a, float ar, float r, float rc, int time)
{
	int Bullet_ID;
	if (!p_Bullet)
	{
		p_Bullet = new Bullet[1];
		if (!p_Bullet) return false;
		memset(&p_Bullet[0], 0, sizeof(Bullet));
		Bullet_ID = 0;
	}
	else
	{
		int m = 0;
		for (int i = 0; i < pBulletNum; i++) {
			if (p_Bullet[i].GetExist() == false) {
				Bullet_ID = i;
				m = 1;
				break;
			}
		}
		if (m == 0) {
			Bullet *temp;
			temp = new Bullet[pBulletNum + 1];
			if (!temp) return false;
			memset(temp, 0, sizeof(Bullet) *
				(pBulletNum + 1));

			memcpy(temp, p_Bullet,
				sizeof(Bullet) * pBulletNum);

			delete[] p_Bullet;
			p_Bullet = temp;
			Bullet_ID = pBulletNum;
			pBulletNum++;
		}
	}

	p_Bullet[Bullet_ID].SetBullet(type, id, x, y, v, vr, a, ar, r, rc, time);
	return true;

}
static int a = 0;

void Player::Run()
{
	if(a==3)
	 {
		SetBullet(BULLET1, 0, vPos.x, vPos.y, 15, 180, 0, 0, 1, 0, 1);
		startTime = (float)::timeGetTime();
		a = 0;
	}
	a++;
}



void Player::SlowMove(int a)
{
	switch (a)
	{
	case DOWN_A:
	{
		vPos.x -= vs;
		break;
	}
	case DOWN_D:
	{
		vPos.x += vs;
		break;
	}
	case DOWN_W:
	{
		vPos.y -= vs;
		break;
	}
	case DOWN_S:
	{
		vPos.y += vs;
		break;
	}
	default:
		break;
	}
	P = PS;
	D3DXMatrixTransformation2D(
		&(matWorld),
		nullptr, 0.0f,
		&D3DXVECTOR2(1, 1),
		&D3DXVECTOR2((float)((P->width) * 1) / 2, (float)((P->height) * 1) / 2),
		0.0,
		&(vPos)
	);
}

void Player::FastMove(int a)
{
	switch (a)
	{
	case DOWN_A:
	{
		vPos.x -= vf;
		break;
	}
	case DOWN_D:
	{
		vPos.x += vf;
		break;
	}
	case DOWN_W:
	{
		vPos.y -= vf;
		break;
	}
	case DOWN_S:
	{
		vPos.y += vf;
		break;
	}
	default:
		break;
	}
	P = PF;
	D3DXMatrixTransformation2D(
		&(matWorld),
		nullptr, 0.0f,
		&D3DXVECTOR2(1, 1),
		&D3DXVECTOR2((float)((P->width) * 1) / 2, (float)((P->height) * 1) / 2),
		0.0,
		&(vPos)
	);
}
