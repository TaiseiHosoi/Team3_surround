#pragma once
#include "Input.h"
#include "Model.h"
#include "Object3d.h"
#include <list>
#include <cassert>


//�O���錾
#include"Player.h"

class GameScene;

class Enemy
{
public:
	void Initialize(Model* redCube,Model* model, Vector3 vector3, float kBulSpeed);
	void Update();
	void Draw();

	bool IsDead() const { return isDead_; }	//���S��
	int GetId() { return enemyId_; }
	void SetID(int ID) { enemyId_ = ID; }

	//�e�̃C�[�W���O
	float easeIn(float x);

	////���[���h���W���擾
	Vector3 GetWorldPosition();

	//�Փ˂����o������Ăяo�����R�[���o�b�N�֐�
	void OnCollision(bool isBreak);

public:
	void SetPlayer(Player* player);

private:
	//���[���h�ϊ��f�[�^
	Object3d worldTransform_;
	Object3d predictionLine_;
	Object3d predictionPoint_;

	//���f��
	Model* model_ = nullptr;
	Model* redCube_ = nullptr;

	//���̑�
	uint32_t textureHandle_ = 0u;
	Input* input_ = nullptr;

	//�ŗLID
	int enemyId_ = 0;

	//�f�X�t���O
	bool isDead_ = false;

	float depth = 40.0f;	//���s
	float xDifference = 10.0f;	//���E��

	//�m�[�c�̑��x
	float kEnemySpeedZ = 0.4;
	//�m�[�c�̉����x
	float kEnemySpeedAcc = 0.003;

	//���݂̃��[��
	//Lane lane_;

	//�t�B�[���h�擾�p
	int fieldLane_ = 0;

	Player* player_ = nullptr;
	

};

