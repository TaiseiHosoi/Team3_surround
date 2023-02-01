#include "Player.h"
#include <cmath>
#include "math.h"
#include"MathFunc.h"
#include<vector>
#define PI 3.141592653589

void Player::Initialize(Model* model, Model* followModel, Model* playerModel)
{
	// NULLポインタチェック
	assert(model);
	assert(followModel);

	model_ = model;
	followModel_ = followModel;
	playerModel_ = playerModel;
	
	
	

	//シングルトン
	input_ = Input::GetInstance();

	//初期座標をセット
	worldTransform_.Initialize();
	worldTransform_.SetModel(playerModel_);
	worldTransform_.position = { -5,0,0 };
	worldTransform_.scale = { 1,1,1 };
	worldTransform_.rotation = { 0,0.5 * PI,0 };
	pVelocity_ = { 0,0,0.4f };	//プレイヤーの移動量

	nowLineWorldTransform_.Initialize(false);	//自機の位置
	nowLineWorldTransform_.SetModel(model_);
	nowLineWorldTransform_.Update();

	//自機旋回フレームカウント
	maxFlameCount_ = 70;
	nowFlameCount_ = 0;

	for (int i = 0; i < _countof(line_); i++) {
		line_[i].worldTransform.Initialize(false);
		line_[i].worldTransform.SetModel(model_);
		line_[i].sLineVec2 = {};
		line_[i].eLineVec2 = {};
		line_[i].isDraw = false;
		line_[i].worldTransform.scale = { 0.2,0.2,1.0f };
		line_[i].worldTransform.position.x += i * 2;
		line_[i].worldTransform.Update();

	}

	nextLine_ = 0;
	worldTransform_.Update();

	// フォロワー初期化
	for (int i = 0; i < _countof(followerWT_); i++) {
		followerWT_[i].Initialize();
		followerWT_[i].parent = &worldTransform_;
		followerWT_[i].scale = { 0.3f,0.3f,0.3f };
		followerWT_[i].SetModel(followModel_);
		followerWT_[i].Update();
	}
	followerPrimeAngle_ = 0.0f;

	//疑似壁
	for (int i = 0; i < _countof(perthLine); i++) {
		perthLine[i].Initialize();
		perthLine[i].SetModel(model_);
		perthLine[i].scale = { 0.2,0.2,300 };
		perthLine[i].rotation = { 0,0,0 };

		if (i == 0) {
			perthLine[0].position = { -15,-10,50 };
		}
		else if (i == 1) {
			perthLine[1].position = { 15,-10,50 };
		}
		else if (i == 2) {
			perthLine[2].position = { 15,10, 50 };
		}
		else if (i == 3) {
			perthLine[3].position = { -15,10,50 };
		}

		perthLine[i].Update();
	}

	//回転したときの角位置保存
	cornerPos_.resize(cornerPosCount_);
	maxPos = {};	//0
	minPos = {};

	//攻撃
	atkTransform_.Initialize();
	atkTransform_.SetModel(model_);
	isAtk = false;
	isAtkDraw = false;

	atkColide_.clear();

}

