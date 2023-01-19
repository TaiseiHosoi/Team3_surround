#pragma once
#include "DirectXCommon.h"
#include "SpriteCommon.h"
#include "Sprite.h"
#include "Object3d.h"
#include "Audio.h"
#include "Model.h"
#include "Player.h"

class GameScene {
public:
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(DirectXCommon* dxcomon);

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

private:
	DirectXCommon* dxCommon_ = nullptr;
	
	Audio* audio = nullptr;
	SpriteCommon* spritecommon = nullptr;
	Sprite* sprite = nullptr;
	Sprite* sprite2 = nullptr;
	Sprite* sprite3 = nullptr;
	Object3d* object3d = nullptr;
	Model* model = nullptr;

	Input* input_ = nullptr;

	//リソース
	Model* whiteCube = nullptr;

	//シーンインスタンス
	Player* player_ = nullptr;
	
};