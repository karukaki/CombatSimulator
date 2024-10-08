#include "../../framework.h"
#include "../../framework/vn_environment.h"
#include "scene_main_game.h"

//�������֐�
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
	pShadow->setTransparent(true);	//�������`��
	//Z Fighting����̂��ߏ�����ɂ�����
	pShadow->setPositionY(0.01f);

	pEnemyShadow = new vnModel(MODEL_FOLDER, L"shadow.vnm");
	pEnemyShadow->setTransparent(true);
	pEnemyShadow->setPositionY(0.01f);

	pGround = new vnModel(MODEL_FOLDER, L"ground.vnm");
	pSky = new vnModel(MODEL_FOLDER, L"skydome.vnm");
	pSky->setLighting(false);	//���C�e�B���O����
	pSky->setScale(10.0f, 10.0f, 10.0f);

	pCVN76 = new vnModel(MAINGAME_FOLDER, CVN76_FAIL);
	pCVN76->setPosition(0.0f, -20.0f, -50.0f);
	pCVN76->setLighting(false);
	pFA18C = new vnModel(MAINGAME_FOLDER, FA18C_FLIGHT_FAIL);
	pFA18C->setPosition(0.0f,30.0f, 0.0f);
	pSu30 = new vnModel(MAINGAME_FOLDER, Su30_FLIGHT_FAIL);
	pSu30->setPosition(0.0f, 50.0f, 1000.0f);
	pSu30->setRotationY(rad180);

	pF15E = new cF15E(MAINGAME_FOLDER, F15E_FAIL);	//F15E���\������Ȃ�
	pF15E->setPosition(0.0f, 50.0f, 500.0f);
	pF15E->setRotationY(rad180);

	for (int i = 0; i < MISSILE_MAX; i++) {
		pAIM120[i] = new vnModel(MAINGAME_FOLDER, AIM120_FAIL);
		pR77[i] = new vnModel(MAINGAME_FOLDER, R77_FAIL);
	}
	

	//pRadar = new vnSprite(200.0f, 400.0f, 262.0f, 386.0f, L"data/image/���[�_�[�x����M�@.png");

	//SE
	seFrying = new vnSound(L"data/sound/�퓬�@���i��s���j.wav");
	seFrying->setVolume(0.5f);
	seMissile = new vnSound(L"data/sound/�~�T�C������2.wav");
	seMissileWarning = new vnSound(L"data/sound/�~�T�C���ڋ߃A���[��.wav");
	seMissileWarning->setVolume(0.7f);
	seExplosion = new vnSound(L"data/sound/����.wav");
	isSEplay = true;
	isSEplay2 = true;

	//�V�[���̊��N���X�֓o�^
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

	//�������̃I�u�W�F�N�g�͕s�����̃I�u�W�F�N�g�̌�ɕ`�悷��K�v������
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

	radius = 20.0f;	//���a
	theta = 0.0f; //-90.0f/180*3.141592f;	//���ʊp
	phi = 10.0f / 180 * 3.141592f;		//�p

	count = 0.0f;

	

	return true;
}