void Player::Update()
{
#pragma region	アクセル
	if (input_->PushKey(DIK_LEFT)) {
		if (maxFlameCount_ > 10) {
			maxFlameCount_--;
		}
	}
	else if (input_->PushKey(DIK_RIGHT)) {
		if (maxFlameCount_ < 100) {
			maxFlameCount_++;
		}
	}
#pragma endregion

#pragma region 自機とライン保存
	nowEndPos = worldTransform_.position;//ライン用の終点(毎フレーム更新)

	//lineのトランスフォーム計算
	nowLineWorldTransform_.position =	//始点終点の中心が座標
	{ (nowStartPos.x + nowEndPos.x) / 2,
		(nowStartPos.y + nowEndPos.y) / 2,
		(nowStartPos.z + nowEndPos.z) / 2
	};	//kmtEngineのオペレータがlib化されていていじれない

	float len = sqrtf(pow(nowEndPos.x - nowStartPos.x, 2) +
		pow(nowEndPos.y - nowStartPos.y, 2) +
		pow(nowEndPos.z - nowStartPos.z, 2));

	nowLineWorldTransform_.scale = { 0.3,0.3,
		len / 2
	};

	nowLineWorldTransform_.rotation = worldTransform_.rotation;


	Vector3 vel = MathFunc::bVelocity(pVelocity_, worldTransform_);

	worldTransform_.position += vel;

	nowFlameCount_++;
	if (nowFlameCount_ > maxFlameCount_) {	//時が来たら90度回転



		nowFlameCount_ = 0;
		worldTransform_.rotation.x += 0.5 * PI;

		int lineCount = 0;
		for (int i = 0; i < _countof(line_); i++) {	// ライン保存
			lineCount++;
			if (line_[i].isDraw == false) {
				line_[i].isDraw = true;
				line_[i].worldTransform.position = nowLineWorldTransform_.position;
				line_[i].worldTransform.rotation = nowLineWorldTransform_.rotation;
				line_[i].worldTransform.scale = nowLineWorldTransform_.scale;

				line_[i].sLineVec2 = { nowStartPos.x, nowStartPos.x };
				line_[i].eLineVec2 = { nowEndPos.x, nowEndPos.x };

				break;
			}
		}


#pragma region ラインと自機衝突
		if (lineCount >= 4) {
			for (int i = 0; i < _countof(line_); i++) {
				if (line_[i].isDraw == true) {
					if (LineColide(Vector2(nowStartPos.x, nowStartPos.y),
						Vector2(nowEndPos.x, nowEndPos.y),
						Vector2(line_[i].sLineVec2.x, line_[i].sLineVec2.y),
						Vector2(line_[i].eLineVec2.x, line_[i].eLineVec2.y))
						== true) {
						isAtk = true;
					}
				}


			}
		}
#pragma endregion ラインと自機衝突
		nowStartPos = nowEndPos;	// 終点が視点になる

		//配列を増やす
		cornerPos_.push_back(worldTransform_);
		cornerPosCount_ = cornerPos_.size();

	}
#pragma endregion 自機とライン保存



#pragma region 攻撃
	if (isAtk == true) {
		isAtk = false;
		isAtkDraw = true;
		for (int i = 0; i < _countof(line_); i++) {
			line_[i].isDraw = false;
		}

		for (int i = 0; i < std::end(cornerPos_) - std::begin(cornerPos_); ++i) {
			if (cornerPos_[i].position.x > maxPos.x) {
				maxPos.x = cornerPos_[i].position.x;
			}
			if (cornerPos_[i].position.x < minPos.x) {
				minPos.x = cornerPos_[i].position.x;
			}

			if (cornerPos_[i].position.y> maxPos.y) {
				maxPos.y = cornerPos_[i].position.y;
			}
			if (cornerPos_[i].position.y < minPos.y) {
				minPos.y = cornerPos_[i].position.y;
			}
		}

		cornerPos_.clear();
		atkTransform_.position = { minPos.x + (maxPos.x - minPos.x) / 2.0f,minPos.y + (maxPos.y - minPos.y) / 2.0f,10.0f };
		atkTransform_.scale = { (maxPos.x - minPos.x) / 2.0f,(maxPos.y - minPos.y) / 2.0f,20.0f };

		saveMinColide = minPos;
		saveMaxColide = maxPos;

		minPos = { worldTransform_.position.x,worldTransform_.position.y };
		maxPos = { worldTransform_.position.x,worldTransform_.position.y };

		atkColide nowAtkColide = { 120,true,atkTransform_ };

		atkColide_.push_back(nowAtkColide);

		for (int i = 0; i < std::end(atkColide_) - std::begin(atkColide_); ++i) {
			atkColide_[i].atkTransform.SetModel(model_);
		}
		
	}

	for (int i = 0; i < std::end(atkColide_) - std::begin(atkColide_); ++i) {
		if (atkColide_[i].isColide == true) {
			atkColide_[i].aliveCount--;
			if (atkColide_[i].aliveCount <= 0) {
				atkColide_[i].isColide = false;
			}
		}

	}

	if (isAtkDraw == true) {
		atkTransform_.position.z += 3.0f;	//velocity
		if (atkTransform_.position.z > 120) {

		}
	}


#pragma endregion 攻撃

#pragma region フォロワー
	float kFollowerAngleSpeed = MathFunc::Ease::In(1,20,nowFlameCount_,maxFlameCount_);
	kFollowerAngleSpeed /= 100.0f;
	followerPrimeAngle_ += kFollowerAngleSpeed;

	float followerLen = MathFunc::Ease::In(40,8, nowFlameCount_, maxFlameCount_);
	followerLen /= 10.0f;
	
	followerWT_[0].position = { sinf(followerPrimeAngle_ * PI) * followerLen,cosf(followerPrimeAngle_ * PI) * followerLen,-1 };
	followerWT_[1].position = { sinf(followerPrimeAngle_ * PI) * -followerLen,cosf(followerPrimeAngle_ * PI) * -followerLen,-1 };
	
#pragma endregion フォロワー

#pragma region ワールドトランスフォーム更新
	for (int i = 0; i < _countof(line_); i++) {
		line_[i].worldTransform.Update();
	}

	worldTransform_.Update();
	nowLineWorldTransform_.Update();
	for (int i = 0; i < _countof(followerWT_); i++) {
		followerWT_[i].Update();
	}

	atkTransform_.Update();

#pragma endregion ワールドトランスフォーム更新


}

