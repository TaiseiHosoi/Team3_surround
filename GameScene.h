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
	/// �R���X�g�N���^
	/// </summary>
	GameScene();

	/// <summary>
	/// �f�X�g���N�^
	/// </summary>
	~GameScene();

	/// <summary>
	/// ������
	/// </summary>
	void Initialize(DirectXCommon* dxcomon);

	/// <summary>
	/// ���t���[������
	/// </summary>
	void Update();

	/// <summary>
	/// �`��
	/// </summary>
	void Draw();

public:
	//�G���X�g���擾
	std::list<std::unique_ptr<Enemy>> enemys_;
	const std::list<std::unique_ptr<Enemy>>& GetEnemys() { return enemys_; }



	//�G�𑫂�
	void AddEnemy(std::unique_ptr<Enemy>& Enemy);
	void GenerEnemy(Vector3 EnemyPos, int ID, int lane);



	/// <summary>
	/// �G�����f�[�^�ǂݍ���
	/// </summary>
	void LoadEnemyPopData();
	void EnemyReset();
	void UpdateEnemyPopCommands();

	//�����蔻��
	void CheckAllCollisions();

	int gameLevel_ = 0;
	const int levelMax_ = 8;

	//�V�[���؂�ւ�
	enum class SceneNo {
		Title, //�^�C�g��
		Operate, //�������(�`���[�g���A��)
		Game,  //�ˌ�
		Over   //�Q���I�[�o�[
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

	//���\�[�X
	Model* whiteCube = nullptr;
	Model* cylinder = nullptr;

	//�V�[��
	Player* player_ = nullptr;

	//�ҋ@���t���O
	bool isStand_ = false;
	//�ҋ@�^�C�}�[
	int standTime_ = 0;
	int gameTimer_ = 0;

	//�K�E�Z���f���p<-�K�E�Z�S�ʂ̓Q�[���V�[�������ł��B���߂�Ȃ����B
	Object3d* deathblowTransform_ = nullptr;
	bool isDeathblow_ = false;
	Model* circle_ = nullptr;
	float deathblowRadius = 0.0f;

	//csv
	std::stringstream enemyPopCommands_;

	/// <summary>
	/// �Q�[���V�[���p
	/// </summary>
	SceneNo sceneNo_ = SceneNo::Title;

	//�X�R�A
	int hit_ = 0;
	//���[��
	int popLane_ = 0;
	
};