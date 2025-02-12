#pragma once

#include "../object/cube.h"

class SceneCollisionTest : public vnScene
{
private:
	cCube* pCube;

	cCube* pTheta;
	cCube* pPhi;


	XMVECTOR Point[5];

	//�ɍ��W�̏��
	float radius;	//���a
	float theta;	//���ʊp
	float phi;		//�p

	//����̎��
	enum eOperation
	{
		PositionX,
		PositionY,
		PositionZ,
		RotateX,
		RotateY,
		RotateZ,
		ScaleX,
		ScaleY,
		ScaleZ,
		CamPosX,
		CamPosY,
		CamPosZ,
		CamTrgX,
		CamTrgY,
		CamTrgZ,

		Radius,
		Theta,
		Phi,
		OperationMax,
	};

	XMVECTOR resetPos;
	XMVECTOR resetRot;
	XMVECTOR resetScl;

	XMVECTOR resetCamPos;
	XMVECTOR resetCamTrg;

	XMVECTOR resetRadius;
	XMVECTOR resetTheta;
	XMVECTOR resetPhi;

	int	Cursor;

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
