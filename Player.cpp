#include "Player.h"
#include <cmath>
#include "math.h"
#define PI 3.141592653589

void Player::Initialize(Model* model, uint32_t textureHandle)
{
	// NULLポインタチェック
	assert(model);
	model_ = model;
	textureHandle_ = textureHandle;

	//シングルトン
	input_ = Input::GetInstance();

	//初期座標をセット
	worldTransform_.Initialize();
	worldTransform_. = { -5,0,0 };
	worldTransform_.scale_ = { 1,1,1 };
	worldTransform_.rotation_ = { 0,0.5 * PI,0 };
	pVelocity_ = { 0,0,0.4f };	//プレイヤーの移動量

	nowLineWorldTransform_.Initialize();	//自機の位置

	//自機旋回フレームカウント
	maxFlameCount_ = 70;
	nowFlameCount_ = 0;

	for (int i = 0; i < _countof(line_); i++) {
		line_[i].worldTransform.Initialize();
		line_[i].sLineVec2 = {};
		line_[i].eLineVec2 = {};
		line_[i].isDraw = false;
		line_[i].worldTransform.translation_.x += i * 2;
		worldTransformUpdate(&line_[i].worldTransform);

	}

	nextLine_ = 0;
	worldTransformUpdate(&worldTransform_);

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
	nowEndPos = worldTransform_.translation_;//ライン用の終点(毎フレーム更新)

	//lineのトランスフォーム計算
	nowLineWorldTransform_.translation_ =	//始点終点の中心が座標
	{ (nowStartPos.x + nowEndPos.x) / 2,
		(nowStartPos.y + nowEndPos.y) / 2,
		(nowStartPos.z + nowEndPos.z) / 2
	};	//kmtEngineのオペレータがlib化されていていじれない

	float len = sqrtf(pow(nowEndPos.x - nowStartPos.x, 2) +
		pow(nowEndPos.y - nowStartPos.y, 2) +
		pow(nowEndPos.z - nowStartPos.z, 2));

	nowLineWorldTransform_.scale_ = { 0.3,0.3,
		len / 2
	};

	nowLineWorldTransform_.rotation_ = worldTransform_.rotation_;


	Vector3 vel = bVelocity(pVelocity_, worldTransform_);

	worldTransform_.translation_ += vel;

	nowFlameCount_++;
	if (nowFlameCount_ > maxFlameCount_) {	//時が来たら90度回転



		nowFlameCount_ = 0;
		worldTransform_.rotation_.x += 0.5 * PI;

		int lineCount = 0;
		for (int i = 0; i < _countof(line_); i++) {	// ライン保存
			lineCount++;
			if (line_[i].isDraw == false) {
				line_[i].isDraw = true;
				line_[i].worldTransform.translation_ = nowLineWorldTransform_.translation_;
				line_[i].worldTransform.rotation_ = nowLineWorldTransform_.rotation_;
				line_[i].worldTransform.scale_ = nowLineWorldTransform_.scale_;

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

	}
#pragma endregion 自機とライン保存



#pragma region 攻撃
	if (isAtk == true) {
		isAtk = false;
		for (int i = 0; i < _countof(line_); i++) {
			line_[i].isDraw = false;
		}
	}
#pragma endregion 攻撃

#pragma region ワールドトランスフォーム更新
	for (int i = 0; i < _countof(line_); i++) {
		worldTransformUpdate(&line_[i].worldTransform);
	}
	worldTransformUpdate(&worldTransform_);
	worldTransformUpdate(&nowLineWorldTransform_);
#pragma endregion ワールドトランスフォーム更新


}

void Player::Draw(ViewProjection viewProjection)
{

	model_->Draw(worldTransform_, viewProjection, textureHandle_);
	model_->Draw(nowLineWorldTransform_, viewProjection, textureHandle_);
	for (int i = 0; i < _countof(line_); i++) {
		if (line_[i].isDraw == true) {
			model_->Draw(line_[i].worldTransform, viewProjection, textureHandle_);
		}

		//トランスレーション
		debugText_->SetPos(20, 20 + i * 15);
		debugText_->Printf("%f,%f,%f", line_[i].worldTransform.translation_.x
			, line_[i].worldTransform.translation_.y
			, line_[i].worldTransform.translation_.z);

		//スケール
		debugText_->SetPos(300, 20 + i * 15);
		debugText_->Printf("%f,%f,%f", line_[i].worldTransform.scale_.x
			, line_[i].worldTransform.scale_.y
			, line_[i].worldTransform.scale_.z);

	}


}

Vector3 Player::GetWorldPosition()
{
	return Vector3();
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