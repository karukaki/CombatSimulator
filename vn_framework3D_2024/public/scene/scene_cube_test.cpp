#include "../../framework.h"
#include "../../framework/vn_environment.h"

//初期化関数
bool SceneCubeTest::initialize()
{
	pCube = new cCube();

	pTheta = new cCube();
	pTheta->setScaleX(0.2f);
	pTheta->setScaleY(0.2f);
	pTheta->setScaleZ(0.2f);

	pPhi = new cCube();
	pPhi->setScaleX(0.2f);
	pPhi->setScaleY(0.2f);
	pPhi->setScaleZ(0.2f);

	resetPos = XMVectorSet(pCube->getPositionX(), pCube->getPositionY(), pCube->getPositionZ(), 0.0f);
	resetRot = XMVectorSet(pCube->getRotationX(), pCube->getRotationY(), pCube->getRotationZ(), 0.0f);
	resetScl = XMVectorSet(pCube->getScaleX(), pCube->getScaleY(), pCube->getScaleZ(), 0.0f);

	resetCamPos = *vnCamera::getPosition();
	resetCamTrg = *vnCamera::getTarget();

	radius = 5.0f;
	theta = 0.0f;
	phi = 0.0f;

	Cursor = 0;

	return true;
}

//終了関数
void SceneCubeTest::terminate()
{
	//オブジェクトの削除
	delete pCube;

	delete pTheta;
	delete pPhi;
}

