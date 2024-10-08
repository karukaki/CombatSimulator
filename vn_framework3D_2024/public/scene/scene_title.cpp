#include "../../framework.h"
#include "../../framework/vn_environment.h"


#include "scene_title.h"

bool SceneTitle::initialize()
{
	pSky = new vnModel(L"data/model/", L"skydome.vnm");
	pSky->setLighting(false);
	pSky->setScale(5.0f, 5.0f, 5.0f);

	pFA18C = new vnModel(L"data/model/Maingame/", L"SM_FA18C.vnm");
	pFA18C->setLighting(false);
	pFA18C->setPosition(0.0f, 18.5f, 0.0f);
	pCVN76 = new vnModel(L"data/model/Maingame/", L"SM_CVN76.vnm");
	pCVN76->setLighting(false);
	for (int i = 0; i < MISSILE_MAX; ++i) {
		pAIM120[i] = new vnModel(L"data/model/Maingame/", L"SM_AIM120.vnm");
	}

	titleLogo = new vnSprite(640.0f, 100.0f, 540.0f, 156.0f, L"data/image/TitleLogo2.png");

	pAIM120[0]->setPosition(2.0f, 20.0f, -2.0f);
	pAIM120[1]->setPosition(3.0f, 20.0f, -2.0f);
	pAIM120[2]->setPosition(-2.0f, 20.0f, -2.0f);
	pAIM120[3]->setPosition(-3.0f, 20.0f, -2.0f);

	registerObject(pSky);
	registerObject(pFA18C);
	registerObject(pCVN76);
	for (int i = 0; i < MISSILE_MAX; ++i) {
		registerObject(pAIM120[i]);
	}
	registerObject(titleLogo);

	
	radius = 25.0f;	//���a
	theta = 140.0f /180 * 3.141592f;	//���ʊp
	phi = 13.0f / 180 * 3.141592f;	//�p

	return true;
}

void SceneTitle::terminate()
{
	deleteObject(pSky);
	deleteObject(pFA18C);
	deleteObject(pCVN76);
	for (int i = 0; i < MISSILE_MAX; ++i) {
		deleteObject(pAIM120[i]);
	}
	deleteObject(titleLogo);
}