//�I���֐�
void SceneMainGame::terminate()
{
	//���N���X�̓o�^����폜

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

//�����֐�
void SceneMainGame::execute()
{
	//���͂����邩�����t���O
	bool input = false;

	//�ړ��X�s�[�h
	float speed = 0.5f;
	float enemySpeed = 0.4f;
	float speedThrottle = 0.1f;
	

	XMVECTOR front=XMVectorSet(0.0f,0.0f,speed,0.0f);
	XMVECTOR missileFront=XMVectorSet(0.0f,0.0f,1.0f,0.0f);
	//��]�s��Ɠ����悤�Ɏg����
	XMMATRIX mRot = XMMatrixRotationRollPitchYaw(pFA18C->getRotationX(), pFA18C->getRotationY(), pFA18C->getRotationZ());
	XMVECTOR q = XMQuaternionRotationMatrix(mRot);	//��]�s�񂩂�ϊ�
	XMMATRIX m = XMMatrixRotationQuaternion(q);		//��]�s��֕ϊ�
	front = XMVector3TransformNormal(front, mRot);
	missileFront = XMVector3TransformNormal(missileFront, m);
	pFA18C->addPosition(&front);	//��]���������Ɉړ�����
	pSky->addPosition(&front);	//�w�i���v���C���[�̓����ɒǏ]�����

	XMVECTOR enemyFront=XMVectorSet(0.0f,0.0f,enemySpeed,0.0f);
	XMMATRIX enemyRot = XMMatrixRotationRollPitchYaw(pSu30->getRotationX(), pSu30->getRotationY(), pSu30->getRotationZ());
	enemyFront = XMVector3TransformNormal(enemyFront, enemyRot);
	pSu30->addPosition(&enemyFront);


	//�G�̌��_����̋����𒲂ׂ�
	XMVECTOR vDist = XMVector3Length(*pSu30->getPosition());
	float dist = XMVectorGetX(vDist);
	//�v���C���[�̌��_����̋����𒲂ׂ�
	XMVECTOR player_vDist = XMVector3Length(*pFA18C->getPosition());
	float playerDist = XMVectorGetX(player_vDist);
	

	//�~�T�C���̔F���͈�
	//�@�G�@�Ƃ̋����������ȓ�
	XMVECTOR MissileToEnemy;	//�G�@����~�T�C�����������ʒu
	XMMATRIX MissileWorld;		//�~�T�C���̃��[���h�}�g���N�X
	XMVECTOR MissileFront;		//�~�T�C���̑O�����x�N�g��
	XMVECTOR vDot_x;
	float dot_x;
	float radian;
	float degree;

	XMVECTOR R77ToPlayer;	//�v���C���[����~�T�C�����������ʒu
	XMMATRIX R77World;		//�~�T�C���̃��[���h�}�g���N�X
	XMVECTOR R77Front;		//�~�T�C���̑O�����x�N�g��
	XMVECTOR e_vDot_x;
	float e_dot_x;
	float e_radian;
	float e_degree;

	
	//��s��SE���[�v�Đ�
	if (!isPlayerShootDown&&isSEplay)
	{
		seFrying->play();
		isSEplay = false;
	}
	if (seFrying->isStopped())
	{
		isSEplay = true;
	}

	//�ړ��x�N�g��
	XMVECTOR vMove = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	//�ړ�
	//�@�̂̃X���b�g��
	/*if (vnKeyboard::on(DIK_R))
	{
		speedThrottle = 0.1f;
	}
	if (vnKeyboard::on(DIK_F))
	{
		speedThrottle = -0.1f;
	}*/
	//�f�o�b�O���\��
	if (vnKeyboard::trg(DIK_P))
	{
		isDebugView = true;
	}
	//�J������G�@�ɐ؂�ւ�
	if (vnKeyboard::trg(DIK_R))
	{
		cameraSwitch = true;
	}
	if (vnKeyboard::trg(DIK_F))
	{
		cameraSwitch = false;
	}
	//�s�b�`
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
	//���[
	if (vnKeyboard::on(DIK_E))
	{
		pFA18C->addRotationY(0.01f);
	}
	if (vnKeyboard::on(DIK_Q))
	{
		pFA18C->addRotationY(-0.01f);
	}
	//���[��
	if (vnKeyboard::on(DIK_A))
	{
		pFA18C->addRotationZ(0.01f);
	}
	if (vnKeyboard::on(DIK_D))
	{
		pFA18C->addRotationZ(-0.01f);
	}
	//�~�T�C������
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
	
	//�G�@(Su30)�̈ړ�����
	//Z���W-500�����������
	if (pSu30->getPositionZ() < -1100.0f)
	{
		pSu30->addRotationY(0.01f);
	}
	//Z���W500�𒴂�����
	if (pSu30->getPositionZ() > 1100.0f)
	{
		pSu30->addRotationY(0.01f);
	}


	//�~�T�C���������Ă��Ȃ��Ƃ��@�̂ƈꏏ�ɓ���
	//�~�T�C�����ꔭ������
	for (int i = 0; i < MISSILE_MAX; ++i) 
	{
		if (!isFired[i]) {
			//�~�T�C���̈ʒu
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
		//�~�T�C�������˂��ꂽ�Ƃ�
		if (isFired[i]) {
			//�~�T�C���̋���
			//pAIM120[i]->addPositionZ(1.0f);

			//�~�T�C���̔F���͈�
			//�@�G�@�Ƃ̋����������ȓ�
			MissileToEnemy = *pSu30->getPosition() - *pAIM120[i]->getPosition();

			//XMVectorSubtract(*pPlayer->getPosition(), *pNPC->getPosition());
			vDist = XMVector3Length(MissileToEnemy);
			dist = XMVectorGetX(vDist);

			//�A�G�@�ւ̊p�x���O���}45���ȓ�
			//1.�~�T�C���̑O���x�N�g��

			MissileWorld = *pAIM120[i]->getWorld();	//���[���h�}�g���N�X���擾

			MissileFront = MissileWorld.r[2];	//���[���h�}�g���N�X��3�s�ڂ��O�����x�N�g��
			//2.�G�@�֌������x�N�g��
			//MissileToEnemy

			//���K��(���ϒl��cos�ƂɂȂ�悤��)
			MissileFront = XMVector3Normalize(MissileFront);
			MissileToEnemy = XMVector3Normalize(MissileToEnemy);

			//����
			vDot_x = XMVector3Dot(MissileFront, MissileToEnemy);
			dot_x = XMVectorGetX(vDot_x);	//cos��
			if (dot_x > 1.0f)dot_x = 1.0f;	//�G���[�`�F�b�N(float�̃m�C�Y�J�b�g)
			radian = acosf(dot_x);
			degree = XMConvertToDegrees(radian);


			//�~�T�C���̔F���͈�
			if (dist < 300.0f && degree < 45.0f /*&& dist > -300.0f && degree < 45.0f*/)
			{	//�F���͈͂ɓ����Ă���

				//�i�s����(�G�@�̕���)�Ɍ�����
				float x = XMVectorGetX(MissileToEnemy);
				float y = XMVectorGetY(MissileToEnemy);
				float z = XMVectorGetZ(MissileToEnemy);

				//Y�����̉�]�i������]�j
				float rotY = atan2f(x, z);
				// X�����̉�]�i�s�b�`�p�j
				float dist_xz = sqrtf(x * x + z * z);
				float rotX = atan2f(y, dist_xz);
				// �~�T�C���̌�����ݒ�
				pAIM120[i]->setRotationY(rotY);
				pAIM120[i]->setPositionX(rotX);

				//�G�@�Ɍ����Ĉړ�������(�~�T�C���̈ړ����x)
				MissileToEnemy *= 1.5f;
				pAIM120[i]->addPosition(&MissileToEnemy);

				//vnFont::print(20.0f, 360.0f, L"�~�T�C���ƓG�@�̊p�x : %.3f", degree);
			}
			//�~�T�C���̔F���͈͊O
			else
			{
				pAIM120[i]->addPosition(&missileFront);
				//pAIM120[i]->setRotation(pFA18C->getRotation());
			}

			//�~�T�C�����G�@�ɖ��������Ƃ�
			if (dist < 2.0f) {
				if (!isPlayerShootDown){
					isEnemyShootDown = true;
					isSEplay2 = false;
					seExplosion->play();
				}
				deleteObject(pAIM120[i]);
			}
		}

		//�G�@�̃~�T�C������
		//�~�T�C���̔F���͈�
		//�@�G�@�Ƃ̋����������ȓ�
		R77ToPlayer = *pFA18C->getPosition() - *pR77[i]->getPosition();

		//XMVectorSubtract(*pPlayer->getPosition(), *pNPC->getPosition());
		player_vDist = XMVector3Length(R77ToPlayer);
		playerDist = XMVectorGetX(player_vDist);

		//�A�G�@�ւ̊p�x���O���}45���ȓ�
		//1.�~�T�C���̑O���x�N�g��

		R77World = *pR77[i]->getWorld();	//���[���h�}�g���N�X���擾

		R77Front = R77World.r[2];	//���[���h�}�g���N�X��3�s�ڂ��O�����x�N�g��
		//2.�G�@�֌������x�N�g��
		//MissileToEnemy

		//���K��(���ϒl��cos�ƂɂȂ�悤��)
		R77Front = XMVector3Normalize(R77Front);
		R77ToPlayer = XMVector3Normalize(R77ToPlayer);

		//����
		e_vDot_x = XMVector3Dot(R77Front, R77ToPlayer);
		e_dot_x = XMVectorGetX(e_vDot_x);	//cos��
		if (e_dot_x > 1.0f)e_dot_x = 1.0f;	//�G���[�`�F�b�N(float�̃m�C�Y�J�b�g)
		e_radian = acosf(e_dot_x);
		e_degree = XMConvertToDegrees(e_radian);

		//�~�T�C���̔F���͈�
		if (playerDist < 300.0f && e_degree < 45.0f && playerDist > -300.0f && e_degree < 45.0f)
		{	//�F���͈͂ɓ����Ă���

			//vnFont::print(20.0f, 400.0f, L"�v���C���[�̋��� : %.3f", playerDist);

			//�i�s����(�G�@�̕���)�Ɍ�����
			float e_x = XMVectorGetX(R77ToPlayer);
			float e_y = XMVectorGetY(R77ToPlayer);
			float e_z = XMVectorGetZ(R77ToPlayer);

			//Y�����̉�]�i������]�j
			float e_rotY = atan2f(e_x, e_z);
			// X�����̉�]�i�s�b�`�p�j
			float e_dist_xz = sqrtf(e_x * e_x + e_z * e_z);
			float e_rotX = atan2f(e_y, e_dist_xz);
			// �~�T�C���̌�����ݒ�
			pR77[i]->setRotationY(e_rotY);
			pR77[i]->setPositionX(e_rotX);

			//�G�@�Ɍ����Ĉړ�������(�~�T�C���̈ړ����x)
			R77ToPlayer *= 1.0f;
			pR77[i]->addPosition(&R77ToPlayer);

			//�~�T�C���x��SE
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
		//�G�@�̃~�T�C���������Ă��Ȃ��Ƃ�
		else {
			//�~�T�C���̈ʒu
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

		//�~�T�C����player�ɖ��������Ƃ�
		if (playerDist < 2.0f) {
			if (!isEnemyShootDown) {
				isPlayerShootDown = true;
				isSEplay2 = false;
				seExplosion->play();
			}
			deleteObject(pR77[i]);
		}



	}
	//�G�����Ă����Ƃ�
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

	



	//�O�ɐi�ݑ�����
	//pFA18C->addPositionZ(speedThrottle);
	//vout = XMVectorScale(vout, speedThrottle);


	//�ړ��x�N�g���ɃX�s�[�h��K�p(������ς���)
	//vMove = XMVectorScale(vMove, speed);
	//vMove = vMove*speed;
	//vMove *= speed;
	

	//360�x�𒴂����0�x�ɖ߂�
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


	//�����x�N�g�����v�Z
	XMVECTOR cam_pos = *vnCamera::getPosition();
	XMVECTOR cam_trg = *vnCamera::getTarget();
	XMVECTOR eye = XMVectorSubtract(cam_trg, cam_pos);
	//XMVECTOR eye = cam_trg - cam_pos;

	//Y������̉�]�l���v�Z
	float cam_rotY = atan2f(XMVectorGetX(eye), XMVectorGetZ(eye));

	//�J������Y������(theta)�̉�]�l
	XMMATRIX mRotY = XMMatrixRotationY(cam_rotY/*theta*/);
	//�ړ��x�N�g������]������
	vMove = XMVector3TransformNormal(vMove, mRotY);

	//�ړ������Ƃ��̂ݏ���
	if (input == true)
	{
		//�i�s����(�����x�N�g��:vMove.x,)
		float rotY = atan2f(XMVectorGetX(vMove), XMVectorGetZ(vMove));
		pFA18C->setRotationY(rotY);
	}


	//�ړ��x�N�g�����v���C���[�̍��W�ɉ��Z
	pFA18C->addPosition(&vMove);

	pShadow->setPosition(pFA18C->getPosition());	//�e��Ǐ]������
	pShadow->setPositionY(0.01f);

	pEnemyShadow->setPosition(pSu30->getPosition());
	pEnemyShadow->setPositionY(0.01f);

	//�G�@�Ƃ̋���
	float playerToEnemyDist = playerDist - dist;

	//�J�����̉�]
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

	//�}�E�X����ŃJ��������
	if (vnMouse::onR())
	{
		radius += vnMouse::getDY() / 100.0f;
	}
	//�}�E�X����ŃJ������]
	if (vnMouse::onL())
	{
		theta += vnMouse::getDX() / 180.0f;
		phi += vnMouse::getDY() / 180.0f;
	}


	//�J�������W�̌v�Z
	//��]���Ă��Ȃ���ƂȂ鎲�̃x�N�g��
	XMVECTOR v = XMVectorSet(0.0f, 0.0f, -radius, 0.0f);
	//��]�}�g���N�X���쐬
	XMMATRIX rotate = XMMatrixRotationRollPitchYaw(phi, theta, 0.0f); //x,y,z���ꂼ�ꉽ�x�񂷂�

	//���(v)�̃x�N�g������]������(��]�}�g���N�X����Z����)
	v = XMVector3TransformNormal(v, rotate);


	XMVECTOR CamPos = v; //XMVectorSet(px, py, pz, 0.0f);

	if (!cameraSwitch)
	{
		//�J�����̍��W�Ƀv���C���[�̍��W�����Z
		CamPos = XMVectorAdd(CamPos, *pFA18C->getPosition());//�֐��ŏ������Ƃ��̑����Z
		//CamPos = CamPos + (*pFA18C->getPosition());
		//CamPos += (*pFA18C->getPosition());

		//����������ɏグ��
		XMVECTOR CamBias = XMVectorSet(0.0f, 1.5f, 0.0f, 0.0f);
		CamPos = XMVectorAdd(CamPos, CamBias);

		//�J�����̍��W
		vnCamera::setPosition(&CamPos);	//FPS���_setTarget

		//�J�����̒����_���v���C���[�̍��W�ɐݒ�
		XMVECTOR CamTrg = *pFA18C->getPosition();
		CamTrg = XMVectorAdd(CamTrg, CamBias);
		vnCamera::setTarget(&CamTrg); //FPS���_setPosition
	}
	if (cameraSwitch)
	{
		CamPos = XMVectorAdd(CamPos, *pSu30->getPosition());//�֐��ŏ������Ƃ��̑����Z
		XMVECTOR CamBias2 = XMVectorSet(0.0f, 1.5f, 0.0f, 0.0f);
		CamPos = XMVectorAdd(CamPos, CamBias2);
		//�J�����̍��W
		vnCamera::setPosition(&CamPos);	//FPS���_setTarget

		//�J�����̒����_���v���C���[�̍��W�ɐݒ�
		XMVECTOR CamTrg2 = *pSu30->getPosition();
		CamTrg2 = XMVectorAdd(CamTrg2, CamBias2);
		vnCamera::setTarget(&CamTrg2); //FPS���_setPosition
	}
	
	

	//vnDebugDraw::Axis(5.0f);

	vnFont::print(20.0f, 20.0f, L"���x : %.1f km/h", speed*1000.0f);
	vnFont::print(20.0f, 40.0f, L"���x : %.1f m", pFA18C->getPositionY()*10.0f);


	//���Ă��ꂽ��\��
	if (isEnemyShootDown)
	{
		vnFont::print(1100.0f, 400.0f, L"Splash Su-30");
	}
	if (isPlayerShootDown)
	{
		vnFont::print(1100.0f, 420.0f, L"Splash F/A-18C");
	}

	//�f�o�b�O���
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

		vnFont::print(20.0f, 300.0f, L"�~�T�C���ƓG�@�̋��� : %.3f", dist);
		vnFont::print(20.0f, 320.0f, L"�G�@�̋��� : %.3f", playerToEnemyDist);
		vnFont::print(20.0f, 340.0f, L"�J�E���g : %.3f", count);

		vnFont::print(20.0f, 500.0f, L"Radius : %.3f", radius);
		vnFont::print(20.0f, 520.0f, L"Theta : %.3f", theta * 180 / 3.141592f);
		vnFont::print(20.0f, 540.0f, L"Phi : %.3f", phi * 180 / 3.141592f);
	}

	

	

	vnScene::execute();
}

//�`��֐�
void SceneMainGame::render()
{
	vnScene::render();
}

