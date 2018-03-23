#include"game.h"



D3DSPRITEClass::D3DSPRITEClass(LPDIRECT3DDEVICE9 device, LPD3DXSPRITE sprite,int x,int y)
{
	m_pDevice = device;
	m_pSprite = sprite;
	m_nWindowWidth = x;
	m_nWindowHeight=y;
	state = 0;
}

bool D3DSPRITEClass::AddPlane()
{
	switch (state)
	{
	case 0: {
		m_Plane = new Plane*[10];
		for (int i = 0; i < 5; i++) {
			/*PlaneA *pa = new PlaneA;
			pa->SetPlane(PLANE, 0, NULL, 0, 30, i*50+5);
			pa->SetWay(2, 15);
			m_Plane[2 * i] = pa;
			PlaneA *pb = new PlaneA;
			pb->SetPlane(PLANE, 0, NULL, 1360, 30, i * 50 + 5);
			pb->SetWay(2, 165);
			m_Plane[2 * i + 1] = pb;*/
			m_Plane[2*i] = new PlaneB;
			m_Plane[2*i]->SetPlane(PLANE, 0, NULL, 1500, 0, 10*i);
			m_Plane[2 * i+1] = new PlaneD;
			m_Plane[2 * i+1]->SetPlane(PLANE, 0, NULL, 1500, 700, 10 * i);
		}
		
		this->nPlaneNum = 10;
		return true;
	}
	default:
		break;
	}


	
}

bool D3DSPRITEClass::AddPlayer()
{
	m_Player = new Player;
	m_Player->SetPlayer(PLANE, PLANE, 0, NULL, 1000, 400);
	return true;
}


void D3DSPRITEClass::ClearUp()
{
	
}



void Play(D3DSPRITEClass *S) {
	if (!S) return;

	LPDIRECT3DDEVICE9 device = S->GetD3dDevice();
	LPD3DXSPRITE sprite = S->GetSprite();
	if (!device)return;
	if (!sprite)return;

	sprite->Begin(D3DXSPRITE_ALPHABLEND);
	int num = Plane::GetBulletNum();
	for (int i = 0; i < num; i++) {
		Bullet *bullet = Plane::GetBullet(i);
		if (bullet->GetExist()) {
			if (bullet->GetTime() <= 0) {
				
				stTexture *pTemp = bullet->GetTemp();

				sprite->SetTransform(&(bullet->GetMatWorld()));
				sprite->Draw(pTemp->mTexture->pTexture, &(pTemp->srcRect), &D3DXVECTOR3(0, 0, 0), &D3DXVECTOR3(0, 0, 0), D3DCOLOR_XRGB(255, 255, 255));
			}
		}
	}
	for (int i = 0; i < S->GetPlaneNum();i++) {
		Plane *plane = S->GetPlane(i);
		if (plane->GetPaint()) {
			stTexture *pTemp = plane->GetTemp();

			sprite->SetTransform(&(plane->GetMatWorld()));
			sprite->Draw(pTemp->mTexture->pTexture, &(pTemp->srcRect), &D3DXVECTOR3(0, 0, 0), &D3DXVECTOR3(0, 0, 0), D3DCOLOR_XRGB(255, 255, 255));

		}
	}
	int pnum = Player::GetBulletNum();
	for (int i = 0; i < pnum; i++) {
		Bullet *bullet = Player::GetBullet(i);
		if (bullet->GetExist()) {
			if (bullet->GetTime() <= 0) {

				stTexture *pTemp = bullet->GetTemp();

				sprite->SetTransform(&(bullet->GetMatWorld()));
				sprite->Draw(pTemp->mTexture->pTexture, &(pTemp->srcRect), &D3DXVECTOR3(0, 0, 0), &D3DXVECTOR3(0, 0, 0), D3DCOLOR_XRGB(255, 255, 255));
			}
		}
	}
	Player *player = S->GetPlayer();
	if (player->GetExist()) {
		stTexture *pTemp = player->GetTemp();

		sprite->SetTransform(&(player->GetMatWorld()));
		sprite->Draw(pTemp->mTexture->pTexture, &(pTemp->srcRect), &D3DXVECTOR3(0, 0, 0), &D3DXVECTOR3(0, 0, 0), D3DCOLOR_XRGB(255, 255, 255));

	}
	
	sprite->End();

}

void Check(D3DSPRITEClass * S)
{
	D3DXVECTOR2 vPos = S->GetPlayer()->GetVPos();
	int x = vPos.x;
	int y = vPos.y;
	int num = Plane::GetBulletNum();
	for (int i = 0; i < num; i++) {
		D3DXVECTOR2 vPos2 = (Plane::GetBullet(i))->GetVPos();
		if ((vPos2.x - x)*(vPos2.x - x) +(vPos2.y-y)*(vPos2.y - y)<(1));
	}
}



