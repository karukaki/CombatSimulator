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


	//�ɍ��W�̏��
	float radius;	//���a
	float theta;	//���ʊp
	float phi;		//�p

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
