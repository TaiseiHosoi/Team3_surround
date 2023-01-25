#include "Enemy.h"
#include <cmath>
#include "math.h"
#define PI 3.141592653589

void Enemy::Initialize(Model* model,Vector3 vector3, float kBulSpeed)
{
	// NULLポインタチェック
	assert(model);
	model_ = model;
	worldTransform_.SetModel(model);

	//シングルインスタンスを取得する
	input_ = Input::GetInstance();

	//初期座標をセット
	worldTransform_.Initialize(false);
	worldTransform_.position = vector3;
	worldTransform_.scale = { 1,1,1 };
	worldTransform_.rotation = { 0,0,0 };



	kEnemySpeedZ = kBulSpeed;
}

void Enemy::Update()
{

	worldTransform_.Update();

	//ノーツの速度の処理
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
	//ワールド座標を入れる変数
	Vector3 worldPos;
	//ワールド行列の平行移動成分
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