#pragma once
#include "Input.h"
#include "Model.h"
#include "Object3d.h"
#include <list>
#include <cassert>


//前方宣言
#include"Player.h"

class GameScene;

class Enemy
{
public:
	void Initialize(Model* redCube,Model* model, Vector3 vector3, float kBulSpeed);
	void Update();
	void Draw();

	bool IsDead() const { return isDead_; }	//死亡時
	int GetId() { return enemyId_; }
	void SetID(int ID) { enemyId_ = ID; }

	//弾のイージング
	float easeIn(float x);

	////ワールド座標を取得
	Vector3 GetWorldPosition();

	//衝突を検出したら呼び出されるコールバック関数
	void OnCollision(bool isBreak);

public:
	void SetPlayer(Player* player);

private:
	//ワールド変換データ
	Object3d worldTransform_;
	Object3d predictionLine_;
	Object3d predictionPoint_;

	//モデル
	Model* model_ = nullptr;
	Model* redCube_ = nullptr;

	//その他
	uint32_t textureHandle_ = 0u;
	Input* input_ = nullptr;

	//固有ID
	int enemyId_ = 0;

	//デスフラグ
	bool isDead_ = false;

	float depth = 40.0f;	//奥行
	float xDifference = 10.0f;	//左右差

	//ノーツの速度
	float kEnemySpeedZ = 0.4;
	//ノーツの加速度
	float kEnemySpeedAcc = 0.003;

	//現在のレーン
	//Lane lane_;

	//フィールド取得用
	int fieldLane_ = 0;

	Player* player_ = nullptr;
	

};