void SceneTitle::execute()
{
	//�����x�N�g�����v�Z
	XMVECTOR cam_pos = *vnCamera::getPosition();
	XMVECTOR cam_trg = *vnCamera::getTarget();
	XMVECTOR eye = XMVectorSubtract(cam_trg, cam_pos);

	//Y������̉�]�l���v�Z
	float cam_rotY = atan2f(XMVectorGetX(eye), XMVectorGetZ(eye));

	//�J������Y������(theta)�̉�]�l
	XMMATRIX mRotY = XMMatrixRotationY(cam_rotY/*theta*/);
	
	float thetaValue = 0.1f * 3.141592f / 180;

	//�}�E�X����ŃJ��������
	if (vnKeyboard::on(DIK_LMENU) && vnMouse::onR())
	{
		radius += vnMouse::getDY() / 100.0f;
	}
	//�}�E�X����ŃJ������]
	if (vnKeyboard::on(DIK_LMENU) && vnMouse::onL())
	{
		theta += vnMouse::getDX() / 100.0f;
		phi += vnMouse::getDY() / 100.0f;
	}

	//�J�����̉�]
	if (vnKeyboard::on(DIK_W))
	{
		phi -= 1.0f * 3.141592f / 180;
	}
	if (vnKeyboard::on(DIK_S))
	{
		phi += 1.0f * 3.141592f / 180;
	}
	if (vnKeyboard::on(DIK_A))
	{
		theta -= 1.0f * 3.141592f / 180;
	}
	if (vnKeyboard::on(DIK_D))
	{
		theta += 1.0f * 3.141592f / 180;
	}
	else
	{
		theta += thetaValue;
	}
	if (vnKeyboard::on(DIK_Q))
	{
		radius -= 1.0f;
	}
	if (vnKeyboard::on(DIK_E))
	{
		radius += 1.0f;
	}
	if (vnKeyboard::on(DIK_UP))
	{
		pFA18C->addPositionY(0.1f);
	}
	if (vnKeyboard::on(DIK_DOWN))
	{
		pFA18C->addPositionY(-0.1f);
	}



	//�J�������W�̌v�Z
	//��]���Ă��Ȃ���ƂȂ鎲�̃x�N�g��
	XMVECTOR v = XMVectorSet(0.0f, 0.0f, -radius, 0.0f);
	//��]�}�g���N�X���쐬
	XMMATRIX rotate = XMMatrixRotationRollPitchYaw(phi, theta, 0.0f); //x,y,z���ꂼ�ꉽ�x�񂷂�

	//���(v)�̃x�N�g������]������(��]�}�g���N�X����Z����)
	v = XMVector3TransformNormal(v, rotate);


	XMVECTOR CamPos = v; //XMVectorSet(px, py, pz, 0.0f);

	//�J�����̍��W�Ƀv���C���[�̍��W�����Z
	CamPos = XMVectorAdd(CamPos, *pFA18C->getPosition());//�֐��ŏ������Ƃ��̑����Z
	//CamPos = CamPos + (*pCVN76->getPosition());
	//CamPos += (*pCVN76->getPosition());

	//����������ɏグ��
	XMVECTOR CamBias = XMVectorSet(0.0f, 1.5f, 0.0f, 0.0f);
	CamPos = XMVectorAdd(CamPos, CamBias);

	//�J�����̍��W
	vnCamera::setPosition(&CamPos);	//FPS���_setTarget

	//�J�����̒����_���v���C���[�̍��W�ɐݒ�
	XMVECTOR CamTrg = *pFA18C->getPosition();
	CamTrg = XMVectorAdd(CamTrg, CamBias);
	vnCamera::setTarget(&CamTrg); //FPS���_setPosition


	////���ʊp�Ƃ̌v�Z
	//float tx = cosf(theta) * radius;
	//float tz = sinf(theta) * radius;

	////�p���̌v�Z
	//float px = cosf(theta) * cosf(phi) * radius;
	//float py = sinf(phi) * radius;
	//float pz = sinf(theta) * cosf(phi) * radius;

	////�J�����̍��W
	//vnCamera::setPosition(px, py, pz);

	//�V�[���I��
	if (vnMouse::trg())
	{
		if (vnMouse::getY() > 500 && vnMouse::getY() < 520)
		{
			if (vnMouse::getX() > 640 && vnMouse::getX() < 690)
			{
				switchScene(eSceneTable::MainGame);
			}
		}
		if (vnMouse::getY() > 540 && vnMouse::getY() < 560)
		{
			if (vnMouse::getX() > 640 && vnMouse::getX() < 700)
			{
				switchScene(eSceneTable::Boot);
			}
		}

	}


	/*vnFont::print(20.0f, 20.0f, L"Player : (X %.3f,Y %.3f,Z %.3f)",
		pFA18C->getPositionX(),
		pFA18C->getPositionY(),
		pFA18C->getPositionZ()
	);
	vnFont::print(20.0f, 40.0f, L"Player : (X %.3f,Y %.3f,Z %.3f)",
		pCVN76->getPositionX(),
		pCVN76->getPositionY(),
		pCVN76->getPositionZ()
	);
	vnFont::print(20.0f, 60.0f, L"Radius : %.3f", radius);
	vnFont::print(20.0f, 80.0f, L"Theta : %.3f", theta * 180 / 3.141592f);
	vnFont::print(20.0f, 100.0f, L"Phi : %.3f", phi * 180 / 3.141592f);*/
	vnFont::print(640.0f, 500.0f, L"START");
	vnFont::print(640.0f, 540.0f, L"OPTION");

	vnScene::execute();
}

void SceneTitle::render()
{
	vnScene::render();
}
