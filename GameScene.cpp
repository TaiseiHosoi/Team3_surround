#include "GameScene.h"
#include <cassert>
#include<fstream>
#include<stdlib.h>


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
	model = Model::LoadFormOBJ("cube",false);
	playerModel = Model::LoadFormOBJ("playerbullet",true);
	object3d = new Object3d;
	object3d->Initialize(false);
	object3d->SetModel(model);

	

	audio->LoadWave("se_amd06.wav");
	audio->PlayWave("se_amd06.wav");

	//シングルトン
	input_ = Input::GetInstance();

	//リソース
	whiteCube = Model::LoadFormOBJ("cube", false);

	//ゲームシーンインスタンス
	player_ = new Player;
	player_->Initialize(model,playerModel);


	//敵初期化
	EnemyReset();

	skyBoxModel = Model::LoadFormOBJ("sky",false);

	railModel = Model::LoadFormOBJ("rail",false);

	skyBox = std::make_unique<SkyBox>();

	skyBox->Initialize(skyBoxModel, railModel);
}

void GameScene::Update()
{
	object3d->Update();


	gameTimer_++;
	if (gameTimer_ > 200) {
		if (gameLevel_ < levelMax_) {
			gameTimer_ = 0;
			gameLevel_++;
		}
		else {
			gameTimer_ = 0;
		}

	}

	player_->Update();

	//デリート
	enemys_.remove_if([](std::unique_ptr<Enemy>& enemy_) { return enemy_->IsDead(); });


	//弾発生
	UpdateEnemyPopCommands();

	for (std::unique_ptr<Enemy>& enemy_ : enemys_) {
		enemy_->Update();
	}

	skyBox->Update();

	CheckAllCollisions();
}

void GameScene::Draw()
{
	spritecommon->SpritePreDraw();
	sprite->Draw();
	sprite2->Draw();
	sprite3->Draw();

	spritecommon->SpritePostDraw();

	Object3d::PreDraw(dxCommon_->GetCommandList());

	/*object3d->Draw();
	player_->Draw();
	for (std::unique_ptr<Enemy>& enemy_ : enemys_) {
		enemy_->Draw();
	}*/
	skyBox->Draw();

	Object3d::PostDraw();
}

void GameScene::GenerEnemy(Vector3 EnemyPos, int ID, int lane)
{
	//生成
	std::unique_ptr<Enemy> newEnemy = std::make_unique<Enemy>();
	//敵キャラの初期化
	float kBulSpeed = 0.4f;
	if (gameLevel_ > 0) {
		kBulSpeed += gameLevel_ * 0.1f + 1.0f;	//レベルが上がると弾が加速
	}

	if (lane == 0) {
		newEnemy->Initialize(model, EnemyPos, kBulSpeed);
	}
	else if (lane == 1) {
		newEnemy->Initialize(model, EnemyPos, kBulSpeed);
	}
	else if (lane == 2) {
		newEnemy->Initialize(model, EnemyPos, kBulSpeed);
	}

	newEnemy->SetID(ID);
	newEnemy->SetPlayer(player_);
	//newEnemy->SetFieldLane(lane);

	//リストに登録する
	enemys_.push_back(std::move(newEnemy));

}

void GameScene::LoadEnemyPopData()
{
	//ファイルを開く
	std::ifstream file;
	file.open("Resources/enemyPop2.csv");

	assert(file.is_open());

	//ファイルの内容を文字列ストリームにコピー
	enemyPopCommands_ << file.rdbuf();

	//ファイルを閉じる
	file.close();
}

