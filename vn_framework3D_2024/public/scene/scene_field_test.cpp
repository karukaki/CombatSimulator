#include "../../framework.h"
#include "../../framework/vn_environment.h"

//初期化関数
bool SceneFieldTest::initialize()
{
	pPlayer = new vnModel(L"data/model/bear/", L"bear.vnm");

	pShadow = new vnModel(L"data/model/", L"shadow.vnm");
	pShadow->setTransparent(true);	//半透明描画
	//Z Fighting回避のため少し上にあげる
	pShadow->setPositionY(0.01f);

	pNPC = new vnModel(L"data/model/bear/", L"bear.vnm");
	pNPCShadow = new vnModel(L"data/model/", L"shadow.vnm");
	pDebugFan = new vnModel(L"data/model/", L"debug_fan.vnm");

	pNPCShadow->setTransparent(true);	//半透明描画
	pDebugFan->setTransparent(true);	//半透明描画
	

	pNPC->setPosition(5.0f, 0.0f, 5.0f);
	pDebugFan->setPosition(5.0f, 0.0f, 5.0f);
	pNPCShadow->setPosition(pNPC->getPosition());
	pNPCShadow->setPositionY(0.01f);

	pGround = new vnModel(L"data/model/", L"ground.vnm");
	pSky = new vnModel(L"data/model/", L"skydome.vnm");
	pSky->setLighting(false);	//ライティング無効

	pHouse = new vnModel(L"data/model/", L"Cabin1.vnm");

	//シーンの基底クラスへ登録
	for (int i = 0; i < FENCE_NUM; i++) {
		registerObject(pFence[i]);
	}
	registerObject(pPlayer);
	registerObject(pNPC);
	registerObject(pGround);
	registerObject(pSky);
	registerObject(pHouse);
	
	for (int i = 0; i < FENCE_NUM; i++) {
		pFence[i] = new vnModel(L"data/model/", L"fence.vnm");

		//角度
		float degree = (float)i * 360.0f / (float)FENCE_NUM;
		float radian = degree * 3.0141592f / 180;

		//座標の計算(極座標→直交座標)
		float x = cosf(radian) * FENCE_RADIUS;
		float z = sinf(radian) * FENCE_RADIUS;

		//オブジェクトへ設定
		pFence[i]->setPosition(z, 0.0f, x); //setPosition(x,y,z)だがx座標が90度を向いていてz座標が0度なのでxとzを入れ替えた
		//モデルデータの都合上でx,zを入れ替えた
		pFence[i]->setRotationY(radian);

	}
	
	//半透明のオブジェクトは不透明のオブジェクトの後に描画する必要がある
	registerObject(pShadow);
	registerObject(pDebugFan);
	registerObject(pNPCShadow);

	radius = 10.0f;	//半径
	theta = 0.0f; //-90.0f/180*3.141592f;	//平面角
	phi = 30.0f/180*3.141592f;		//仰角

	return true;
}

//終了関数
void SceneFieldTest::terminate()
{
	//基底クラスの登録から削除
	
	deleteObject(pPlayer);
	deleteObject(pShadow);
	deleteObject(pNPC);
	deleteObject(pDebugFan);
	deleteObject(pNPCShadow);
	deleteObject(pGround);
	deleteObject(pSky);
	deleteObject(pHouse);

	for (int i = 0; i < FENCE_NUM; i++) {
		deleteObject(pFence[i]);
	}
}

