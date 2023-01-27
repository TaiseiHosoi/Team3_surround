#pragma once
#include "DirectXCommon.h"
#include "SpriteCommon.h"
#include "Sprite.h"
#include "Object3d.h"
#include "Audio.h"
#include "Model.h"

#include "memory"
#include "list"
#include "sstream"

#include "Player.h"
#include "Enemy.h"



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

public:
	//敵リストを取得
	std::list<std::unique_ptr<Enemy>> enemys_;
	const std::list<std::unique_ptr<Enemy>>& GetEnemys() { return enemys_; }



	//敵を足す
	void AddEnemy(std::unique_ptr<Enemy>& Enemy);
	void GenerEnemy(Vector3 EnemyPos, int ID, int lane);



	/// <summary>
	/// 敵発生データ読み込み
	/// </summary>
	void LoadEnemyPopData();
	void EnemyReset();
	void UpdateEnemyPopCommands();

	//当たり判定
	void CheckAllCollisions();

	int gameLevel_ = 0;
	const int levelMax_ = 8;

	//シーン切り替え
	enum class SceneNo {
		Title, //タイトル
		Operate, //操作説明(チュートリアル)
		Game,  //射撃
		Over   //ゲメオーバー
	};

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
	Model* cylinder = nullptr;

	//シーン
	Player* player_ = nullptr;

	//待機中フラグ
	bool isStand_ = false;
	//待機タイマー
	int standTime_ = 0;
	int gameTimer_ = 0;

	//必殺技モデル用<-必殺技全般はゲームシーン直下です。ごめんなさい。
	Object3d* deathblowTransform_ = nullptr;
	bool isDeathblow_ = false;
	Model* circle_ = nullptr;
	float deathblowRadius = 0.0f;

	//csv
	std::stringstream enemyPopCommands_;

	/// <summary>
	/// ゲームシーン用
	/// </summary>
	SceneNo sceneNo_ = SceneNo::Title;

	//スコア
	int hit_ = 0;
	//レーン
	int popLane_ = 0;
	
};