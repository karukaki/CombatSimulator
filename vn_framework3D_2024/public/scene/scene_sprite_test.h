#pragma once

class SceneSpriteTest : public vnScene
{
private:
	vnSprite* pSprite;

	//‘€ì‚Ìí—Ş
	enum eOperation
	{
		PositionX,
		PositionY,
		ScaleX,
		ScaleY,
		Rotate,
		OperationMax,
	};

	int	Cursor;

public:
	bool initialize();
	void terminate();
	
	void execute();
	void render();
};
