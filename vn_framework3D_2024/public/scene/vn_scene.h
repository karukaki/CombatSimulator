//--------------------------------------------------------------//
//	"vn_scene.h"												//
//		シーンクラス											//
//													2024/04/01	//
//														Ichii	//
//--------------------------------------------------------------//
#pragma once

#define vnOBJECT2D_MAX (512)	//2Dオブジェクトを登録できる最大数
#define vnOBJECT3D_MAX (512)	//3Dオブジェクトを登録できる最大数

class vnScene
{
private:
	vnSprite *pObject2D_Array[vnOBJECT2D_MAX];	//2Dオブジェクトを登録しておく配列
	vnObject *pObject3D_Array[vnOBJECT3D_MAX];	//3Dオブジェクトを登録しておく配列

	//2Dオブジェクトをソートする必要があるか
	static bool UpdateRenderPriority;

public:
	vnScene();
	virtual ~vnScene();

	virtual bool initialize() = 0;
	virtual void terminate() = 0;
	
	virtual void execute();
	virtual void render();

	//登録された2Dオブジェクトの描画優先順位が変更されたので、配列をソートをする必要がある
	static void setUpdateRenderPriority();

	//オブジェクトの登録(配列の空きに入れる)
	bool registerObject(vnSprite *p);

	//オブジェクトの破棄(配列から削除/オブジェクト自体をdeleteする)
	void deleteObject(vnSprite *p);

	//オブジェクトの登録(配列の空きに入れる)
	bool registerObject(vnObject* p);	//3D用

	//オブジェクトの破棄(配列から削除/オブジェクト自体をdeleteする)
	void deleteObject(vnObject* p);		//3D用
};
