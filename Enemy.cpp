#include "Enemy.h"
#include <cmath>
#include "math.h"
#define PI 3.141592653589

void Enemy::Initialize(Model* redCube, Model* model,Vector3 vector3, float kBulSpeed, int howColor)
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
	//color
	XMFLOAT4 blueColor = { 0.2f,1.0f,0.8f,1.0f };
	XMFLOAT4 pinkColor = { 1.0f,0.1f,1.0f,1.0f };
	
	worldTransform_.Initialize(true);
	howColor_ = howColor;

	if (howColor_ == 1) {
		worldTransform_.SetRimColor(blueColor);
	}
	else if (howColor_ == 2) {
		worldTransform_.SetRimColor(pinkColor);
	}
	else {
		worldTransform_.SetRimColor({ 1.0,1.0,1.0,1.0 });
	}

	worldTransform_.SetRimEmission(100.0f);
	worldTransform_.position = vector3;
	worldTransform_.scale = { 1,1,1 };
	worldTransform_.rotation = { 0,0,0 };

	//予測線
	predictionLine_.Initialize(false);
	predictionLine_.position = vector3;
	predictionLine_.scale = { 0.3f,0.3f,1.0f };
	predictionLine_.rotation = { 0,0,0 };
	predictionLine_.Update();

	//予測ポイント
	predictionPoint_.Initialize(false);
	predictionPoint_.position = { vector3.x,vector3.y,0 };
	predictionPoint_.scale = { 0.7f,0.7f,0.2f };
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

int Enemy::GetColorNum()
{

	return howColor_;
}