void Player::Draw()
{

	//パース
	for (int i = 0; i < _countof(perthLine); i++) {
		perthLine[i].Draw();
	}

	worldTransform_.Draw();
	nowLineWorldTransform_.Draw();
	
	for (int i = 0; i < _countof(line_); i++) {
		if (line_[i].isDraw == true) {
			line_[i].worldTransform.Draw();
		}
	}

	for (int i = 0; i < _countof(followerWT_); i++) {
		followerWT_[i].Draw();
	}

	//atk
	if (isAtkDraw == true) {
		atkTransform_.Draw();
	}

	for (int i = 0; i < std::end(atkColide_) - std::begin(atkColide_); ++i) {
		if (atkColide_[i].isColide == true) {
			atkColide_[i].atkTransform.Draw();
		}
	}


}

Vector3 Player::GetWorldPosition()
{
	return Vector3();
}

Vector2 Player::GetAtkMinColidion()
{
	return saveMinColide;
}

Vector2 Player::GetAtkMaxColidion()
{
	return saveMaxColide;
}

std::vector<atkColide> Player::GetAtkColide()
{
	return atkColide_;
}







bool Player::GetIsAtkDraw()
{
	return isAtkDraw;
}

void Player::OnCollision(bool isBreak)
{
	//hp--;
}

bool Player::LineColide(Vector2 line_abStart, Vector2 line_abEnd, Vector2 line_cdStart, Vector2 line_cdEnd)
{
	// グループ①
	Vector2 a_to_b = Vector2(line_abEnd.x - line_abStart.x, line_abEnd.y - line_abStart.y);
	Vector2 a_to_c = Vector2(line_cdStart.x - line_abStart.x, line_cdStart.y - line_abStart.y);
	Vector2 a_to_d = Vector2(line_cdEnd.x - line_abStart.x, line_cdEnd.y - line_abStart.y);

	// グループ②
	Vector2 c_to_d = Vector2(line_cdEnd.x - line_cdStart.x, line_cdEnd.y - line_cdStart.y);
	Vector2 c_to_a = Vector2(line_abStart.x - line_cdStart.x, line_abStart.y - line_cdStart.y);
	Vector2 c_to_b = Vector2(line_abEnd.x - line_cdStart.x, line_abEnd.y - line_cdStart.y);

	// グループ①の外積
	float d_01 = (a_to_b.x * a_to_c.y) - (a_to_c.x * a_to_b.y);
	float d_02 = (a_to_b.x * a_to_d.y) - (a_to_d.x * a_to_b.y);

	bool isColide = true;
	// 乗算結果が正なので始点と終点がまたがっていない
	if (d_01 * d_02 > -2.0f)
	{
		// グループ②の外積
		d_01 = (c_to_d.x * c_to_a.y) - (c_to_a.x * c_to_d.y);
		d_02 = (c_to_d.x * c_to_b.y) - (c_to_b.x * c_to_d.y);

		// 乗算結果が正なので始点と終点がまたがっていない
		if (d_01 * d_02 > -2.0f)
		{
			isColide = false;
		}

	}
	return isColide;
}