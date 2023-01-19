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
	Vector2 sLineVec2;	//�n�_
	Vector2 eLineVec2;	//�I�_
	Object3d worldTransform;
	bool isDraw;

};

class Player
{
public:
	void Initialize(Model* model);
	void Update();
	void Draw();

	bool IsDead() const { return isDead_; }	//���S��

	////���[���h���W���擾
	Vector3 GetWorldPosition();

	//�Փ˂����o������Ăяo�����R�[���o�b�N�֐�
	void OnCollision(bool isBreak);

public://�v�Z
	bool LineColide(Vector2 line_abStart, Vector2 line_abEnd, Vector2 line_cdStart, Vector2 line_cdEnd);


private:
	//���[���h�ϊ��f�[�^
	Object3d worldTransform_;

	//���f��
	Model* model_ = nullptr;

	//���̑�
	uint32_t textureHandle_ = 0u;
	Input* input_ = nullptr;

	//�ߋ��̃��C��
	//Object3d lineWorldTransform_[10];
	Line line_[100];

	int nextLine_ = 0;

	//���ݒn���C��
	Vector3 nowStartPos = {};	//���݂̃��C���ۑ��p
	Vector3 nowEndPos{};	//
	Object3d nowLineWorldTransform_;

	Vector3 pVelocity_ = {};


	//�f�X�t���O
	bool isDead_ = false;

	int maxFlameCount_;	//�v���C���[���Ȃ���܂ł̋���
	int nowFlameCount_;

	//�U��
	bool isAtk = false;


};