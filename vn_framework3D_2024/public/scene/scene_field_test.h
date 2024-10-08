#pragma once

#define FENCE_NUM		(32)	//フェンスモデル

#define FENCE_RADIUS	(37.5f)	//フェンスを配置する円周の半径

#define FIELD_RADIUS	(36.5f)	//フェンスの当たり判定を取る円周の半径

class SceneFieldTest : public vnScene
{
private:
	vnModel* pFence[FENCE_NUM];

	vnModel* pPlayer;
	vnModel* pShadow;

	vnModel* pNPC;
	vnModel* pNPCShadow;
	vnModel* pDebugFan;

	vnModel* pGround;
	vnModel* pSky;
	vnModel* pHouse;


	//極座標の情報
	float radius;	//半径
	float theta;	//平面角
	float phi;		//仰角

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