//処理関数
void SceneCubeTest::execute()
{
	float value_t = 0.01f;
	float value_r = 1.0f * 3.141592f / 180.0f;
	float value_s = 0.01f;

	//マウス操作でカメラ回転
	if (vnKeyboard::on(DIK_LMENU) && vnMouse::onL() && vnMouse::getDX())
	{
		Theta:	theta += value_r;
	}
	if (vnKeyboard::on(DIK_LMENU) && vnMouse::onL() && vnMouse::getDY())
	{
		Phi:	phi += value_r;
	}

	//マウス操作でカメラ遠近
	if (vnKeyboard::on(DIK_LMENU) && vnMouse::onR() && vnMouse::getDX())
	{
		Radius:	radius += value_r;
	}
	

	if (vnKeyboard::trg(DIK_UP) && --Cursor < 0)
	{
		Cursor = OperationMax - 1;
	}
	else if (vnKeyboard::trg(DIK_DOWN) && ++Cursor >= OperationMax)
	{
		Cursor = 0;
	}
	else if (vnKeyboard::on(DIK_RIGHT))
	{
		switch (Cursor)
		{
		case PositionX:	pCube->addPositionX(value_t);	break;
		case PositionY:	pCube->addPositionY(value_t);	break;
		case PositionZ:	pCube->addPositionZ(value_t);	break;
		case RotateX:	pCube->addRotationX(value_r);	break;
		case RotateY:	pCube->addRotationY(value_r);	break;
		case RotateZ:	pCube->addRotationZ(value_r);	break;
		case ScaleX:	pCube->addScaleX(value_s);		break;
		case ScaleY:	pCube->addScaleY(value_s);		break;
		case ScaleZ:	pCube->addScaleZ(value_s);		break;
		case CamPosX:	vnCamera::addPositionX(value_t); break;
		case CamPosY:	vnCamera::addPositionY(value_t); break;
		case CamPosZ:	vnCamera::addPositionZ(value_t); break;
		case CamTrgX:	vnCamera::addTargetX(value_t); break;
		case CamTrgY:	vnCamera::addTargetY(value_t); break;
		case CamTrgZ:	vnCamera::addTargetZ(value_t); break;
		case Radius:	radius+=value_r; break;
		case Theta:		theta+=value_r; break;
		case Phi:		phi+=value_r; break;
		}
	}
	else if (vnKeyboard::on(DIK_LEFT))
	{
		switch (Cursor)
		{
		case PositionX:	pCube->addPositionX(-value_t);	break;
		case PositionY:	pCube->addPositionY(-value_t);	break;
		case PositionZ:	pCube->addPositionZ(-value_t);	break;
		case RotateX:	pCube->addRotationX(-value_r);	break;
		case RotateY:	pCube->addRotationY(-value_r);	break;
		case RotateZ:	pCube->addRotationZ(-value_r);	break;
		case ScaleX:	pCube->addScaleX(-value_s);		break;
		case ScaleY:	pCube->addScaleY(-value_s);		break;
		case ScaleZ:	pCube->addScaleZ(-value_s);		break;
		case CamPosX:	vnCamera::addPositionX(-value_t); break;
		case CamPosY:	vnCamera::addPositionY(-value_t); break;
		case CamPosZ:	vnCamera::addPositionZ(-value_t); break;
		case CamTrgX:	vnCamera::addTargetX(-value_t); break;
		case CamTrgY:	vnCamera::addTargetY(-value_t); break;
		case CamTrgZ:	vnCamera::addTargetZ(value_t); break;
		case Radius:	radius -= value_r; break;
		case Theta:		theta -= value_r; break;
		case Phi:		phi -= value_r; break;
		}
	}
	else if (vnKeyboard::on(DIK_SPACE))
	{
		switch (Cursor)
		{
		case PositionX:	pCube->setPositionX(XMVectorGetX(resetPos));	break;
		case PositionY:	pCube->setPositionY(XMVectorGetY(resetPos));	break;
		case PositionZ:	pCube->setPositionZ(XMVectorGetZ(resetPos));	break;
		case RotateX:	pCube->setRotationX(XMVectorGetX(resetRot));	break;
		case RotateY:	pCube->setRotationY(XMVectorGetY(resetRot));	break;
		case RotateZ:	pCube->setRotationZ(XMVectorGetZ(resetRot));	break;
		case ScaleX:	pCube->setScaleX(XMVectorGetX(resetScl));		break;
		case ScaleY:	pCube->setScaleY(XMVectorGetY(resetScl));		break;
		case ScaleZ:	pCube->setScaleZ(XMVectorGetZ(resetScl));		break;
		case CamPosX:	vnCamera::setPositionX(XMVectorGetX(resetCamPos)); break;
		case CamPosY:	vnCamera::setPositionY(XMVectorGetY(resetCamPos)); break;
		case CamPosZ:	vnCamera::setPositionZ(XMVectorGetZ(resetCamPos)); break;
		case CamTrgX:	vnCamera::setTargetX(XMVectorGetX(resetCamTrg)); break;
		case CamTrgY:	vnCamera::setTargetY(XMVectorGetY(resetCamTrg)); break;
		case CamTrgZ:	vnCamera::setTargetZ(XMVectorGetZ(resetCamTrg)); break;
		case Radius:	pTheta->setPositionX(XMVectorGetX(resetRadius)); break;
		case Theta:		pTheta->setPositionY(XMVectorGetX(resetTheta)); break;
		case Phi:		pTheta->setPositionZ(XMVectorGetX(resetPhi)); break;
		}
	}

	//マウス操作でカメラ遠近
	if (vnKeyboard::on(DIK_LMENU) && vnMouse::onR())
	{
		radius += vnMouse::getDY() / 100.0f;
	}
	//マウス操作でカメラ回転
	if (vnKeyboard::on(DIK_LMENU) && vnMouse::onL())
	{
		theta += vnMouse::getDX() / 100.0f;
		phi += vnMouse::getDY() / 100.0f;
	}

	/*
	マウスによるカメラ操作

	Alt + 左ドラッグ：カメラ回転
	Alt + 右ドラッグ：カメラ遠近

	vnMouse::getX(),getY()
	getDX(),getDY
	*/

	//平面角θの計算
	float tx = cosf(theta) * radius;
	float tz = sinf(theta) * radius;
	pTheta->setPositionX(tx);
	pTheta->setPositionZ(tz);

	//仰角Φの計算
	float px = cosf(theta) * cosf(phi) * radius;
	float py = sinf(phi) * radius;
	float pz = sinf(theta) * cosf(phi) * radius;
	pPhi->setPositionX(px);
	pPhi->setPositionY(py);
	pPhi->setPositionZ(pz);

	//カメラの座標
	vnCamera::setPosition(px, py, pz);


	int line = 0;
	vnFont::print(80.0f, (float)(100 + Cursor * 16), L"→");
	vnFont::print(100.0f, 100.0f + (float)(line++ * 16), L"PositionX : %.3f", pCube->getPositionX());
	vnFont::print(100.0f, 100.0f + (float)(line++ * 16), L"PositionY : %.3f", pCube->getPositionY());
	vnFont::print(100.0f, 100.0f + (float)(line++ * 16), L"PositionZ : %.3f", pCube->getPositionZ());
	vnFont::print(100.0f, 100.0f + (float)(line++ * 16), L"RotationX : %.3f", pCube->getRotationX() / 3.141592f * 180.0f);
	vnFont::print(100.0f, 100.0f + (float)(line++ * 16), L"RotationY : %.3f", pCube->getRotationY() / 3.141592f * 180.0f);
	vnFont::print(100.0f, 100.0f + (float)(line++ * 16), L"RotationZ : %.3f", pCube->getRotationZ() / 3.141592f * 180.0f);
	vnFont::print(100.0f, 100.0f + (float)(line++ * 16), L"ScaleX : %.3f", pCube->getScaleX());
	vnFont::print(100.0f, 100.0f + (float)(line++ * 16), L"ScaleY : %.3f", pCube->getScaleY());
	vnFont::print(100.0f, 100.0f + (float)(line++ * 16), L"ScaleZ : %.3f", pCube->getScaleZ());
	vnFont::print(100.0f, 100.0f + (float)(line++ * 16), L"CamPosX : %.3f", vnCamera::getPositionX());
	vnFont::print(100.0f, 100.0f + (float)(line++ * 16), L"CamPosY : %.3f", vnCamera::getPositionY());
	vnFont::print(100.0f, 100.0f + (float)(line++ * 16), L"CamPosZ : %.3f", vnCamera::getPositionZ());
	vnFont::print(100.0f, 100.0f + (float)(line++ * 16), L"CamTrgX : %.3f", vnCamera::getTargetX());
	vnFont::print(100.0f, 100.0f + (float)(line++ * 16), L"CamTrgY : %.3f", vnCamera::getTargetY());
	vnFont::print(100.0f, 100.0f + (float)(line++ * 16), L"CamTrgZ : %.3f", vnCamera::getTargetZ());
	vnFont::print(100.0f, 100.0f + (float)(line++ * 16), L"Radius : %.3f", radius);
	vnFont::print(100.0f, 100.0f + (float)(line++ * 16), L"Theta : %.3f",  theta);
	vnFont::print(100.0f, 100.0f + (float)(line++ * 16), L"Phi : %.3f", phi);

	vnDebugDraw::Grid();
	vnDebugDraw::Axis();

	vnScene::execute();
}

//描画関数
void SceneCubeTest::render()
{
	//ビューポートの設定(画面中央正方形)
	//D3D12_VIEWPORT	Viewport;
	//Viewport.TopLeftX = (SCREEN_WIDTH - SCREEN_HEIGHT) / 2;
	//Viewport.TopLeftY = 0;
	//Viewport.Width = (float)SCREEN_HEIGHT;
	//Viewport.Height = (float)SCREEN_HEIGHT;
	//Viewport.MinDepth = 0.0f;
	//Viewport.MaxDepth = 1.0f;
	//vnDirect3D::getCommandList()->RSSetViewports(1, &Viewport);

	//キューブの描画
	pCube->render();

	pTheta->render();
	pPhi->render();

	//ビューポートの設定(フルスクリーンに戻す)
	//Viewport.TopLeftX = 0;
	//Viewport.Width = (float)SCREEN_WIDTH;
	//vnDirect3D::getCommandList()->RSSetViewports(1, &Viewport);

	vnScene::render();
}
