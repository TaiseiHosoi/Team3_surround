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

	int gameLevel_ = 1;
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

	//���\�[�X
	std::unique_ptr<Model> whiteCube;
	std::unique_ptr<Model> playerModel;

	//�V�[��
	std::unique_ptr <Player> player_;

	//�ҋ@���t���O
	bool isStand_ = false;
	//�ҋ@�^�C�}�[
	int standTime_ = 0;
	int gameTimer_ = 0;

	//�K�E�Z���f���p<-�K�E�Z�S�ʂ̓Q�[���V�[�������ł��B���߂�Ȃ����B
	std::unique_ptr <Object3d> deathblowTransform_ = nullptr;
	bool isDeathblow_ = false;
	std::unique_ptr <Model> circle_;
	float deathblowRadius = 0.0f;

	//csv
	std::stringstream enemyPopCommands_;

	/// <summary>
	/// �Q�[���V�[���p
	/// </summary>
	SceneNo sceneNo_ = SceneNo::Over;

	//�X�R�A
	int hit_ = 0;
	//���[��
	int popLane_ = 0;

	//�X�J�C�{�b�N�X
	std::unique_ptr <Model> skyBoxModel;

	std::unique_ptr <Model> railModel;

	std::unique_ptr<SkyBox> skyBox;

	//�J����
	XMFLOAT3 normalEyePos;
	XMFLOAT3 extendEyePos;
	int cameraMode;
	
	int popEnemyCount=0;
	int killEnemyCount=0;
};