#pragma once

#include "../vn_Model.h"

class cF15E : public vnModel
{	
public:
	//�R���X�g���N�^
	cF15E(const WCHAR* folder, const WCHAR* file);
	//�f�X�g���N�^
	~cF15E();
	//����
	void execute();
	//�`��
	void render();

};