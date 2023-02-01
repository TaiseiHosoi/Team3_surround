#include "skyBox.h"

void SkyBox::Initialize(Model* boxModel, Model* railModel)
{
	boxModel_ = boxModel;
	railModel_ = railModel;

	obj = std::make_unique<Object3d>();

	obj->SetModel(boxModel);

	obj->Initialize(false);

	obj->SetScale({ 20,20,20 });

	obj->SetPosition({ 0,0,125 });

	obj2 = std::make_unique<Object3d>();

	obj2->SetModel(boxModel);

	obj2->Initialize(false);

	obj2->SetScale({ 20,20,20 });

	obj2->SetPosition({ 0,0,375 });

	Object3d::SetEye({ 0.0f,0.0f,-15.0f });

	heightRail[0] = std::make_unique<Object3d>();

	heightRail[1] = std::make_unique<Object3d>();

	heightRail[0]->Initialize(false);

	heightRail[1]->Initialize(false);

	heightRail[0]->SetModel(railModel);

	heightRail[1]->SetModel(railModel);

	heightRail[0]->SetScale({ 0.1,0.1,200 });

	heightRail[1]->SetScale({ 0.1,0.1,200 });

	heightRail[0]->SetPosition({ -10,-10,190 });

	heightRail[1]->SetPosition({ 10,-10,190 });

	for (int i = 0; i < 20; i++)
	{
		widthRail[i] = std::make_unique<Object3d>();

		widthRail[i]->Initialize(false);

		widthRail[i]->SetModel(railModel);

		widthRail[i]->SetScale({ 10,0.1,0.1 });

		widthRail[i]->SetPosition({ 0,-10,static_cast<float>(i * 20) });
	}
}

void SkyBox::Update()
{
	Vector3 move = obj->GetPosition();

	move.z--;

	obj->SetPosition(move);

	Vector3 move2 = obj2->GetPosition();

	move2.z--;

	obj2->SetPosition(move2);

	if (move.z <= -125)
	{
		obj->SetPosition({ 0, 0, 375 });
	}
	if (move2.z <= -125)
	{
		obj2->SetPosition({ 0, 0, 375 });
	}

	for (int i = 0; i < 20; i++)
	{
		Vector3 moveRail = widthRail[i]->GetPosition();

		moveRail.z--;

		widthRail[i]->SetPosition(moveRail);

		if (moveRail.z <= -15)
		{
			widthRail[i]->SetPosition({ 0, -10,400 });
		}
	}

	obj2->Update();

	obj->Update();

	heightRail[0]->Update();

	heightRail[1]->Update();

	for (int i = 0; i < 20; i++)
	{
		widthRail[i]->Update();
	}
}

void SkyBox::Draw()
{
	obj->Draw();

	obj2->Draw();

	heightRail[0]->Draw();

	heightRail[1]->Draw();

	for (int i = 0; i < 20; i++)
	{
		widthRail[i]->Draw();
	}
}
