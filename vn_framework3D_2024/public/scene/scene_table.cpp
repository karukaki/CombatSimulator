//--------------------------------------------------------------//
//	"scene_table.cpp"											//
//		シーンテーブル											//
//													2024/04/01	//
//														Ichii	//
//--------------------------------------------------------------//
#include "../../framework.h"
#include "../../framework/vn_environment.h"

//起動時のシーン
eSceneTable initialScene = eSceneTable::Title;
//現在のシーン
eSceneTable currentScene = (eSceneTable)-1;
//切り替え予約のシーン
eSceneTable reserveScene = initialScene;


//各シーンの名前
WCHAR SceneName[(int)eSceneTable::SceneMax][32] =
{
	L"Boot",
	L"Title",
	L"Main Game",
	L"Collision Test",
	L"Field Test",
	L"Model Test",
	L"Cube Test",
	L"Sprite Test",
	L"Joystick Test",
	L"SE Test",
	L"Font Test",
};

//ステージ切り替え関数(予約)
void switchScene(eSceneTable scene)
{
	reserveScene = scene;
}

//現在のステージ取得(実処理)
void switchScene()
{
	if(currentScene==reserveScene)
	{
		return;
	}
	
	vnMainFrame::terminateScene();
	
	vnScene *pScene=NULL;
	switch(reserveScene)
	{
	case Boot:			pScene = new SceneBoot();			break;
	case Title:			pScene = new SceneTitle();			break;
	case MainGame:		pScene = new SceneMainGame();		break;
	case CollisionTest:	pScene = new SceneCollisionTest();	break;
	case FieldTest:		pScene = new SceneFieldTest();		break;
	case ModelTest:		pScene = new SceneModelTest();		break;
	case CubeTest:		pScene = new SceneCubeTest();		break;
	case SpriteTest:	pScene = new SceneSpriteTest();		break;
	case JoystickTest:	pScene = new SceneJoystickTest();	break;
	case SeTest:		pScene = new SceneSeTest();			break;
	case FontTest:		pScene = new SceneFontTest();		break;
	}
	
	vnMainFrame::initializeScene(pScene);
	
	currentScene = reserveScene;
}
