#pragma once

#include "../object/cube.h"

class SceneCubeTest : public vnScene
{
private:
	cCube* pCube;

	cCube* pTheta;
	cCube* pPhi;

	//ã…ç¿ïWÇÃèÓïÒ
	float radius;	//îºåa
	float theta;	//ïΩñ äp
	float phi;		//ã¬äp

	//ëÄçÏÇÃéÌóﬁ
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
	
	bool initialize();
	void terminate();
	
	void execute();
	void render();
};
