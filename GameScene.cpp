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
	////DirectX�����������@��������
	//dxCommon_->Initialize(winApp_);
	//�X�v���C�g���ʕ����̏�����
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

	//OBJ���烂�f���f�[�^��ǂݍ���
	model = Model::LoadFormOBJ("cube",false);
	playerModel = Model::LoadFormOBJ("playerbullet",true);
	object3d = new Object3d;
	object3d->Initialize(false);
	object3d->SetModel(model);

	

	audio->LoadWave("se_amd06.wav");
	audio->PlayWave("se_amd06.wav");

	//�V���O���g��
	input_ = Input::GetInstance();

	//���\�[�X
	whiteCube = Model::LoadFormOBJ("cube", false);

	//�Q�[���V�[���C���X�^���X
	player_ = new Player;
	player_->Initialize(model,playerModel);


	//�G������
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

	//�f���[�g
	enemys_.remove_if([](std::unique_ptr<Enemy>& enemy_) { return enemy_->IsDead(); });


	//�e����
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
	//����
	std::unique_ptr<Enemy> newEnemy = std::make_unique<Enemy>();
	//�G�L�����̏�����
	float kBulSpeed = 0.4f;
	if (gameLevel_ > 0) {
		kBulSpeed += gameLevel_ * 0.1f + 1.0f;	//���x�����オ��ƒe������
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

	//���X�g�ɓo�^����
	enemys_.push_back(std::move(newEnemy));

}

void GameScene::LoadEnemyPopData()
{
	//�t�@�C�����J��
	std::ifstream file;
	file.open("Resources/enemyPop2.csv");

	assert(file.is_open());

	//�t�@�C���̓��e�𕶎���X�g���[���ɃR�s�[
	enemyPopCommands_ << file.rdbuf();

	//�t�@�C�������
	file.close();
}

void GameScene::UpdateEnemyPopCommands()
{
	//�ҋ@����
	if (isStand_) {
		standTime_--;
		if (standTime_ <= 0) {
			//�ҋ@����
			isStand_ = false;
		}
		return;
	}
	// 1�s���̕����������ϐ�
	std::string line;

	//�R�}���h���s���[�v
	while (getline(enemyPopCommands_, line)) {
		// 1�s���̕��������X�g���[���ɕϊ����ĉ�܂��₷���Ȃ�
		std::istringstream line_stream(line);

		std::string word;
		//,��؂�ōs�̐擪�������擾
		getline(line_stream, word, ',');

		//"//"����n�܂�s�̓R�����g
		if (word.find("//") == 0) {
			//�R�����g�s���΂�
			continue;
		}
		// POP�R�}���h
		if (word.find("POP") == 0) {

			//���[��
			std::getline(line_stream, word, ',');
			int lane = static_cast<int>(std::atof(word.c_str()));

			// ID
			std::getline(line_stream, word, ',');
			int ID = static_cast<int>(std::atof(word.c_str()));

			float depth = 400.0f;	//���s
			float xDifference = 10.0f;	//���E��

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
		// WAIT�R�}���h
		else if (word.find("WAIT") == 0) {
			std::getline(line_stream, word, ',');

			//�҂�����
			int32_t waitTime = std::atoi(word.c_str());

			//�ҋ@�J�n
			isStand_ = true;
			int maxTimeDiv = 10;
			if (gameLevel_ <= 0) {
				standTime_ = waitTime * (maxTimeDiv - gameLevel_) / maxTimeDiv;
			}
			else {

				standTime_ = waitTime * (maxTimeDiv - gameLevel_) / maxTimeDiv;
			}

			//������
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

	//����Ώ�A��B�̍��W
	Vector3 posA, posB;



#pragma region ���e�ƓG�L�����̓����蔻��
	//�G�L�����̍��W
	for (std::unique_ptr<Enemy>& enemy_ : enemys_) {
		posA = enemy_->GetWorldPosition();


		//���e�̍��W
		posB = { 0,0,-20 };

		float x = posB.x - posA.x;
		float y = posB.y - posA.y;
		float z = posB.z - posA.z;

		float cd = sqrt(x * x + y * y + z * z);
		
		if (cd <= 4.0f) {
			//�G�L�����̏Փˎ��R�[���o�b�N���Ăяo��
			enemy_->OnCollision(true);
			//GenerEffect(enemy_->GetWorldPosition(), enemy_->GetFieldLane());

			//�Փˎ��R�[���o�b�N���Ăяo��
			//goal_->OnCollision();
			hit_++;
		}


		if (cd <= deathblowRadius) {
			//�G�L�����̏Փˎ��R�[���o�b�N���Ăяo��
			enemy_->OnCollision(true);
			//GenerEffect(enemy_->GetWorldPosition(), enemy_->GetFieldLane());
			hit_++;
			//�Փˎ��R�[���o�b�N���Ăяo��
			//goal_->OnCollision();
		}

		if (posA.z < -50/*��ʊO*/) {
			enemy_->OnCollision(false);

		}



	}
}
