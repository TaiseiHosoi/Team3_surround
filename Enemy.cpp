#include "Enemy.h"
#include <cmath>
#include "math.h"
#define PI 3.141592653589

void Enemy::Initialize(Model* model,Vector3 vector3, float kBulSpeed)
{
	// NULL�|�C���^�`�F�b�N
	assert(model);
	model_ = model;
	worldTransform_.SetModel(model);

	//�V���O���C���X�^���X���擾����
	input_ = Input::GetInstance();

	//�������W���Z�b�g
	worldTransform_.Initialize(false);
	worldTransform_.position = vector3;
	worldTransform_.scale = { 1,1,1 };
	worldTransform_.rotation = { 0,0,0 };



	kEnemySpeedZ = kBulSpeed;
}

void Enemy::Update()
{

	worldTransform_.Update();

	//�m�[�c�̑��x�̏���
	kEnemySpeedZ += kEnemySpeedAcc;
	Vector3 kEnemySpeed = { 0.0f,0.0f,-1.0f };
	worldTransform_.position += kEnemySpeed;
	worldTransform_.Update();

}

void Enemy::Draw()
{
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