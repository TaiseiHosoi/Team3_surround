#pragma once
#include<cassert>
#include"Matrix4.h"
#include"stdlib.h"
#include "Model.h"
#include "Input.h"
#include"Vector2.h"
#include"Vector3.h"
#include"Object3d.h"

typedef struct Line {
	Vector2 sLineVec2;	//始点
	Vector2 eLineVec2;	//終点
	Object3d worldTransform;
	bool isDraw;

};

class Player
{
public:
	void Initialize(Model* model);
	void Update();
	void Draw();

	bool IsDead() const { return isDead_; }	//死亡時

	////ワールド座標を取得
	Vector3 GetWorldPosition();

	//衝突を検出したら呼び出されるコールバック関数
	void OnCollision(bool isBreak);

public://計算
	bool LineColide(Vector2 line_abStart, Vector2 line_abEnd, Vector2 line_cdStart, Vector2 line_cdEnd);


private:
	//ワールド変換データ
	Object3d worldTransform_;

	//モデル
	Model* model_ = nullptr;

	//その他
	uint32_t textureHandle_ = 0u;
	Input* input_ = nullptr;

	//過去のライン
	//Object3d lineWorldTransform_[10];
	Line line_[100];

	int nextLine_ = 0;

	//現在地ライン
	Vector3 nowStartPos = {};	//現在のライン保存用
	Vector3 nowEndPos{};	//
	Object3d nowLineWorldTransform_;

	Vector3 pVelocity_ = {};


	//デスフラグ
	bool isDead_ = false;

	int maxFlameCount_;	//プレイヤーが曲がるまでの挙動
	int nowFlameCount_;

	//攻撃
	bool isAtk = false;


};