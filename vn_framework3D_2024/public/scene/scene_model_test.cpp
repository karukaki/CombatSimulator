#include "../../framework.h"
#include "../../framework/vn_environment.h"

//初期化関数
bool SceneModelTest::initialize()
{
	pModel = new vnModel(L"data/model/Maingame/", L"SM_FA18C.vnm");

	resetPos = XMVectorSet(pModel->getPositionX(), pModel->getPositionY(), pModel->getPositionZ(), 0.0f);
	resetRot = XMVectorSet(pModel->getRotationX(), pModel->getRotationY(), pModel->getRotationZ(), 0.0f);
	resetScl = XMVectorSet(pModel->getScaleX(), pModel->getScaleY(), pModel->getScaleZ(), 0.0f);

	resetCamPos = *vnCamera::getPosition();
	resetCamTrg = *vnCamera::getTarget();

	radius = 5.0f;
	theta = 0.0f;
	phi = 0.0f;

	Cursor = 0;

	return true;
}

//終了関数
void SceneModelTest::terminate()
{
	//オブジェクトの削除
	delete pModel;
}

//処理関数
void SceneModelTest::execute()
{
	float value_t = 0.01f;
	float value_r = 1.0f * 3.141592f / 180.0f;
	float value_s = 0.01f;
	
	

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
		case PositionX:	pModel->addPositionX(value_t);	break;
		case PositionY:	pModel->addPositionY(value_t);	break;
		case PositionZ:	pModel->addPositionZ(value_t);	break;
		case RotateX:	pModel->addRotationX(value_r);	break;
		case RotateY:	pModel->addRotationY(value_r);	break;
		case RotateZ:	pModel->addRotationZ(value_r);	break;
		case ScaleX:	pModel->addScaleX(value_s);		break;
		case ScaleY:	pModel->addScaleY(value_s);		break;
		case ScaleZ:	pModel->addScaleZ(value_s);		break;
		case CamPosX:	vnCamera::addPositionX(value_t); break;
		case CamPosY:	vnCamera::addPositionY(value_t); break;
		case CamPosZ:	vnCamera::addPositionZ(value_t); break;
		case CamTrgX:	vnCamera::addTargetX(value_t); break;
		case CamTrgY:	vnCamera::addTargetY(value_t); break;
		case CamTrgZ:	vnCamera::addTargetZ(value_t); break;
		case Radius:	radius+=value_t; break;
		case Theta:		theta+=value_r; break;
		case Phi:		phi+=value_r; break;
		}
	}
	else if (vnKeyboard::on(DIK_LEFT))
	{
		switch (Cursor)
		{
		case PositionX:	pModel->addPositionX(-value_t);	break;
		case PositionY:	pModel->addPositionY(-value_t);	break;
		case PositionZ:	pModel->addPositionZ(-value_t);	break;
		case RotateX:	pModel->addRotationX(-value_r);	break;
		case RotateY:	pModel->addRotationY(-value_r);	break;
		case RotateZ:	pModel->addRotationZ(-value_r);	break;
		case ScaleX:	pModel->addScaleX(-value_s);		break;
		case ScaleY:	pModel->addScaleY(-value_s);		break;
		case ScaleZ:	pModel->addScaleZ(-value_s);		break;
		case CamPosX:	vnCamera::addPositionX(-value_t); break;
		case CamPosY:	vnCamera::addPositionY(-value_t); break;
		case CamPosZ:	vnCamera::addPositionZ(-value_t); break;
		case CamTrgX:	vnCamera::addTargetX(-value_t); break;
		case CamTrgY:	vnCamera::addTargetY(-value_t); break;
		case CamTrgZ:	vnCamera::addTargetZ(value_t); break;
		case Radius:	radius -= value_t; break;
		case Theta:		theta -= value_r; break;
		case Phi:		phi -= value_r; break;
		}
	}
	else if (vnKeyboard::on(DIK_SPACE))
	{
		switch (Cursor)
		{
		case PositionX:	pModel->setPositionX(XMVectorGetX(resetPos));	break;
		case PositionY:	pModel->setPositionY(XMVectorGetY(resetPos));	break;
		case PositionZ:	pModel->setPositionZ(XMVectorGetZ(resetPos));	break;
		case RotateX:	pModel->setRotationX(XMVectorGetX(resetRot));	break;
		case RotateY:	pModel->setRotationY(XMVectorGetY(resetRot));	break;
		case RotateZ:	pModel->setRotationZ(XMVectorGetZ(resetRot));	break;
		case ScaleX:	pModel->setScaleX(XMVectorGetX(resetScl));		break;
		case ScaleY:	pModel->setScaleY(XMVectorGetY(resetScl));		break;
		case ScaleZ:	pModel->setScaleZ(XMVectorGetZ(resetScl));		break;
		case CamPosX:	vnCamera::setPositionX(XMVectorGetX(resetCamPos)); break;
		case CamPosY:	vnCamera::setPositionY(XMVectorGetY(resetCamPos)); break;
		case CamPosZ:	vnCamera::setPositionZ(XMVectorGetZ(resetCamPos)); break;
		case CamTrgX:	vnCamera::setTargetX(XMVectorGetX(resetCamTrg)); break;
		case CamTrgY:	vnCamera::setTargetY(XMVectorGetY(resetCamTrg)); break;
		case CamTrgZ:	vnCamera::setTargetZ(XMVectorGetZ(resetCamTrg)); break;
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

	//平面角θの計算
	float tx = cosf(theta) * radius;
	float tz = sinf(theta) * radius;

	//仰角Φの計算
	float px = cosf(theta) * cosf(phi) * radius;
	float py = sinf(phi) * radius;
	float pz = sinf(theta) * cosf(phi) * radius;

	//カメラの座標
	vnCamera::setPosition(px, py, pz);


	int line = 0;
	vnFont::print(80.0f, (float)(100 + Cursor * 16), L"→");
	vnFont::print(100.0f, 100.0f + (float)(line++ * 16), L"PositionX : %.3f", pModel->getPositionX());
	vnFont::print(100.0f, 100.0f + (float)(line++ * 16), L"PositionY : %.3f", pModel->getPositionY());
	vnFont::print(100.0f, 100.0f + (float)(line++ * 16), L"PositionZ : %.3f", pModel->getPositionZ());
	vnFont::print(100.0f, 100.0f + (float)(line++ * 16), L"RotationX : %.3f", pModel->getRotationX() / 3.141592f * 180.0f);
	vnFont::print(100.0f, 100.0f + (float)(line++ * 16), L"RotationY : %.3f", pModel->getRotationY() / 3.141592f * 180.0f);
	vnFont::print(100.0f, 100.0f + (float)(line++ * 16), L"RotationZ : %.3f", pModel->getRotationZ() / 3.141592f * 180.0f);
	vnFont::print(100.0f, 100.0f + (float)(line++ * 16), L"ScaleX : %.3f", pModel->getScaleX());
	vnFont::print(100.0f, 100.0f + (float)(line++ * 16), L"ScaleY : %.3f", pModel->getScaleY());
	vnFont::print(100.0f, 100.0f + (float)(line++ * 16), L"ScaleZ : %.3f", pModel->getScaleZ());
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
void SceneModelTest::render()
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
	pModel->render();

	

	//ビューポートの設定(フルスクリーンに戻す)
	//Viewport.TopLeftX = 0;
	//Viewport.Width = (float)SCREEN_WIDTH;
	//vnDirect3D::getCommandList()->RSSetViewports(1, &Viewport);

	vnScene::render();
}