void GameScene::UpdateEnemyPopCommands()
{
	//待機処理
	if (isStand_) {
		standTime_--;
		if (standTime_ <= 0) {
			//待機完了
			isStand_ = false;
		}
		return;
	}
	// 1行分の文字列を入れる変数
	std::string line;

	//コマンド実行ループ
	while (getline(enemyPopCommands_, line)) {
		// 1行分の文字数をストリームに変換して解折しやすくなる
		std::istringstream line_stream(line);

		std::string word;
		//,区切りで行の先頭文字を取得
		getline(line_stream, word, ',');

		//"//"から始まる行はコメント
		if (word.find("//") == 0) {
			//コメント行を飛ばす
			continue;
		}
		// POPコマンド
		if (word.find("POP") == 0) {

			//レーン
			std::getline(line_stream, word, ',');
			int lane = static_cast<int>(std::atof(word.c_str()));

			// ID
			std::getline(line_stream, word, ',');
			int ID = static_cast<int>(std::atof(word.c_str()));

			float depth = 400.0f;	//奥行
			float xDifference = 10.0f;	//左右差

			Vector2 respawnPos = { float(rand() % 30) - 15,float(rand() % 20) - 10 };//x:15~-15,y:10~-10


			GenerEnemy(Vector3(respawnPos.x, respawnPos.y, depth), ID, popLane_);


			/*if (lane == 1) {
				for (int i = 0; i < 3; i++) {
					if (field_[i].GetLane() == 0) {
						popLane_ = i;
						break;
					}
				}
				GenerEnemy(Vector3(-xDifference, 0, depth), ID, popLane_);

			}
			else if (lane == 2) {
				for (int i = 0; i < 3; i++) {
					if (field_[i].GetLane() == 1) {
						popLane_ = i;
						break;
					}
				}
				GenerEnemy(Vector3(0, 0, depth), ID, popLane_);
			}
			else if (lane == 3) {
				for (int i = 0; i < 3; i++) {
					if (field_[i].GetLane() == 2) {
						popLane_ = i;
						break;
					}
				}
				GenerEnemy(Vector3(xDifference, 0, depth), ID, popLane_);
			}
			else {


			}*/
		}
		// WAITコマンド
		else if (word.find("WAIT") == 0) {
			std::getline(line_stream, word, ',');

			//待ち時間
			int32_t waitTime = std::atoi(word.c_str());

			//待機開始
			isStand_ = true;
			int maxTimeDiv = 10;
			if (gameLevel_ <= 0) {
				standTime_ = waitTime * (maxTimeDiv - gameLevel_) / maxTimeDiv;
			}
			else {

				standTime_ = waitTime * (maxTimeDiv - gameLevel_) / maxTimeDiv;
			}

			//抜ける
			break;
		}
		else if (word.find("OVER") == 0) {
			sceneNo_ = SceneNo::Over;
			break;

		}
	}
}

void GameScene::EnemyReset()
{
	enemyPopCommands_.str("");
	enemyPopCommands_.clear(std::stringstream::goodbit);
	LoadEnemyPopData();
}


void GameScene::CheckAllCollisions() {

	//判定対象AとBの座標
	Vector3 posA, posB;



#pragma region 自弾と敵キャラの当たり判定
	//敵キャラの座標
	for (std::unique_ptr<Enemy>& enemy_ : enemys_) {
		posA = enemy_->GetWorldPosition();


		//自弾の座標
		posB = { 0,0,-20 };

		float x = posB.x - posA.x;
		float y = posB.y - posA.y;
		float z = posB.z - posA.z;

		float cd = sqrt(x * x + y * y + z * z);
		
		if (cd <= 4.0f) {
			//敵キャラの衝突時コールバックを呼び出す
			enemy_->OnCollision(true);
			//GenerEffect(enemy_->GetWorldPosition(), enemy_->GetFieldLane());

			//衝突時コールバックを呼び出す
			//goal_->OnCollision();
			hit_++;
		}


		if (cd <= deathblowRadius) {
			//敵キャラの衝突時コールバックを呼び出す
			enemy_->OnCollision(true);
			//GenerEffect(enemy_->GetWorldPosition(), enemy_->GetFieldLane());
			hit_++;
			//衝突時コールバックを呼び出す
			//goal_->OnCollision();
		}

		if (posA.z < -50/*画面外*/) {
			enemy_->OnCollision(false);

		}



	}
}
