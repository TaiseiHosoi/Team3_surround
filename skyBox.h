#pragma once
#include"Object3d.h"
class SkyBox
{
public:
	void Initialize(Model* boxModel, Model* railModel);
	void Update();
	void Draw();
private:

	std::unique_ptr<Object3d> obj;

	std::unique_ptr<Object3d> obj2;

	std::unique_ptr<Object3d> Rail[2];

	std::unique_ptr<Object3d> widthRail[30];

	std::unique_ptr<Object3d> height[30][2];


	Model* boxModel_=nullptr;
	Model* railModel_ = nullptr;
};

