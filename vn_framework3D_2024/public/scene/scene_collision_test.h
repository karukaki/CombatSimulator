#pragma once

#include "../object/cube.h"

class SceneCollisionTest : public vnScene
{
private:
	cCube* pCube;

	cCube* pTheta;
	cCube* pPhi;


	XMVECTOR Point[5];

	//‹ÉÀ•W‚Ìî•ñ
	float radius;	//”¼Œa
	float theta;	//•½–ÊŠp
	float phi;		//‹ÂŠp

	//‘€ì‚Ìí—Ş
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
	//‰Šú‰»
	bool initialize();
	//I—¹
	void terminate();

	//ˆ—
	void execute();
	//•`‰æ
	void render();
};
