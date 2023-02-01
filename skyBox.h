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

	std::unique_ptr<Object3d> heightRail[2];

	std::unique_ptr<Object3d> widthRail[20];

	Model* boxModel_;
	Model* railModel_;
};

