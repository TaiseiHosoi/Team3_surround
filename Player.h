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

typedef struct atkColide {
	int aliveCount;
	bool isColide;
	Object3d atkTransform;
	
};

class Player
{
public:
	void Initialize(Model* model,Model* followModel,Model* playerModel);
	void Update();
	void Draw();

	bool IsDead() const { return isDead_; }	//死亡時

	////ワールド座標を取得
	Vector3 GetWorldPosition();
	Vector2 GetAtkMinColidion();
	Vector2 GetAtkMaxColidion();
	std::vector<atkColide> GetAtkColide();	//可変長配列
	int GetMaxTimeCount();
	int GetNowTimeCount();
	


	bool GetIsAtkDraw();

	//衝突を検出したら呼び出されるコールバック関数
	void OnCollision(bool isBreak);

public://計算
	bool LineColide(Vector2 line_abStart, Vector2 line_abEnd, Vector2 line_cdStart, Vector2 line_cdEnd);


private:
	//ワールド変換データ
	Object3d worldTransform_;
	Object3d followerWT_[2];	//周りにくっついてるやつのトランスフォーム

	//モデル
	Model* model_ = nullptr;
	Model* followModel_ = nullptr;
	Model* playerModel_ = nullptr;

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
	size_t cornerPosCount_ = 0;
	std::vector<Object3d> cornerPos_;
	Vector2 maxPos;
	Vector2 minPos;

	Vector3 pVelocity_ = {};


	//デスフラグ
	bool isDead_ = false;

	int maxFlameCount_;	//プレイヤーが曲がるまでの挙動
	int nowFlameCount_;

	//攻撃
	bool isAtkDraw = false;
	bool isAtk = false;
	Object3d atkTransform_;
	Vector2 saveMinColide;
	Vector2 saveMaxColide;

	

	std::vector<atkColide> atkColide_;

	//フォロワー関連
	float followerPrimeAngle_;

	//疑似壁(パースライン)
	Object3d perthLine[4] = {};


	
};