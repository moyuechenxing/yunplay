#pragma once
#include<d3d9.h>
#include<d3dx9.h>

#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib,"d3d9.lib")

#define PLANE 0
#define BULLET1 1//����14
#define BULLET2 2//�㵯��4
#define BULLET3 3//С��7
#define BULLET4 4//����8

#define BULLET0 9//�����ӵ�


#define PLANETEXTURE 1
#define BULLETTEXTURE 2

struct Texture {
	int id;
	LPDIRECT3DTEXTURE9 pTexture;    //����ָ��  
	D3DXIMAGE_INFO imageInfo;       //������ϸ��Ϣ  
};
struct stTexture
{
	Texture *mTexture;				//����
	int width;
	int height;
	RECT srcRect;					//ѡȡ����
};



class TLibClass {
private:
	LPDIRECT3DDEVICE9 pDevice;
	stTexture *bullet1Temp;
	stTexture *bullet2Temp;
	stTexture *bullet3Temp;
	stTexture *bullet4Temp;
	stTexture *planeTemp;
	Texture *m_Texture;
	int nBullet1Num;
	int nBullet2Num;
	int nBullet3Num;
	int nBullet4Num;
	int nPlaneNum;
	int nTextureNum;
	TLibClass();
	~TLibClass();
	static TLibClass *Lib;
public:

	static TLibClass * GetLib() {
		if (Lib == NULL)
			Lib = new TLibClass();
		return Lib;
	}
	void SetDevice(LPDIRECT3DDEVICE9 device) { pDevice = device; }
	bool AddTexture(wchar_t* strTemp,int id);
	bool AddTemp(int tid, int width, int height, int frame, int columns,int type);

	Texture *GetTexture(int id) {
		for (int i = 0; i < nTextureNum; i++) {
			if (m_Texture[i].id == id) {
				return &m_Texture[i];
			}
		}
		return NULL;
	}
	stTexture* GetTemp(int type, int id);

};
//TLibClass *TLibClass::Lib = NULL;