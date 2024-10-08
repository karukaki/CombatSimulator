#pragma once

#define MISSILE_MAX (4)

class SceneTitle : public vnScene
{
private:
	vnModel* pSky;

	vnModel* pFA18C;
	vnModel* pCVN76;
	vnModel* pAIM120[MISSILE_MAX];

	vnSprite* titleLogo;


	//‹ÉÀ•W‚Ìî•ñ
	float radius;	//”¼Œa
	float theta;	//•½–ÊŠp
	float phi;		//‹ÂŠp

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
