#pragma once

#include "../vn_Model.h"

class cF15E : public vnModel
{	
public:
	//コンストラクタ
	cF15E(const WCHAR* folder, const WCHAR* file);
	//デストラクタ
	~cF15E();
	//処理
	void execute();
	//描画
	void render();

};