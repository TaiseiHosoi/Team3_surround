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

	bool IsDead() const { return isDead_; }	//���S��

	////���[���h���W���擾
	Vector3 GetWorldPosition();
	Vector2 GetAtkMinColidion();
	Vector2 GetAtkMaxColidion();
	std::vector<atkColide> GetAtkColide();	//�ϒ��z��
	int GetMaxTimeCount();
	int GetNowTimeCount();
	


	bool GetIsAtkDraw();

	//�Փ˂����o������Ăяo�����R�[���o�b�N�֐�
	void OnCollision(bool isBreak);

public://�v�Z
	bool LineColide(Vector2 line_abStart, Vector2 line_abEnd, Vector2 line_cdStart, Vector2 line_cdEnd);


private:
	//���[���h�ϊ��f�[�^
	Object3d worldTransform_;
	Object3d followerWT_[2];	//����ɂ������Ă��̃g�����X�t�H�[��

	//���f��
	Model* model_ = nullptr;
	Model* followModel_ = nullptr;
	Model* playerModel_ = nullptr;

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
	size_t cornerPosCount_ = 0;
	std::vector<Object3d> cornerPos_;
	Vector2 maxPos;
	Vector2 minPos;

	Vector3 pVelocity_ = {};


	//�f�X�t���O
	bool isDead_ = false;

	int maxFlameCount_;	//�v���C���[���Ȃ���܂ł̋���
	int nowFlameCount_;

	//�U��
	bool isAtkDraw = false;
	bool isAtk = false;
	Object3d atkTransform_;
	Vector2 saveMinColide;
	Vector2 saveMaxColide;

	

	std::vector<atkColide> atkColide_;

	//�t�H�����[�֘A
	float followerPrimeAngle_;

	//�^����(�p�[�X���C��)
	Object3d perthLine[4] = {};


	
};