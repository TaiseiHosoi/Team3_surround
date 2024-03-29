#include "GameScene.h"
#include <cassert>
#include<fstream>
#include<stdlib.h>

using namespace Microsoft::WRL;

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
	spritecommon->LoadTexture(0, "haikei.png");
	spritecommon->LoadTexture(1, "mario.png");
	spritecommon->LoadTexture(2, "reimu.png");
	spritecommon->LoadTexture(3, "UI01.png");
	spritecommon->LoadTexture(4, "speedMemorie.png");
	spritecommon->LoadTexture(5, "noneKey.png");
	spritecommon->LoadTexture(6, "rightKey.png");
	spritecommon->LoadTexture(7, "leftKey.png");
	spritecommon->LoadTexture(8, "setumei.png");


	sprite = std::make_unique <Sprite>();
	sprite->Initialize(spritecommon, 0);
	sprite2 = std::make_unique <Sprite>();
	sprite2->Initialize(spritecommon, 1);
	sprite3 = std::make_unique <Sprite>();
	sprite3->Initialize(spritecommon, 2);


	UI01 = std::make_unique <Sprite>();
	UI01->Initialize(spritecommon, 3);
	maxSpeedMem = std::make_unique<Sprite>();
	maxSpeedMem->Initialize(spritecommon, 4);
	nowSpeedMem = std::make_unique<Sprite>();
	nowSpeedMem->Initialize(spritecommon, 4);

	noneKeySP = std::make_unique<Sprite>();
	noneKeySP->Initialize(spritecommon, 5);
	XMFLOAT2 picSize = { 150,90 };
	XMFLOAT2 picPos = { 60,550 };
	noneKeySP->SetSize(picSize);
	noneKeySP->SetPozition(picPos);
	rightKeySP = std::make_unique<Sprite>();
	rightKeySP->Initialize(spritecommon, 6);
	rightKeySP->SetSize(picSize );
	rightKeySP->SetPozition(picPos);
	leftKeySP = std::make_unique<Sprite>();
	leftKeySP->Initialize(spritecommon, 7);
	leftKeySP->SetSize( picSize );
	leftKeySP->SetPozition(picPos);

	tutorial = std::make_unique<Sprite>();
	tutorial->Initialize(spritecommon, 8);
	tutorial->SetSize({128.0f,32.0f});
	tutorial->SetPozition({500,100});

	sprite->SetSize({ 1280,720 });
	sprite2->SetSize({ 200,200 });
	sprite3->SetSize({ 200,200 });
	UI01->SetSize({ 1280,720 });

	sprite2->SetPozition({ 100,100 });
	sprite3->SetPozition({ 100,200 });

	maxSpeedMem->SetAnchorPoint({ 0,0 });
	maxSpeedMem->SetPozition({ 80,100 });
	nowSpeedMem->SetAnchorPoint({ 0,0 });
	nowSpeedMem->SetPozition({80,140});
	
	audio = new Audio();
	audio->Initialize();

	//OBJからモデルデータを読み込む
	model.reset(Model::LoadFormOBJ("cube",true));
	circle_.reset(Model::LoadFormOBJ("ico", true));
	redCube.reset(Model::LoadFormOBJ("redCube", true));

	object3d = Object3d::Create();
	object3d->SetModel(model.get());

	

	audio->LoadWave("se_amd06.wav");
	//audio->PlayWave("se_amd06.wav");

	//シングルトン
	input_ = Input::GetInstance();

	//リソース
	whiteCube.reset(Model::LoadFormOBJ("cube", true));
	playerModel.reset(Model::LoadFormOBJ("iceTier", true));

	//ゲームシーンインスタンス
	player_ = make_unique<Player>();
	player_->Initialize(model.get(), circle_.get(), playerModel.get());


	//敵初期化
	EnemyReset();

	//カウント初期化
	gameTimer_ = 0;
	gameLevel_ = 1;

	skyBoxModel.reset(Model::LoadFormOBJ("sky", true));
	railModel.reset(Model::LoadFormOBJ("rail",true));

	skyBox = std::make_unique<SkyBox>();
	skyBox->Initialize(skyBoxModel.get(),railModel.get());
}

