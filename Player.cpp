#include "Player.h"
#include <cmath>
#include "math.h"
#include"MathFunc.h"
#define PI 3.141592653589

void(Player::* Player::moveFuncTable[])() = {
	&Player::Turn,
	&Player::Straight,
	&Player::ATK1
};

enum Class {
	turn,//0
	straight,//1
	atk1//2
};

void Player::Initialize(Model* model, Model* cylinder)
{
	// NULL�|�C���^�`�F�b�N
	assert(model);
	model_ = model;
	cylinder_ = cylinder;

	//�V���O���g��
	input_ = Input::GetInstance();

	//�������W���Z�b�g
	worldTransform_.Initialize();
	worldTransform_.SetModel(model_);
	worldTransform_.position = { -5,0,0 };
	worldTransform_.scale = { 1,1,1 };
	worldTransform_.rotation = { 0,0.5 * PI,0 };
	pVelocity_ = { 0,0,0.4f };	//�v���C���[�̈ړ���
	movePhase = 1;//�s���t�F�[�Y������
	turnFlameCount = 0;	//��]���t���[���ڂ�

	nowLineWorldTransform_.Initialize();	//���@�̈ʒu
	nowLineWorldTransform_.SetModel(cylinder_);	

	//���@����t���[���J�E���g
	maxFlameCount_ = 70;
	nowFlameCount_ = 0;

	for (int i = 0; i < _countof(line_); i++) {
		line_[i].worldTransform.Initialize();
		line_[i].worldTransform.SetModel(cylinder_);
		line_[i].sLineVec2 = {};
		line_[i].eLineVec2 = {};
		line_[i].isDraw = false;
		line_[i].worldTransform.position.x += i * 2;
		line_[i].worldTransform.Update();
	}

	nextLine_ = 0;
	worldTransform_.Update();

}

void Player::Update()
{
#pragma region	�A�N�Z��
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

	

#pragma region ���@�ƃ��C���ۑ�
	nowEndPos = worldTransform_.position;//���C���p�̏I�_(���t���[���X�V)

	//line�̃g�����X�t�H�[���v�Z
	nowLineWorldTransform_.position = nowStartPos;//�n�_�I�_�̒��S�����W

	float len = sqrtf(pow(nowEndPos.x - nowStartPos.x, 2) +
		pow(nowEndPos.y - nowStartPos.y, 2) +
		pow(nowEndPos.z - nowStartPos.z, 2));

	nowLineWorldTransform_.scale = { 0.3,0.3,
		len /2
	};

	nowLineWorldTransform_.rotation = worldTransform_.rotation;


	Vector3 vel = MathFunc::bVelocity(pVelocity_, worldTransform_);

	worldTransform_.position += vel;

	nowFlameCount_++;

	

	if (nowFlameCount_ > maxFlameCount_) {	//����������90�x��]

		nowFlameCount_ = 0;
		isTurn = true;
		movePhase = turn;

	
	}
	(this->*moveFuncTable[movePhase])();	//�s���̊֐��e�[�u��
#pragma endregion ���@�ƃ��C���ۑ�



#pragma region �U��
	if (isAtk == true) {
		isAtk = false;
		for (int i = 0; i < _countof(line_); i++) {
			line_[i].isDraw = false;
		}
	}
#pragma endregion �U��

#pragma region ���[���h�g�����X�t�H�[���X�V
	for (int i = 0; i < _countof(line_); i++) {
		line_[i].worldTransform.Update();
	}
	worldTransform_.Update();
	nowLineWorldTransform_.Update();
#pragma endregion ���[���h�g�����X�t�H�[���X�V


}

void Player::Draw()
{

	worldTransform_.Draw();
	nowLineWorldTransform_.Draw();
	for (int i = 0; i < _countof(line_); i++) {
		if (line_[i].isDraw == true) {
			line_[i].worldTransform.Draw();
		}

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
	// �O���[�v�@
	Vector2 a_to_b = Vector2(line_abEnd.x - line_abStart.x, line_abEnd.y - line_abStart.y);
	Vector2 a_to_c = Vector2(line_cdStart.x - line_abStart.x, line_cdStart.y - line_abStart.y);
	Vector2 a_to_d = Vector2(line_cdEnd.x - line_abStart.x, line_cdEnd.y - line_abStart.y);

	// �O���[�v�A
	Vector2 c_to_d = Vector2(line_cdEnd.x - line_cdStart.x, line_cdEnd.y - line_cdStart.y);
	Vector2 c_to_a = Vector2(line_abStart.x - line_cdStart.x, line_abStart.y - line_cdStart.y);
	Vector2 c_to_b = Vector2(line_abEnd.x - line_cdStart.x, line_abEnd.y - line_cdStart.y);

	// �O���[�v�@�̊O��
	float d_01 = (a_to_b.x * a_to_c.y) - (a_to_c.x * a_to_b.y);
	float d_02 = (a_to_b.x * a_to_d.y) - (a_to_d.x * a_to_b.y);

	bool isColide = true;
	// ��Z���ʂ����Ȃ̂Ŏn�_�ƏI�_���܂������Ă��Ȃ�
	if (d_01 * d_02 > -2.0f)
	{
		// �O���[�v�A�̊O��
		d_01 = (c_to_d.x * c_to_a.y) - (c_to_a.x * c_to_d.y);
		d_02 = (c_to_d.x * c_to_b.y) - (c_to_b.x * c_to_d.y);

		// ��Z���ʂ����Ȃ̂Ŏn�_�ƏI�_���܂������Ă��Ȃ�
		if (d_01 * d_02 > -2.0f)
		{
			isColide = false;
		}

	}
	return isColide;
}

void Player::Turn()
{
	if (isTurn == true) {
		if (turnFlameCount == 0) {
			lineCount = 0;
			for (int i = 0; i < _countof(line_); i++) {	// ���C���ۑ�
				lineCount++;
				if (line_[i].isDraw == false) {
					line_[i].isDraw = true;
					line_[i].worldTransform.position = worldTransform_.position;
					line_[i].worldTransform.rotation = nowLineWorldTransform_.rotation;
					line_[i].worldTransform.scale = nowLineWorldTransform_.scale;

					line_[i].sLineVec2 = { nowStartPos.x, nowStartPos.x };
					line_[i].eLineVec2 = { nowEndPos.x, nowEndPos.x };

					break;
				}
			}
		}
		worldTransform_.rotation.x += 0.05f * PI;
		turnFlameCount++;
		if (turnFlameCount >= 10) {
			isTurn = false;
			turnFlameCount = 0;
		}
	}
	else {
		


#pragma region ���C���Ǝ��@�Փ�
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
#pragma endregion ���C���Ǝ��@�Փ�
		nowStartPos = nowEndPos;	// �I�_�����_�ɂȂ�
		movePhase = straight;
	}

}

void Player::Straight()
{
}

void Player::ATK1()
{
}
