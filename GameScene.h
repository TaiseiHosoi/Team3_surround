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

#include"skyBox.h"



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

	int gameLevel_ = 1;
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
	std::unique_ptr<Sprite> sprite;
	std::unique_ptr<Sprite> sprite2;
	std::unique_ptr<Sprite> sprite3;
	std::unique_ptr<Sprite> UI01;
	std::unique_ptr<Sprite> maxSpeedMem;
	std::unique_ptr<Sprite> nowSpeedMem;
	std::unique_ptr<Sprite> noneKeySP;
	std::unique_ptr<Sprite> rightKeySP;
	std::unique_ptr<Sprite> leftKeySP;
	std::unique_ptr<Sprite> tutorial;
	std::unique_ptr<Sprite> space;
	std::unique_ptr<Sprite> result;
	std::unique_ptr<Sprite> resultMaxEnemy;
	std::unique_ptr<Sprite> resultNumberTens;
	std::unique_ptr<Sprite> resultNumberOnes;
	std::unique_ptr<Sprite> gameMaxEnemy;
	std::unique_ptr<Sprite> gameNumberTens;
	std::unique_ptr<Sprite> gameNumberOnes;

	

	Object3d* object3d = nullptr;
	std::unique_ptr<Model> model;
	std::unique_ptr<Model> redCube;

	Input* input_ = nullptr;

	//リソース
	std::unique_ptr<Model> whiteCube;
	std::unique_ptr<Model> playerModel;

	//シーン
	std::unique_ptr <Player> player_;

	//待機中フラグ
	bool isStand_ = false;
	//待機タイマー
	int standTime_ = 0;
	int gameTimer_ = 0;

	//必殺技モデル用<-必殺技全般はゲームシーン直下です。ごめんなさい。
	std::unique_ptr <Object3d> deathblowTransform_ = nullptr;
	bool isDeathblow_ = false;
	std::unique_ptr <Model> circle_;
	float deathblowRadius = 0.0f;

	//csv
	std::stringstream enemyPopCommands_;

	/// <summary>
	/// ゲームシーン用
	/// </summary>
	SceneNo sceneNo_ = SceneNo::Over;

	//スコア
	int hit_ = 0;
	//レーン
	int popLane_ = 0;

	//スカイボックス
	std::unique_ptr <Model> skyBoxModel;

	std::unique_ptr <Model> railModel;

	std::unique_ptr<SkyBox> skyBox;

	//カメラ
	XMFLOAT3 normalEyePos;
	XMFLOAT3 extendEyePos;
	int cameraMode;
	
	int popEnemyCount=0;
	int killEnemyCount=0;
};