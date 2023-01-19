#pragma once
#include "DirectXCommon.h"
#include "SpriteCommon.h"
#include "Sprite.h"
#include "Object3d.h"
#include "Audio.h"
#include "Model.h"
#include "Player.h"

class GameScene {
public:
	/// <summary>
	/// �R���X�g�N���^
	/// </summary>
	GameScene();

	/// <summary>
	/// �f�X�g���N�^
	/// </summary>
	~GameScene();

	/// <summary>
	/// ������
	/// </summary>
	void Initialize(DirectXCommon* dxcomon);

	/// <summary>
	/// ���t���[������
	/// </summary>
	void Update();

	/// <summary>
	/// �`��
	/// </summary>
	void Draw();

private:
	DirectXCommon* dxCommon_ = nullptr;
	
	Audio* audio = nullptr;
	SpriteCommon* spritecommon = nullptr;
	Sprite* sprite = nullptr;
	Sprite* sprite2 = nullptr;
	Sprite* sprite3 = nullptr;
	Object3d* object3d = nullptr;
	Model* model = nullptr;

	Input* input_ = nullptr;

	//���\�[�X
	Model* whiteCube = nullptr;

	//�V�[���C���X�^���X
	Player* player_ = nullptr;
	
};