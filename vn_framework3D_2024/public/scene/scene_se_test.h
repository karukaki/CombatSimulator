//--------------------------------------------------------------//
//	"scene_se_test.h"											//
//		SEƒeƒXƒg												//
//													2024/04/01	//
//														Ichii	//
//--------------------------------------------------------------//
#pragma once

class SceneSeTest : public vnScene
{
private:
	int fileNum;
	vnSound	**pSound;
	
public:
	
	bool initialize();
	void terminate();
	
	void execute();
	void render();
};