void GameScene::Update()
{
	object3d->Update();


	gameTimer_++;
	if (gameTimer_ > 300) {
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

	if (player_->GetIsAtkDraw() == true) {
		CheckAllCollisions();
	}

	//スピードゲージ用
	
	
	maxSpeedMem->SetSize({ (float)player_.get()->GetMaxTimeCount() * 3.0f , 30 });
	nowSpeedMem->SetSize({ (float)player_.get()->GetNowTimeCount() * 3.0f , 30 });
	

}

void GameScene::Draw()
{
	spritecommon->SpritePreDraw();
	//sprite->Draw();
	//sprite2->Draw();
	//sprite3->Draw();
	

	spritecommon->SpritePostDraw();

	Object3d::PreDraw(dxCommon_->GetCommandList());

	//object3d->Draw();
	player_->Draw();
	for (std::unique_ptr<Enemy>& enemy_ : enemys_) {
		enemy_->Draw();
	}
	skyBox->Draw();

	Object3d::PostDraw();

	//後景スプライト
	spritecommon->SpritePreDraw();
	//sprite->Draw();
	//sprite2->Draw();
	//sprite3->Draw();
	UI01->Draw();
	maxSpeedMem->Draw();
	nowSpeedMem->Draw();

	if (input_->PushKey(DIK_RIGHT)) {
		rightKeySP->Draw();
	}
	else if (input_->PushKey(DIK_LEFT)) {
		leftKeySP->Draw();
	}
	else {
		noneKeySP->Draw();
	}
	tutorial->Draw();

	spritecommon->SpritePostDraw();
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
		newEnemy->Initialize(redCube.get(),model.get(), EnemyPos, kBulSpeed);
	}
	else if (lane == 1) {
		newEnemy->Initialize(redCube.get(),model.get(), EnemyPos, kBulSpeed);
	}
	else if (lane == 2) {
		newEnemy->Initialize(redCube.get(),model.get(), EnemyPos, kBulSpeed);
	}

	newEnemy->SetID(ID);
	newEnemy->SetPlayer(player_.get());
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
			int maxTimeDiv = 2;
			if (gameLevel_ <= 0) {
				standTime_ = waitTime * (10 - gameLevel_) / maxTimeDiv;
			}
			else {

				standTime_ = waitTime * (10 - gameLevel_) / maxTimeDiv;
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
	Vector3 posA; 
	std::vector<atkColide> atkColideB_;
	atkColideB_ = player_.get()->GetAtkColide();



#pragma region 自弾と敵キャラの当たり判定
	//敵キャラの座標
	for (std::unique_ptr<Enemy>& enemy_ : enemys_) {
		posA = enemy_->GetWorldPosition();
		
		for (int i = 0; i < std::end(atkColideB_) - std::begin(atkColideB_); ++i) {
			if (atkColideB_[i].isColide == true) {
				if (atkColideB_[i].atkTransform.position.x + atkColideB_[i].atkTransform.scale.x  >
					posA.x &&
					atkColideB_[i].atkTransform.position.x - atkColideB_[i].atkTransform.scale.x  <
					posA.x) {
					if (atkColideB_[i].atkTransform.position.y + atkColideB_[i].atkTransform.scale.y  >
						posA.y &&
						atkColideB_[i].atkTransform.position.y - atkColideB_[i].atkTransform.scale.y  <
						posA.y) {

						enemy_->OnCollision(false);
					}

				}
			}
		}
		
		////自弾の座標
		//posB = { 0,0,-20 };

		//float x = posB.x - posA.x;
		//float y = posB.y - posA.y;
		//float z = posB.z - posA.z;

		//float cd = sqrt(x * x + y * y + z * z);

		//if (cd <= 4.0f) {
		//	//敵キャラの衝突時コールバックを呼び出す
		//	enemy_->OnCollision(true);
		//	//GenerEffect(enemy_->GetWorldPosition(), enemy_->GetFieldLane());

		//	//衝突時コールバックを呼び出す
		//	//goal_->OnCollision();
		//	hit_++;
		//}


		//if (cd <= deathblowRadius) {
		//	//敵キャラの衝突時コールバックを呼び出す
		//	enemy_->OnCollision(true);
		//	//GenerEffect(enemy_->GetWorldPosition(), enemy_->GetFieldLane());
		//	hit_++;
		//	//衝突時コールバックを呼び出す
		//	//goal_->OnCollision();
		//}

		if (posA.z < -50/*画面外*/) {
			enemy_->OnCollision(false);

		}



	}
}
