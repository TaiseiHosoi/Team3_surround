#include "GameScene.h"


GameScene::GameScene()
{
}

GameScene::~GameScene()
{
}

void GameScene::Initialize(DirectXCommon* dxcomon)
{
	assert(dxcomon);
	dxCommon_ = dxcomon;
	////DirectX初期化処理　ここから
	//dxCommon_->Initialize(winApp_);
	//スプライト共通部分の初期化
	spritecommon = new SpriteCommon;
	spritecommon->Initialize(dxCommon_);
	spritecommon->LoadTexture(0, "meemu.jpg");
	spritecommon->LoadTexture(1, "mario.png");
	spritecommon->LoadTexture(2, "reimu.png");

	sprite = new Sprite();
	sprite->Initialize(spritecommon, 0);
	sprite2 = new Sprite();
	sprite2->Initialize(spritecommon, 1);
	sprite3 = new Sprite();
	sprite3->Initialize(spritecommon, 2);

	sprite->SetSize({ 200,200 });
	sprite2->SetSize({ 200,200 });
	sprite3->SetSize({ 200,200 });
	sprite2->SetPozition({ 100,100 });
	sprite3->SetPozition({ 100,200 });
	audio = new Audio();
	audio->Initialize();

	//OBJからモデルデータを読み込む
	model = Model::LoadFormOBJ("cube");

	object3d = Object3d::Create();
	object3d->SetModel(model);

	

	audio->LoadWave("se_amd06.wav");
	audio->PlayWave("se_amd06.wav");

	//シングルトン
	input_ = Input::GetInstance();

	//リソース
	whiteCube = Model::LoadFormOBJ("cube");

	//ゲームシーンインスタンス
	player_ = new Player;
	player_->Initialize(model);
}

void GameScene::Update()
{
	object3d->Update();
	player_->Update();
}

void GameScene::Draw()
{
	spritecommon->SpritePreDraw();
	sprite->Draw();
	sprite2->Draw();
	sprite3->Draw();

	spritecommon->SpritePostDraw();

	Object3d::PreDraw(dxCommon_->GetCommandList());

	object3d->Draw();
	player_->Draw();

	Object3d::PostDraw();

}
