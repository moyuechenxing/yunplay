#include "Temp.h"


TLibClass::TLibClass()
{
	pDevice=NULL;
	bullet1Temp=NULL;
	bullet2Temp=NULL;
	bullet3Temp=NULL;
	planeTemp=NULL;
	m_Texture=NULL;
	nBullet1Num=0;
	nBullet2Num=0;
	nBullet3Num=0;
	nPlaneNum=0;
	nTextureNum=0;
}

TLibClass::~TLibClass()
{
}

bool TLibClass::AddTexture(wchar_t * strTemp, int id)
{
	if (!strTemp) return false;
	if (!m_Texture)
	{
		m_Texture = new Texture[1];
		if (!m_Texture) return false;
		memset(&m_Texture[0], 0, sizeof(Texture));
	}
	else
	{
		Texture *temp;
		temp = new Texture[nTextureNum + 1];
		if (!temp) return false;
		memset(temp, 0, sizeof(Texture) *
			(nTextureNum + 1));

		memcpy(temp, m_Texture,
			sizeof(Texture) * nTextureNum);

		delete[] m_Texture;
		m_Texture = temp;
	}

	D3DXCreateTextureFromFileEx(
		pDevice,
		strTemp,
		D3DX_FROM_FILE, D3DX_FROM_FILE, 0, 0, D3DFMT_UNKNOWN,
		D3DPOOL_MANAGED, D3DX_FILTER_LINEAR, D3DX_FILTER_LINEAR,
		D3DCOLOR_XRGB(254, 254, 254), &(m_Texture[nTextureNum].imageInfo),
		nullptr, &(m_Texture[nTextureNum].pTexture));
	m_Texture[nTextureNum].id = id;
	nTextureNum++;
	return true;
}

bool TLibClass::AddTemp(int tid, int width, int height, int frame, int columns, int type)
{
	int fx, fy;
	switch (type)
	{
	case BULLET1:
		if (!bullet1Temp)
		{
			bullet1Temp = new stTexture[1];
			if (!bullet1Temp) return false;
			memset(&bullet1Temp[0], 0, sizeof(stTexture));
		}
		else
		{
			stTexture *temp;
			temp = new stTexture[nBullet1Num + 1];
			if (!temp) return false;
			memset(temp, 0, sizeof(stTexture) *
				(nBullet1Num + 1));

			memcpy(temp, bullet1Temp,
				sizeof(stTexture) * nBullet1Num);

			delete[] bullet1Temp;
			bullet1Temp = temp;
		}


		bullet1Temp[nBullet1Num].mTexture = GetTexture(tid);
		bullet1Temp[nBullet1Num].width = width;
		bullet1Temp[nBullet1Num].height = height;
		fx = (frame%columns)*width;
		fy = (frame / columns)*height;
		bullet1Temp[nBullet1Num].srcRect = { fx,fy,fx + width,fy + height };

		nBullet1Num++;
		break;
	case BULLET2:
		if (!bullet2Temp)
		{
			bullet2Temp = new stTexture[1];
			if (!bullet2Temp) return false;
			memset(&bullet2Temp[0], 0, sizeof(stTexture));
		}
		else
		{
			stTexture *temp;
			temp = new stTexture[nBullet2Num + 1];
			if (!temp) return false;
			memset(temp, 0, sizeof(stTexture) *
				(nBullet2Num + 1));

			memcpy(temp, bullet2Temp,
				sizeof(stTexture) * nBullet2Num);

			delete[] bullet2Temp;
			bullet2Temp = temp;
		}


		bullet2Temp[nBullet2Num].mTexture = GetTexture(tid);
		bullet2Temp[nBullet2Num].width = width;
		bullet2Temp[nBullet2Num].height = height;
		fx = (frame%columns)*width;
		fy = (frame / columns)*height;
		bullet2Temp[nBullet2Num].srcRect = { fx,fy,fx + width,fy + height };

		nBullet2Num++;
		break;
	case BULLET3:

		if (!bullet3Temp)
		{
			bullet3Temp = new stTexture[1];
			if (!bullet3Temp) return false;
			memset(&bullet3Temp[0], 0, sizeof(stTexture));
		}
		else
		{
			stTexture *temp;
			temp = new stTexture[nBullet3Num + 1];
			if (!temp) return false;
			memset(temp, 0, sizeof(stTexture) *
				(nBullet3Num + 1));

			memcpy(temp, bullet3Temp,
				sizeof(stTexture) * nBullet3Num);

			delete[] bullet3Temp;
			bullet3Temp = temp;
		}


		bullet3Temp[nBullet3Num].mTexture = GetTexture(tid);
		bullet3Temp[nBullet3Num].width = width;
		bullet3Temp[nBullet3Num].height = height;
		fx = (frame%columns)*width;
		fy = (frame / columns)*height;
		bullet3Temp[nBullet3Num].srcRect = { fx,fy,fx + width,fy + height };

		nBullet3Num++;
		break;
	case BULLET4:
		if (!bullet4Temp)
		{
			bullet4Temp = new stTexture[1];
			if (!bullet4Temp) return false;
			memset(&bullet4Temp[0], 0, sizeof(stTexture));
		}
		else
		{
			stTexture *temp;
			temp = new stTexture[nBullet4Num + 1];
			if (!temp) return false;
			memset(temp, 0, sizeof(stTexture) *
				(nBullet4Num + 1));

			memcpy(temp, bullet4Temp,
				sizeof(stTexture) * nBullet4Num);

			delete[] bullet4Temp;
			bullet4Temp = temp;
		}


		bullet4Temp[nBullet4Num].mTexture = GetTexture(tid);
		bullet4Temp[nBullet4Num].width = width;
		bullet4Temp[nBullet4Num].height = height;
		fx = (frame%columns)*width;
		fy = (frame / columns)*height;
		bullet4Temp[nBullet4Num].srcRect = { fx,fy,fx + width,fy + height };

		nBullet4Num++;
		break;
	case PLANE:

		if (!planeTemp)
		{
			planeTemp = new stTexture[1];
			if (!planeTemp) return false;
			memset(&planeTemp[0], 0, sizeof(stTexture));
		}
		else
		{
			stTexture *temp;
			temp = new stTexture[nPlaneNum + 1];
			if (!temp) return false;
			memset(temp, 0, sizeof(stTexture) *
				(nPlaneNum + 1));

			memcpy(temp, planeTemp,
				sizeof(stTexture) * nPlaneNum);

			delete[] planeTemp;
			planeTemp = temp;
		}


		planeTemp[nPlaneNum].mTexture = GetTexture(tid);
		planeTemp[nPlaneNum].width = width;
		planeTemp[nPlaneNum].height = height;
		fx = (frame%columns)*width;
		fy = (frame / columns)*height;
		planeTemp[nPlaneNum].srcRect = { fx,fy,fx + width,fy + height };

		nPlaneNum++;
		break;
	default:
		break;
	}

}

stTexture * TLibClass::GetTemp(int type, int id)
{
	switch (type)
	{
	case BULLET1:
		if (id< 0 || id >= nBullet1Num) return NULL;
		return &bullet1Temp[id];
		break;
	case BULLET2:
		if (id< 0 || id >= nBullet2Num) return NULL;
		return &bullet2Temp[id];
		break;
	case BULLET3:
		if (id< 0 || id >= nBullet3Num) return NULL;
		return &bullet3Temp[id];
		break;
	case PLANE:
		if (id< 0 || id >= nPlaneNum) return NULL;
		return &planeTemp[id];
		break;
	default:
		return NULL;
		break;
	}
}



