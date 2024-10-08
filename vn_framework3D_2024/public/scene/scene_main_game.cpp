#include "../../framework.h"
#include "../../framework/vn_environment.h"
#include "scene_main_game.h"

//初期化関数
bool SceneMainGame::initialize()
{
	rad0 = 0.0f;
	rad45 = XMConvertToRadians(45.0f);
	rad90 = XMConvertToRadians(90.0f);
	rad135 = XMConvertToRadians(135.0f);
	rad180 = XMConvertToRadians(180.0f);
	rad225 = XMConvertToRadians(225.0f);
	rad270 = XMConvertToRadians(270.0f);
	rad315 = XMConvertToRadians(315.0f);
	rad360 = XMConvertToRadians(360.0f);


	pShadow = new vnModel(MODEL_FOLDER, L"shadow.vnm");
	pShadow->setTransparent(true);	//半透明描画
	//Z Fighting回避のため少し上にあげる
	pShadow->setPositionY(0.01f);

	pEnemyShadow = new vnModel(MODEL_FOLDER, L"shadow.vnm");
	pEnemyShadow->setTransparent(true);
	pEnemyShadow->setPositionY(0.01f);

	pGround = new vnModel(MODEL_FOLDER, L"ground.vnm");
	pSky = new vnModel(MODEL_FOLDER, L"skydome.vnm");
	pSky->setLighting(false);	//ライティング無効
	pSky->setScale(10.0f, 10.0f, 10.0f);

	pCVN76 = new vnModel(MAINGAME_FOLDER, CVN76_FAIL);
	pCVN76->setPosition(0.0f, -20.0f, -50.0f);
	pCVN76->setLighting(false);
	pFA18C = new vnModel(MAINGAME_FOLDER, FA18C_FLIGHT_FAIL);
	pFA18C->setPosition(0.0f,30.0f, 0.0f);
	pSu30 = new vnModel(MAINGAME_FOLDER, Su30_FLIGHT_FAIL);
	pSu30->setPosition(0.0f, 50.0f, 1000.0f);
	pSu30->setRotationY(rad180);

	pF15E = new cF15E(MAINGAME_FOLDER, F15E_FAIL);	//F15Eが表示されない
	pF15E->setPosition(0.0f, 50.0f, 500.0f);
	pF15E->setRotationY(rad180);

	for (int i = 0; i < MISSILE_MAX; i++) {
		pAIM120[i] = new vnModel(MAINGAME_FOLDER, AIM120_FAIL);
		pR77[i] = new vnModel(MAINGAME_FOLDER, R77_FAIL);
	}
	

	//pRadar = new vnSprite(200.0f, 400.0f, 262.0f, 386.0f, L"data/image/レーダー警告受信機.png");

	//SE
	seFrying = new vnSound(L"data/sound/戦闘機内（飛行中）.wav");
	seFrying->setVolume(0.5f);
	seMissile = new vnSound(L"data/sound/ミサイル発射2.wav");
	seMissileWarning = new vnSound(L"data/sound/ミサイル接近アラーム.wav");
	seMissileWarning->setVolume(0.7f);
	seExplosion = new vnSound(L"data/sound/爆発.wav");
	isSEplay = true;
	isSEplay2 = true;

	//シーンの基底クラスへ登録
	//registerObject(pGround);
	registerObject(pSky);
	registerObject(pCVN76);
	registerObject(pFA18C);
	registerObject(pSu30);
	registerObject(pF15E);
	//registerObject(pRadar);
	for (int i = 0; i < MISSILE_MAX; i++) {
	registerObject(pAIM120[i]);
	registerObject(pR77[i]);
	}

	//半透明のオブジェクトは不透明のオブジェクトの後に描画する必要がある
	registerObject(pShadow);
	registerObject(pEnemyShadow);
	


	for (int i = 0; i < MISSILE_MAX; i++) {
		isFired[i] = false;
		isEnemyFired[i] = false;
	}
	
	isPlayerShootDown = false;
	isEnemyShootDown = false;

	cameraSwitch = false;

	isDebugView = false;

	radius = 20.0f;	//半径
	theta = 0.0f; //-90.0f/180*3.141592f;	//平面角
	phi = 10.0f / 180 * 3.141592f;		//仰角

	count = 0.0f;

	

	return true;
}

