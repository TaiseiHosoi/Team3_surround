#include "Enemy.h"
#include <cmath>
#include "math.h"
#define PI 3.141592653589

void Enemy::Initialize(Model* redCube, Model* model,Vector3 vector3, float kBulSpeed)
{
	// NULL�|�C���^�`�F�b�N
	assert(model);
	model_ = model;
	redCube_ = redCube;
	worldTransform_.SetModel(model);
	predictionLine_.SetModel(redCube_);
	predictionPoint_.SetModel(redCube_);

	//�V���O���C���X�^���X���擾����
	input_ = Input::GetInstance();

	//�������W���Z�b�g
	worldTransform_.Initialize(false);
	worldTransform_.position = vector3;
	worldTransform_.scale = { 1,1,1 };
	worldTransform_.rotation = { 0,0,0 };

	//�\����
	predictionLine_.Initialize(false);
	predictionLine_.position = vector3;
	predictionLine_.scale = { 0.1f,0.1f,1.0f };
	predictionLine_.rotation = { 0,0,0 };
	predictionLine_.Update();

	//�\���|�C���g
	predictionPoint_.Initialize(false);
	predictionPoint_.position = { vector3.x,vector3.y,0 };
	predictionPoint_.scale = { 0.7f,0.7f,0.1f };
	predictionPoint_.rotation = { 0,0,0 };
	predictionPoint_.Update();

	kEnemySpeedZ = kBulSpeed;
}

void Enemy::Update()
{

	worldTransform_.Update();

	//�m�[�c�̑��x�̏���
	kEnemySpeedZ += kEnemySpeedAcc;
	Vector3 kEnemySpeed = { 0.0f,0.0f,-1.0f };
	worldTransform_.position += kEnemySpeed;

	//�\����
	predictionLine_.position.z = worldTransform_.position.z / 2.0f;
	predictionLine_.scale.z = (float)abs(worldTransform_.position.z) / 2.0f;
	predictionPoint_.position.z = 0.0f;

	worldTransform_.Update();
	predictionLine_.Update();
	predictionPoint_.Update();

}

void Enemy::Draw()
{
	if (worldTransform_.position.z > 0) {
		
		predictionLine_.Draw();
		predictionPoint_.Draw();
	}
	worldTransform_.Draw();
}

float Enemy::easeIn(float x)
{
	return 1 - cos((x * PI) / 2);
}

Vector3 Enemy::GetWorldPosition()
{
	//���[���h���W������ϐ�
	Vector3 worldPos;
	//���[���h�s��̕��s�ړ�����
	worldPos.x = worldTransform_.matWorld.m[3][0];
	worldPos.y = worldTransform_.matWorld.m[3][1];
	worldPos.z = worldTransform_.matWorld.m[3][2];

	return worldPos;
}

void Enemy::OnCollision(bool isBreak)
{
	isDead_ = true;
}

void Enemy::SetPlayer(Player* player) {
	player_ = player;
}