#include "../../framework.h"
#include "../../framework/vn_environment.h"
#include "scene_collision_test.h"

bool SceneCollisionTest::initialize()
{
	Point[0] = XMVectorSet(-2.0f, 4.0f, -2.0f, 0.0f);
	Point[1] = XMVectorSet(2.0f, -4.0f, 2.0f, 0.0f);
	Point[2] = XMVectorSet(-1.0f, 0.0f, 1.0f, 0.0f);
	Point[3] = XMVectorSet(-1.0f, 4.0f, -1.0f, 0.0f);
	Point[4] = XMVectorSet(1.0f, 0.0f, -1.0f, 0.0f);

	pTheta = new cCube;
	pTheta->setScaleX(0.2f);
	pTheta->setScaleY(0.2f);
	pTheta->setScaleZ(0.2f);

	pPhi = new cCube();
	pPhi->setScaleX(0.2f);
	pPhi->setScaleY(0.2f);
	pPhi->setScaleZ(0.2f);


	radius = 5.0f;
	theta = 0.0f;
	phi = 0.0f;

	Cursor = 0;

	return true;
}

void SceneCollisionTest::terminate()
{
	delete pTheta;
	delete pPhi;
}

void SceneCollisionTest::execute()
{
	float value_t = 0.01f;
	float value_r = 1.0f * 3.141592f / 180.0f;
	float value_s = 0.01f;

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
	vnFont::print(100.0f, 100.0f + (float)(line++ * 16), L"CamPosX : %.3f", vnCamera::getPositionX());
	vnFont::print(100.0f, 100.0f + (float)(line++ * 16), L"CamPosY : %.3f", vnCamera::getPositionY());
	vnFont::print(100.0f, 100.0f + (float)(line++ * 16), L"CamPosZ : %.3f", vnCamera::getPositionZ());
	vnFont::print(100.0f, 100.0f + (float)(line++ * 16), L"CamTrgX : %.3f", vnCamera::getTargetX());
	vnFont::print(100.0f, 100.0f + (float)(line++ * 16), L"CamTrgY : %.3f", vnCamera::getTargetY());
	vnFont::print(100.0f, 100.0f + (float)(line++ * 16), L"CamTrgZ : %.3f", vnCamera::getTargetZ());
	vnFont::print(100.0f, 100.0f + (float)(line++ * 16), L"Radius : %.3f", radius);
	vnFont::print(100.0f, 100.0f + (float)(line++ * 16), L"Theta : %.3f", theta);
	vnFont::print(100.0f, 100.0f + (float)(line++ * 16), L"Phi : %.3f", phi);


	//画像のでバッグ描画
	vnDebugDraw::Line(&Point[0],&Point[1], 0xffffff00);		//カラーは16進数のABGRの順で入力する
	//三角形のデバッグ描画
	vnDebugDraw::Line(&Point[2], &Point[3], 0xffff00ff);
	vnDebugDraw::Line(&Point[3], &Point[4], 0xffff00ff);
	vnDebugDraw::Line(&Point[4], &Point[2], 0xffff00ff);
	vnDebugDraw::Grid(9,9);
	vnDebugDraw::Axis();

	vnScene::execute();
}

void SceneCollisionTest::render()
{
	pTheta->render();
	pPhi->render();

	vnScene::render();
}