//処理関数
void SceneFieldTest::execute()
{
	//入力があるか示すフラグ
	bool input = false;

	//移動スピード
	float speed = 0.1f;

	//移動ベクトル
	XMVECTOR vMove = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	//移動
	if (vnKeyboard::on(DIK_W))
	{
		vMove = XMVectorSetZ(vMove, 1.0f);
		input = true;
		//pPlayer->addPositionZ(0.1f);
	}
	if (vnKeyboard::on(DIK_S))
	{
		vMove = XMVectorSetZ(vMove, -1.0f);
		input = true;
		//pPlayer->addPositionZ(-0.1f);
	}
	if (vnKeyboard::on(DIK_A))
	{
		vMove = XMVectorSetX(vMove, -1.0f);
		input = true;
		//pPlayer->addPositionX(-0.1f);
	}
	if (vnKeyboard::on(DIK_D))
	{
		vMove = XMVectorSetX(vMove, 1.0f);
		input = true;
		//pPlayer->addPositionX(0.1f);
	}

	//移動ベクトルにスピードを適用(長さを変える)
	vMove = XMVectorScale(vMove, speed);
	//vMove = vMove*speed;
	//vMove *= speed;

	//視線ベクトルを計算
	XMVECTOR cam_pos = *vnCamera::getPosition();
	XMVECTOR cam_trg = *vnCamera::getTarget();
	XMVECTOR eye = XMVectorSubtract(cam_trg,cam_pos);
	//XMVECTOR eye = cam_trg - cam_pos;

	//Y軸周りの回転値を計算
	float cam_rotY = atan2f(XMVectorGetX(eye),XMVectorGetZ(eye));

	//カメラのY軸周り(theta)の回転値
	XMMATRIX mRotY = XMMatrixRotationY(cam_rotY/*theta*/);
	//移動ベクトルを回転させる
	vMove = XMVector3TransformNormal(vMove, mRotY);

	//移動したときのみ処理
	if (input == true)
	{
		//進行方向(方向ベクトル:vMove.x,)
		float rotY = atan2f(XMVectorGetX(vMove), XMVectorGetZ(vMove));
		pPlayer->setRotationY(rotY);
	}


	//移動ベクトルをプレイヤーの座標に加算
	pPlayer->addPosition(&vMove);

	pShadow->setPosition(pPlayer->getPosition());	//影を追従させる
	pShadow->setPositionY(0.01f);


	//マウス操作でカメラ遠近
	if (vnMouse::onR())
	{
		radius += vnMouse::getDY() / 100.0f;
	}
	//マウス操作でカメラ回転
	if (vnMouse::onL())
	{
		theta += vnMouse::getDX() / 180.0f;
		phi += vnMouse::getDY() / 180.0f;
	}


	//フェンスからはみ出さないように座標を制限する

	//プレイヤーの原点(円の中心)からの距離を調べる
	XMVECTOR vDist = XMVector3Length(*pPlayer->getPosition());	//円の中心が原点なので-0を省略
	float dist = XMVectorGetX(vDist);

	//距離が、フェンスの半径を超えているか調べる
	if (dist > FIELD_RADIUS) {
		//超えていた場合、
		//プレイヤーの位置(座標)を半径内に収まるところに設定

		//正規化(長さを1にする)
		XMVECTOR v = XMVector3Normalize(*pPlayer->getPosition());

		//数値をかけて、設定したい長さのベクトルを作る
		v *= FIELD_RADIUS;
		//v = XMVectorScale(v,FIELD_RSDIUS);

		//半径の内側に再設計された座標にプレイヤーを移動
		pPlayer->setPosition(&v);
	}



	//カメラの回転
	if (vnKeyboard::on(DIK_UP))
	{
		phi -= 1.0f * 3.141592f / 180;
	}
	if (vnKeyboard::on(DIK_DOWN))
	{
		phi += 1.0f * 3.141592f / 180;
	}
	if (vnKeyboard::on(DIK_LEFT))
	{
		theta -= 1.0f * 3.141592f / 180;
	}
	if (vnKeyboard::on(DIK_RIGHT))
	{
		theta += 1.0f * 3.141592f / 180;
	}

	//カメラ座標の計算
	//回転していない基準となる軸のベクトル
	XMVECTOR v = XMVectorSet(0.0f, 0.0f, -radius, 0.0f);
	//回転マトリクスを作成
	XMMATRIX rotate = XMMatrixRotationRollPitchYaw(phi,theta,0.0f); //x,y,zそれぞれ何度回すか

	//基準軸(v)のベクトルを回転させる(回転マトリクスを乗算する)
	v = XMVector3TransformNormal(v, rotate);


	XMVECTOR CamPos = v; //XMVectorSet(px, py, pz, 0.0f);

	//カメラの座標にプレイヤーの座標を加算
	CamPos = XMVectorAdd(CamPos, *pPlayer->getPosition());//関数で書いたときの足し算
	//CamPos = CamPos + (*pPlayer->getPosition());
	//CamPos += (*pPlayer->getPosition());

	//少しだけ上に上げる
	XMVECTOR CamBias = XMVectorSet(0.0f, 1.5f, 0.0f, 0.0f);
	CamPos = XMVectorAdd(CamPos,CamBias);

	//カメラの座標
	vnCamera::setPosition(&CamPos);	//FPS視点setTarget

	//カメラの注視点をプレイヤーの座標に設定
	XMVECTOR CamTrg = *pPlayer->getPosition();
	CamTrg = XMVectorAdd(CamTrg, CamBias);
	vnCamera::setTarget(&CamTrg); //FPS視点setPosition


	//NPCの認識範囲
	//①プレイヤーとの距離が10以内
	XMVECTOR NpcToPlayer = *pPlayer->getPosition() - *pNPC->getPosition();
	//XMVectorSubtract(*pPlayer->getPosition(), *pNPC->getPosition());
	vDist = XMVector3Length(NpcToPlayer);
	dist = XMVectorGetX(vDist);
	
	//②プレイヤーへの角度が前方±45°以内
	//1.NPCの前方ベクトル
	XMMATRIX NpcWorld = *pNPC->getWorld();	//ワールドマトリクスを取得
	XMVECTOR NpcFront = NpcWorld.r[2];	//ワールドマトリクスの3行目が前方向ベクトル
	//2.プレイヤーへ向かうベクトル
	//NpcToPlayer

	//正規化(内積値がcosθになるように)
	NpcFront = XMVector3Normalize(NpcFront);
	NpcToPlayer = XMVector3Normalize(NpcToPlayer);

	//内積
	XMVECTOR vDot = XMVector3Dot(NpcFront, NpcToPlayer);
	float dot = XMVectorGetX(vDot);	//cosθ
	if (dot > 1.0f)dot = 1.0f;	//エラーチェック(floatのノイズカット)
	float radian = acosf(dot);
	float degree = XMConvertToDegrees(radian);

	if (dist < 10.0f && degree < 45.0f)
	{	//認識範囲に入っている

		//進行方向(プレイヤーの方向)に向ける
		float x = XMVectorGetX(NpcToPlayer);
		float z = XMVectorGetZ(NpcToPlayer);
		float rotY = atan2f(x, z);
		pNPC->setRotationY(rotY);

		//プレイヤーに向けて移動させる(プレイヤーの移動速度の半分)
		NpcToPlayer *= speed * 0.5f;
		pNPC->addPosition(&NpcToPlayer);

		pNPCShadow->setPosition(pNPC->getPosition());
		pNPCShadow->setPositionY(0.01f);
		pDebugFan->setPosition(pNPC->getPosition());
		pDebugFan->setRotationY(pNPC->getRotationY());
	}



	vnDebugDraw::Grid();
	vnDebugDraw::Axis();

	//デバッグ情報
	vnFont::print(20.0f,20.0f,L"Player : (%.3f,%.3f,%.3f)",
								pPlayer->getPositionX(),
								pPlayer->getPositionY(),
								pPlayer->getPositionZ()
								);
	vnFont::print(20.0f, 40.0f, L"Radius : %.3f", radius);
	vnFont::print(20.0f, 60.0f, L"Theta : %.3f", theta * 180/3.141592f);
	vnFont::print(20.0f, 80.0f, L"Phi : %.3f", phi * 180 / 3.141592f);
	vnFont::print(20.0f, 100.0f, L"Player_RY : %.3f", pPlayer->getRotationY()*180.0f/3.141592f);
	vnFont::print(20.0f, 120.0f, L"NPCとプレイヤーの距離 : %.3f", dist);
	vnFont::print(20.0f, 140.0f, L"NPCとプレイヤーの角度 : %.3f", degree);

	vnScene::execute();
}

//描画関数
void SceneFieldTest::render()
{
	vnScene::render();
}
