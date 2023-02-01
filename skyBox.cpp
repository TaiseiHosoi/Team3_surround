#include "skyBox.h"
#include<random>
#include<time.h>

void SkyBox::Initialize(Model* boxModel, Model* railModel)
{
	assert(boxModel);
	assert(railModel);

	std::srand(time(NULL));

	boxModel_ =boxModel;
	railModel_ = railModel;

	obj = std::make_unique<Object3d>();

	obj->SetModel(boxModel);

	obj->Initialize(false);

	obj->SetScale({ 50,50,70 });

	obj->SetPosition({ 0,0,450 });

	obj2 = std::make_unique<Object3d>();

	obj2->SetModel(boxModel);

	obj2->Initialize(false);

	obj2->SetScale({ 50,50,70 });

	obj2->SetPosition({ 0,0,1350 });

	Rail[0] = std::make_unique<Object3d>();

	Rail[1] = std::make_unique<Object3d>();

	Rail[0]->Initialize(false);

	Rail[1]->Initialize(false);

	Rail[0]->SetModel(railModel);

	Rail[1]->SetModel(railModel);

	Rail[0]->SetScale({ 0.1,0.1,200 });

	Rail[1]->SetScale({ 0.1,0.1,200 });

	Rail[0]->SetPosition({ -45,-45,150 });

	Rail[1]->SetPosition({ 45,-45,150 });

	for (int i = 0; i < 30; i++)
	{
		widthRail[i] = std::make_unique<Object3d>();

		widthRail[i]->Initialize(false);

		widthRail[i]->SetModel(railModel);

		widthRail[i]->SetScale({ 45,0.1,0.1 });

		widthRail[i]->SetPosition({ 0,-45,static_cast<float>(i * 20) });
	}

	for (int i = 0; i < 30; i++)
	{
		height[i][0] = std::make_unique<Object3d>();
		height[i][1] = std::make_unique<Object3d>();

		height[i][0]->Initialize(false);
		height[i][1]->Initialize(false);

		height[i][0]->SetModel(railModel);
		height[i][1]->SetModel(railModel);

		height[i][0]->SetScale({0.1,45.0f,0.1});
		height[i][1]->SetScale({0.1,45.0f,0.1});

		height[i][0]->SetPosition({-45,0,static_cast<float>(i * 20)});
		height[i][1]->SetPosition({ 45,0,static_cast<float>(i * 20)});
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

	if (move.z <= -450)
	{
		obj->SetPosition({ 0, 0, 1350 });
	}
	if (move2.z <= -450)
	{
		obj2->SetPosition({ 0, 0, 1350 });
	}

	for (int i = 0; i < 30; i++)
	{
		Vector3 moveRail = widthRail[i]->GetPosition();

		moveRail.z--;

		widthRail[i]->SetPosition(moveRail);

		if (moveRail.z <= -15)
		{
			widthRail[i]->SetPosition({ 0, -45,600 });
		}
	}

	for (int i = 0; i < 30; i++)
	{
			Vector3 Move = height[i][0]->GetPosition();
			Vector3 Move2 = height[i][1]->GetPosition();

			Move.z--;
			Move2.z--;

			height[i][0]->SetPosition(Move);
			height[i][1]->SetPosition(Move2);

			if (Move.z <= -15)
			{
				height[i][0]->SetPosition({-45, 0,600});
				height[i][1]->SetPosition({ 45, 0,600 });
			}
	}

	obj2->Update();

	obj->Update();

	Rail[0]->Update();

	Rail[1]->Update();

	for (int i = 0; i < 30; i++)
	{
		widthRail[i]->Update();
	}

	for (int i = 0; i < 30; i++)
	{
		height[i][0]->Update();
		height[i][1]->Update();
	}
}

void SkyBox::Draw()
{
	obj->Draw();

	obj2->Draw();

	Rail[0]->Draw();

	Rail[1]->Draw();

	for (int i = 0; i < 30; i++)
	{
		widthRail[i]->Draw();
	}

	for (int i = 0; i < 30; i++)
	{
		height[i][0]->Draw();
		height[i][1]->Draw();
	}
}
