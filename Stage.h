#pragma once
#include "./Library/GameObject.h"
#include "Source/Screen.h"
#include "global.h"


class Stage :
    public GameObject
{
public:
	Stage();
	~Stage();
	void Update() override;
	void Draw() override;
	int GetMap(int x, int y);
};

