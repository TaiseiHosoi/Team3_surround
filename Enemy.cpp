#include "Enemy.h"
#include <cmath>
#include "math.h"
#define PI 3.141592653589

void Enemy::Initialize(Model* redCube, Model* model,Vector3 vector3, float kBulSpeed)
{
	// NULLポインタチェック
	assert(model);
	model_ = model;
	redCube_ = redCube;
	worldTransform_.SetModel(model);
	predictionLine_.SetModel(redCube_);
	predictionPoint_.SetModel(redCube_);

	//シングルインスタンスを取得する
	input_ = Input::GetInstance();

	//初期座標をセット
	worldTransform_.Initialize(false);
	worldTransform_.position = vector3;
	worldTransform_.scale = { 1,1,1 };
	worldTransform_.rotation = { 0,0,0 };

	//予測線
	predictionLine_.Initialize(true);
	predictionLine_.position = vector3;
	predictionLine_.scale = { 0.1f,0.1f,1.0f };
	predictionLine_.rotation = { 0,0,0 };
	predictionLine_.Update();

	//予測ポイント
	predictionPoint_.Initialize(false);
	predictionPoint_.position = { vector3.x,vector3.y,0 };
	predictionPoint_.scale = { 0.4f,0.4f,0.1f };
	predictionPoint_.rotation = { 0,0,0 };
	predictionPoint_.Update();

	kEnemySpeedZ = kBulSpeed;
}

void Enemy::Update()
{

	worldTransform_.Update();

	//ノーツの速度の処理
	kEnemySpeedZ += kEnemySpeedAcc;
	Vector3 kEnemySpeed = { 0.0f,0.0f,-1.0f };
	worldTransform_.position += kEnemySpeed;

	//予測線
	predictionLine_.position.z = worldTransform_.position.z / 2.0f;
	predictionLine_.scale.z = (float)abs(worldTransform_.position.z) / 2.0f;
	predictionPoint_.position.z = 0.0f;

	worldTransform_.Update();
	predictionLine_.Update();
	predictionPoint_.Update();

}

void Enemy::Draw()
{
	worldTransform_.Draw();
	predictionLine_.Draw();
	predictionPoint_.Draw();
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