//終了関数
void SceneMainGame::terminate()
{
	//基底クラスの登録から削除

	deleteObject(pEnemyShadow);
	deleteObject(pShadow);
	for (int i = 0; i < MISSILE_MAX; i++) {
		deleteObject(pR77[i]);
		deleteObject(pAIM120[i]);
	}
	deleteObject(pF15E);
	deleteObject(pSu30);
	deleteObject(pFA18C);
	deleteObject(pCVN76);
	deleteObject(pSky);
	

	//SE
	delete seFrying;
	delete seMissile;
	delete seMissileWarning;
	delete seExplosion;

}

//処理関数
void SceneMainGame::execute()
{
	//入力があるか示すフラグ
	bool input = false;

	//移動スピード
	float speed = 0.5f;
	float enemySpeed = 0.4f;
	float speedThrottle = 0.1f;
	

	XMVECTOR front=XMVectorSet(0.0f,0.0f,speed,0.0f);
	XMVECTOR missileFront=XMVectorSet(0.0f,0.0f,1.0f,0.0f);
	//回転行列と同じように使える
	XMMATRIX mRot = XMMatrixRotationRollPitchYaw(pFA18C->getRotationX(), pFA18C->getRotationY(), pFA18C->getRotationZ());
	XMVECTOR q = XMQuaternionRotationMatrix(mRot);	//回転行列から変換
	XMMATRIX m = XMMatrixRotationQuaternion(q);		//回転行列へ変換
	front = XMVector3TransformNormal(front, mRot);
	missileFront = XMVector3TransformNormal(missileFront, m);
	pFA18C->addPosition(&front);	//回転した方向に移動する
	pSky->addPosition(&front);	//背景をプレイヤーの動きに追従される

	XMVECTOR enemyFront=XMVectorSet(0.0f,0.0f,enemySpeed,0.0f);
	XMMATRIX enemyRot = XMMatrixRotationRollPitchYaw(pSu30->getRotationX(), pSu30->getRotationY(), pSu30->getRotationZ());
	enemyFront = XMVector3TransformNormal(enemyFront, enemyRot);
	pSu30->addPosition(&enemyFront);


	//敵の原点からの距離を調べる
	XMVECTOR vDist = XMVector3Length(*pSu30->getPosition());
	float dist = XMVectorGetX(vDist);
	//プレイヤーの原点からの距離を調べる
	XMVECTOR player_vDist = XMVector3Length(*pFA18C->getPosition());
	float playerDist = XMVectorGetX(player_vDist);
	

	//ミサイルの認識範囲
	//①敵機との距離が○○以内
	XMVECTOR MissileToEnemy;	//敵機からミサイルを引いた位置
	XMMATRIX MissileWorld;		//ミサイルのワールドマトリクス
	XMVECTOR MissileFront;		//ミサイルの前方向ベクトル
	XMVECTOR vDot_x;
	float dot_x;
	float radian;
	float degree;

	XMVECTOR R77ToPlayer;	//プレイヤーからミサイルを引いた位置
	XMMATRIX R77World;		//ミサイルのワールドマトリクス
	XMVECTOR R77Front;		//ミサイルの前方向ベクトル
	XMVECTOR e_vDot_x;
	float e_dot_x;
	float e_radian;
	float e_degree;

	
	//飛行中SEループ再生
	if (!isPlayerShootDown&&isSEplay)
	{
		seFrying->play();
		isSEplay = false;
	}
	if (seFrying->isStopped())
	{
		isSEplay = true;
	}

	//移動ベクトル
	XMVECTOR vMove = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	//移動
	//機体のスロットル
	/*if (vnKeyboard::on(DIK_R))
	{
		speedThrottle = 0.1f;
	}
	if (vnKeyboard::on(DIK_F))
	{
		speedThrottle = -0.1f;
	}*/
	//デバッグ情報表示
	if (vnKeyboard::trg(DIK_P))
	{
		isDebugView = true;
	}
	//カメラを敵機に切り替え
	if (vnKeyboard::trg(DIK_R))
	{
		cameraSwitch = true;
	}
	if (vnKeyboard::trg(DIK_F))
	{
		cameraSwitch = false;
	}
	//ピッチ
	if (vnKeyboard::on(DIK_W))
	{
		pFA18C->addRotationX(-0.01f);

		/*if (pFA18C->getRotationZ() < rad45 && pFA18C->getRotationZ() < rad135) {
			pFA18C->addRotationY(0.01f);
		}
		if (pFA18C->getRotationZ() > -rad45 && pFA18C->getRotationZ() > -rad135) {
			pFA18C->addRotationY(-0.01f);
		}*/
		
	}
	if (vnKeyboard::on(DIK_S))
	{
		pFA18C->addRotationX(0.01f);
	}
	//ヨー
	if (vnKeyboard::on(DIK_E))
	{
		pFA18C->addRotationY(0.01f);
	}
	if (vnKeyboard::on(DIK_Q))
	{
		pFA18C->addRotationY(-0.01f);
	}
	//ロール
	if (vnKeyboard::on(DIK_A))
	{
		pFA18C->addRotationZ(0.01f);
	}
	if (vnKeyboard::on(DIK_D))
	{
		pFA18C->addRotationZ(-0.01f);
	}
	//ミサイル発射
	if (vnKeyboard::trg(DIK_SPACE)) 
	{
		for (int i = 0; i < MISSILE_MAX; ++i) {
			if (!isFired[i]) {
				seMissile->play();
				isFired[i] = true;
				break;
			}
		}
	}
	
	//敵機(Su30)の移動処理
	//Z座標-500を下回ったら
	if (pSu30->getPositionZ() < -1100.0f)
	{
		pSu30->addRotationY(0.01f);
	}
	//Z座標500を超えたら
	if (pSu30->getPositionZ() > 1100.0f)
	{
		pSu30->addRotationY(0.01f);
	}


	//ミサイルを撃っていないとき機体と一緒に動く
	//ミサイルを一発ずつ撃つ
	for (int i = 0; i < MISSILE_MAX; ++i) 
	{
		if (!isFired[i]) {
			//ミサイルの位置
			XMVECTOR AIM120_offset[4] = {
			XMVectorSet(2.0f, 1.5f, -2.0f, 0.0f),
			XMVectorSet(3.0f, 1.5f, -2.0f, 0.0f),
			XMVectorSet(-2.0f, 1.5f, -2.0f, 0.0f),
			XMVectorSet(-3.0f, 1.5f, -2.0f, 0.0f)
			};

			XMVECTOR AIM120_front = XMVector3TransformNormal(AIM120_offset[i], mRot);
			XMVECTOR AIM120_pos = XMVectorAdd(AIM120_front, *pFA18C->getPosition());
			pAIM120[i]->setPosition(&AIM120_pos);

			pAIM120[i]->setRotation(pFA18C->getRotation());
		}
		//ミサイルが発射されたとき
		if (isFired[i]) {
			//ミサイルの挙動
			//pAIM120[i]->addPositionZ(1.0f);

			//ミサイルの認識範囲
			//①敵機との距離が○○以内
			MissileToEnemy = *pSu30->getPosition() - *pAIM120[i]->getPosition();

			//XMVectorSubtract(*pPlayer->getPosition(), *pNPC->getPosition());
			vDist = XMVector3Length(MissileToEnemy);
			dist = XMVectorGetX(vDist);

			//②敵機への角度が前方±45°以内
			//1.ミサイルの前方ベクトル

			MissileWorld = *pAIM120[i]->getWorld();	//ワールドマトリクスを取得

			MissileFront = MissileWorld.r[2];	//ワールドマトリクスの3行目が前方向ベクトル
			//2.敵機へ向かうベクトル
			//MissileToEnemy

			//正規化(内積値がcosθになるように)
			MissileFront = XMVector3Normalize(MissileFront);
			MissileToEnemy = XMVector3Normalize(MissileToEnemy);

			//内積
			vDot_x = XMVector3Dot(MissileFront, MissileToEnemy);
			dot_x = XMVectorGetX(vDot_x);	//cosθ
			if (dot_x > 1.0f)dot_x = 1.0f;	//エラーチェック(floatのノイズカット)
			radian = acosf(dot_x);
			degree = XMConvertToDegrees(radian);


			//ミサイルの認識範囲
			if (dist < 300.0f && degree < 45.0f /*&& dist > -300.0f && degree < 45.0f*/)
			{	//認識範囲に入っている

				//進行方向(敵機の方向)に向ける
				float x = XMVectorGetX(MissileToEnemy);
				float y = XMVectorGetY(MissileToEnemy);
				float z = XMVectorGetZ(MissileToEnemy);

				//Y軸回りの回転（水平回転）
				float rotY = atan2f(x, z);
				// X軸回りの回転（ピッチ角）
				float dist_xz = sqrtf(x * x + z * z);
				float rotX = atan2f(y, dist_xz);
				// ミサイルの向きを設定
				pAIM120[i]->setRotationY(rotY);
				pAIM120[i]->setPositionX(rotX);

				//敵機に向けて移動させる(ミサイルの移動速度)
				MissileToEnemy *= 1.5f;
				pAIM120[i]->addPosition(&MissileToEnemy);

				//vnFont::print(20.0f, 360.0f, L"ミサイルと敵機の角度 : %.3f", degree);
			}
			//ミサイルの認識範囲外
			else
			{
				pAIM120[i]->addPosition(&missileFront);
				//pAIM120[i]->setRotation(pFA18C->getRotation());
			}

			//ミサイルが敵機に命中したとき
			if (dist < 2.0f) {
				if (!isPlayerShootDown){
					isEnemyShootDown = true;
					isSEplay2 = false;
					seExplosion->play();
				}
				deleteObject(pAIM120[i]);
			}
		}

		//敵機のミサイル処理
		//ミサイルの認識範囲
		//①敵機との距離が○○以内
		R77ToPlayer = *pFA18C->getPosition() - *pR77[i]->getPosition();

		//XMVectorSubtract(*pPlayer->getPosition(), *pNPC->getPosition());
		player_vDist = XMVector3Length(R77ToPlayer);
		playerDist = XMVectorGetX(player_vDist);

		//②敵機への角度が前方±45°以内
		//1.ミサイルの前方ベクトル

		R77World = *pR77[i]->getWorld();	//ワールドマトリクスを取得

		R77Front = R77World.r[2];	//ワールドマトリクスの3行目が前方向ベクトル
		//2.敵機へ向かうベクトル
		//MissileToEnemy

		//正規化(内積値がcosθになるように)
		R77Front = XMVector3Normalize(R77Front);
		R77ToPlayer = XMVector3Normalize(R77ToPlayer);

		//内積
		e_vDot_x = XMVector3Dot(R77Front, R77ToPlayer);
		e_dot_x = XMVectorGetX(e_vDot_x);	//cosθ
		if (e_dot_x > 1.0f)e_dot_x = 1.0f;	//エラーチェック(floatのノイズカット)
		e_radian = acosf(e_dot_x);
		e_degree = XMConvertToDegrees(e_radian);

		//ミサイルの認識範囲
		if (playerDist < 300.0f && e_degree < 45.0f && playerDist > -300.0f && e_degree < 45.0f)
		{	//認識範囲に入っている

			//vnFont::print(20.0f, 400.0f, L"プレイヤーの距離 : %.3f", playerDist);

			//進行方向(敵機の方向)に向ける
			float e_x = XMVectorGetX(R77ToPlayer);
			float e_y = XMVectorGetY(R77ToPlayer);
			float e_z = XMVectorGetZ(R77ToPlayer);

			//Y軸回りの回転（水平回転）
			float e_rotY = atan2f(e_x, e_z);
			// X軸回りの回転（ピッチ角）
			float e_dist_xz = sqrtf(e_x * e_x + e_z * e_z);
			float e_rotX = atan2f(e_y, e_dist_xz);
			// ミサイルの向きを設定
			pR77[i]->setRotationY(e_rotY);
			pR77[i]->setPositionX(e_rotX);

			//敵機に向けて移動させる(ミサイルの移動速度)
			R77ToPlayer *= 1.0f;
			pR77[i]->addPosition(&R77ToPlayer);

			//ミサイル警告SE
			if (isSEplay2)
			{
				seMissileWarning->play();
				isSEplay2 = false;
			}
			if (seMissileWarning->isStopped())
			{
				isSEplay2 = true;
			}

			vnFont::print(20.0f, 60.0f, L"TRACK");
		}
		//敵機のミサイルが撃っていないとき
		else {
			//ミサイルの位置
			XMVECTOR R77_offset[4] = {
			XMVectorSet(3.45f, 2.0f, -1.5f, 0.0f),
			XMVectorSet(4.7f, 2.0f, -2.0f, 0.0f),
			XMVectorSet(-3.45f, 2.0f, -1.5f, 0.0f),
			XMVectorSet(-4.7f, 2.0f, -2.0f, 0.0f)
			};
			XMVECTOR R77_front = XMVector3TransformNormal(R77_offset[i], enemyRot);
			XMVECTOR R77_pos = XMVectorAdd(R77_front, *pSu30->getPosition());
			pR77[i]->setPosition(&R77_pos);

			pR77[i]->setRotation(pSu30->getRotation());
		}

		//ミサイルがplayerに命中したとき
		if (playerDist < 2.0f) {
			if (!isEnemyShootDown) {
				isPlayerShootDown = true;
				isSEplay2 = false;
				seExplosion->play();
			}
			deleteObject(pR77[i]);
		}



	}
	//敵を撃墜したとき
	if (isEnemyShootDown)
	{
		count++;
		cameraSwitch = false;
		deleteObject(pSu30);
	}
	if (isPlayerShootDown)
	{
		count++;
		cameraSwitch = true;
		deleteObject(pFA18C);
	}
	if (count > 500.0f)
	{
		switchScene(eSceneTable::Title);
	}

	



	//前に進み続ける
	//pFA18C->addPositionZ(speedThrottle);
	//vout = XMVectorScale(vout, speedThrottle);


	//移動ベクトルにスピードを適用(長さを変える)
	//vMove = XMVectorScale(vMove, speed);
	//vMove = vMove*speed;
	//vMove *= speed;
	

	//360度を超えれば0度に戻す
	if (pFA18C->getRotationX() > rad360 || pFA18C->getRotationX() < -rad360)
	{
		pFA18C->setRotationX(0.0f);
	}
	if (pFA18C->getRotationY() > rad360 || pFA18C->getRotationY() < -rad360)
	{
		pFA18C->setRotationY(0.0f);
	}
	if (pFA18C->getRotationZ() > rad360 || pFA18C->getRotationZ() < -rad360)
	{
		pFA18C->setRotationZ(0.0f);
	}


	//視線ベクトルを計算
	XMVECTOR cam_pos = *vnCamera::getPosition();
	XMVECTOR cam_trg = *vnCamera::getTarget();
	XMVECTOR eye = XMVectorSubtract(cam_trg, cam_pos);
	//XMVECTOR eye = cam_trg - cam_pos;

	//Y軸周りの回転値を計算
	float cam_rotY = atan2f(XMVectorGetX(eye), XMVectorGetZ(eye));

	//カメラのY軸周り(theta)の回転値
	XMMATRIX mRotY = XMMatrixRotationY(cam_rotY/*theta*/);
	//移動ベクトルを回転させる
	vMove = XMVector3TransformNormal(vMove, mRotY);

	//移動したときのみ処理
	if (input == true)
	{
		//進行方向(方向ベクトル:vMove.x,)
		float rotY = atan2f(XMVectorGetX(vMove), XMVectorGetZ(vMove));
		pFA18C->setRotationY(rotY);
	}


	//移動ベクトルをプレイヤーの座標に加算
	pFA18C->addPosition(&vMove);

	pShadow->setPosition(pFA18C->getPosition());	//影を追従させる
	pShadow->setPositionY(0.01f);

	pEnemyShadow->setPosition(pSu30->getPosition());
	pEnemyShadow->setPositionY(0.01f);

	//敵機との距離
	float playerToEnemyDist = playerDist - dist;

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


	//カメラ座標の計算
	//回転していない基準となる軸のベクトル
	XMVECTOR v = XMVectorSet(0.0f, 0.0f, -radius, 0.0f);
	//回転マトリクスを作成
	XMMATRIX rotate = XMMatrixRotationRollPitchYaw(phi, theta, 0.0f); //x,y,zそれぞれ何度回すか

	//基準軸(v)のベクトルを回転させる(回転マトリクスを乗算する)
	v = XMVector3TransformNormal(v, rotate);


	XMVECTOR CamPos = v; //XMVectorSet(px, py, pz, 0.0f);

	if (!cameraSwitch)
	{
		//カメラの座標にプレイヤーの座標を加算
		CamPos = XMVectorAdd(CamPos, *pFA18C->getPosition());//関数で書いたときの足し算
		//CamPos = CamPos + (*pFA18C->getPosition());
		//CamPos += (*pFA18C->getPosition());

		//少しだけ上に上げる
		XMVECTOR CamBias = XMVectorSet(0.0f, 1.5f, 0.0f, 0.0f);
		CamPos = XMVectorAdd(CamPos, CamBias);

		//カメラの座標
		vnCamera::setPosition(&CamPos);	//FPS視点setTarget

		//カメラの注視点をプレイヤーの座標に設定
		XMVECTOR CamTrg = *pFA18C->getPosition();
		CamTrg = XMVectorAdd(CamTrg, CamBias);
		vnCamera::setTarget(&CamTrg); //FPS視点setPosition
	}
	if (cameraSwitch)
	{
		CamPos = XMVectorAdd(CamPos, *pSu30->getPosition());//関数で書いたときの足し算
		XMVECTOR CamBias2 = XMVectorSet(0.0f, 1.5f, 0.0f, 0.0f);
		CamPos = XMVectorAdd(CamPos, CamBias2);
		//カメラの座標
		vnCamera::setPosition(&CamPos);	//FPS視点setTarget

		//カメラの注視点をプレイヤーの座標に設定
		XMVECTOR CamTrg2 = *pSu30->getPosition();
		CamTrg2 = XMVectorAdd(CamTrg2, CamBias2);
		vnCamera::setTarget(&CamTrg2); //FPS視点setPosition
	}
	
	

	//vnDebugDraw::Axis(5.0f);

	vnFont::print(20.0f, 20.0f, L"速度 : %.1f km/h", speed*1000.0f);
	vnFont::print(20.0f, 40.0f, L"高度 : %.1f m", pFA18C->getPositionY()*10.0f);


	//撃墜されたら表示
	if (isEnemyShootDown)
	{
		vnFont::print(1100.0f, 400.0f, L"Splash Su-30");
	}
	if (isPlayerShootDown)
	{
		vnFont::print(1100.0f, 420.0f, L"Splash F/A-18C");
	}

	//デバッグ情報
	if (!isDebugView)
	{
		vnFont::print(20.0f, 100.0f, L"FA18C : (X %.3f,Y %.3f,Z %.3f)",
			pFA18C->getPositionX(),
			pFA18C->getPositionY(),
			pFA18C->getPositionZ()
		);
		vnFont::print(20.0f, 120.0f, L"FA18C_RotationX : %.3f", pFA18C->getRotationX() * 180.0f / 3.141592f);
		vnFont::print(20.0f, 140.0f, L"FA18C_RotationY : %.3f", pFA18C->getRotationY() * 180.0f / 3.141592f);
		vnFont::print(20.0f, 160.0f, L"FA18C_RotationZ : %.3f", pFA18C->getRotationZ() * 180.0f / 3.141592f);

		vnFont::print(20.0f, 200.0f, L"Su30 : (X %.3f,Y %.3f,Z %.3f)",
			pSu30->getPositionX(),
			pSu30->getPositionY(),
			pSu30->getPositionZ()
		);
		vnFont::print(20.0f, 220.0f, L"Su30_RotationX : %.3f", pSu30->getRotationX() * 180.0f / 3.141592f);
		vnFont::print(20.0f, 240.0f, L"Su30_RotationY : %.3f", pSu30->getRotationY() * 180.0f / 3.141592f);
		vnFont::print(20.0f, 260.0f, L"Su30_RotationZ : %.3f", pSu30->getRotationZ() * 180.0f / 3.141592f);

		vnFont::print(20.0f, 300.0f, L"ミサイルと敵機の距離 : %.3f", dist);
		vnFont::print(20.0f, 320.0f, L"敵機の距離 : %.3f", playerToEnemyDist);
		vnFont::print(20.0f, 340.0f, L"カウント : %.3f", count);

		vnFont::print(20.0f, 500.0f, L"Radius : %.3f", radius);
		vnFont::print(20.0f, 520.0f, L"Theta : %.3f", theta * 180 / 3.141592f);
		vnFont::print(20.0f, 540.0f, L"Phi : %.3f", phi * 180 / 3.141592f);
	}

	

	

	vnScene::execute();
}

//描画関数
void SceneMainGame::render()
{
	vnScene::render();
}

