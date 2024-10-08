#pragma once

#include "../object/F15E.h"
#include "../../macro.h"


class SceneMainGame : public vnScene
{
private:
	vnModel* pPlayer;
	vnModel* pShadow;

	vnModel* pEnemyShadow;

	vnModel* pGround;
	vnModel* pSky;

	vnModel* pCVN76;
	vnModel* pFA18C;
	vnModel* pSu30;
	vnModel* pSu33;
	vnModel* pR77[MISSILE_MAX];
	vnModel* pAIM120[MISSILE_MAX];

	cF15E* pF15E;

	vnSprite* pRadar;

	//SE
	vnSound* seFrying;
	vnSound* seMissile;
	vnSound* seMissileWarning;
	vnSound* seExplosion;
	bool isSEplay;
	bool isSEplay2;

	//�~�T�C�����˃t���O
	bool isFired[MISSILE_MAX];
	bool isEnemyFired[MISSILE_MAX];

	//���Ĕ���
	bool isPlayerShootDown;	//�v���C���[�����Ă��ꂽ�Ƃ�
	bool isEnemyShootDown;	//�G�@�����Ă��ꂽ�Ƃ�

	//�J�����؂�ւ�
	bool cameraSwitch;

	bool isDebugView;

	float count;

	//�ɍ��W�̏��
	float radius;	//���a
	float theta;	//���ʊp
	float phi;		//�p

	float rad0;
	float rad45;
	float rad90;
	float rad135;
	float rad180;
	float rad225;
	float rad270;
	float rad315;
	float rad360;

public:
	//������
	bool initialize();
	//�I��
	void terminate();

	//����
	void execute();
	//�`��
	void render();
};
