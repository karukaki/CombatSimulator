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

	//ミサイル発射フラグ
	bool isFired[MISSILE_MAX];
	bool isEnemyFired[MISSILE_MAX];

	//撃墜判定
	bool isPlayerShootDown;	//プレイヤーが撃墜されたとき
	bool isEnemyShootDown;	//敵機が撃墜されたとき

	//カメラ切り替え
	bool cameraSwitch;

	bool isDebugView;

	float count;

	//極座標の情報
	float radius;	//半径
	float theta;	//平面角
	float phi;		//仰角

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
	//初期化
	bool initialize();
	//終了
	void terminate();

	//処理
	void execute();
	//描画
	void render();
};
