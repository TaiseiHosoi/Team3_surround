#pragma once
#include"Error.h"
#include"input.h"
#include <wrl.h>
#include"Vector3.h"

#include <d3d12.h>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <DirectXmath.h>
#include <d3dcompiler.h>

#include<wrl.h>

#include "DirectXCommon.h"

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3dcompiler")

#include <string.h>

template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

//���_�f�[�^�\����
struct Vertex
{
	XMFLOAT3 pos; //xyz���W
	XMFLOAT3 normal;//�@���x�N�g��
	XMFLOAT2 uv;  //uv���W
};

//�萔�o�b�t�@�p�f�[�^�\���́i�}�e���A���j
struct ConstBufferDataMaterial {
	XMFLOAT4 color;	//�F(RGBA)
};

//�萔�o�b�t�@�p�f�[�^�\����
struct ConstBufferDataTransform {
	XMMATRIX mat;
};




struct Object3d {
	ComPtr<ID3D12Resource> constBuffTransform;
	ConstBufferDataTransform* constMapTransform = nullptr;

	XMFLOAT3 scale = { 1,1,1 };
	XMFLOAT3 rotation = { 0,0,0 };
	XMFLOAT3 position = { 0,0,0 };

	XMMATRIX matWorld{};

	Object3d* parent = nullptr;


};




class Mesh
{
public:
	// ���U���g
	HRESULT result;
	// ���_�o�b�t�@�r���[�̍쐬
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	// �O���t�B�b�N�X�p�C�v���C���ݒ�
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};
	// �C���f�b�N�X�o�b�t�@�r���[�̍쐬
	D3D12_INDEX_BUFFER_VIEW ibView{};
	// ���[�g�V�O�l�`��
	ID3D12RootSignature* rootSignature = nullptr;
	// �p�C�v�����X�e�[�g�̐���
	ID3D12PipelineState* pipelineState = nullptr;
	// �萔�o�b�t�@
	ID3D12Resource* constBuffMaterial = nullptr;
	// �萔�o�b�t�@�̃}�b�s���O
	ConstBufferDataMaterial* constMapMaterial = nullptr;

	//3D�I�u�W�F�N�g�̐�
	//static const size_t kObjectCount = 50;
	// 3D�I�u�W�F�N�g�̔z��
	Object3d object3d;

	//GPU��̃o�b�t�@�ɑΉ��������z������
	Vertex* vertMap = nullptr;
	// �ݒ������SRV�p�f�X�N���v�^�q�[�v�𐶐�
	ID3D12DescriptorHeap* srvHeap = nullptr;
	// 
	UINT incrementSize;

	// ���[���h�ϊ��s��
	XMMATRIX matWorld;
	XMMATRIX matWorld1;

	//���W
	XMFLOAT3 scale = { 1.0f,1.0f,1.0f };
	XMFLOAT3 position = { 0.0f,0.0f,0.0f };
	XMFLOAT3 rotation = { 0.0f,0.0f,0.0f };



	// �ˉe�ϊ��s��̌v�Z
	XMMATRIX matProjection;

	// �r���[�ϊ��s��
	XMMATRIX matView;
	XMFLOAT3 eye;	// ���_���W
	XMFLOAT3 target;	// �����_���W
	XMFLOAT3 up;		// ������x�N�g��

	//�J�����p�ϐ�
	float focalLengs = 200;
	float sensor = 24;
	float angle = 0.0f;

	Vector3 R = { 0.0f,0 ,0 };
	Vector3 G = { 0.5f,0 ,0 };
	Vector3 B = { 1.0f,0 ,0 };

	//���f���̃t�@�C���`��
	char fileType[5] = { 0 };

	// ���_�f�[�^�z��
	std::vector<Vertex> vertices;
	// ���_�C���f�b�N�X�z��
	std::vector<unsigned short> indices;
	


public:

	//// ���_�f�[�^
	//Vertex vertices[24] = {// �O
	//	{{ 0.0f, 0.0f,  0.0f},{}, {0.0f, 0.0f}/* ����*/},
	//	{{ 0.0f, 0.0f,  0.0f},{}, {0.0f, 0.0f}/* ����*/},
	//	{{ 0.0f, 0.0f,  0.0f},{}, {0.0f, 0.0f}/* �E��*/},
	//	{{ 0.0f, 0.0f,  0.0f},{}, {0.0f, 0.0f}/* �E��*/},
	//	// ���
	//	{{ 0.0f, 0.0f,  0.0f},{}, {0.0f, 0.0f}/* ����*/},
	//	{{ 0.0f, 0.0f,  0.0f},{}, {0.0f, 0.0f}/* ����*/},
	//	{{ 0.0f, 0.0f,  0.0f},{}, {0.0f, 0.0f}/* �E��*/},
	//	{{ 0.0f, 0.0f,  0.0f},{}, {0.0f, 0.0f}/* �E��*/},
	//	// ��
	//	{{ 0.0f, 0.0f,  0.0f},{}, {0.0f, 1.0f}/* ����*/},
	//	{{ 0.0f, 0.0f,  0.0f},{}, {0.0f, 0.0f}/* ����*/},
	//	{{ 0.0f, 0.0f,  0.0f},{}, {1.0f, 1.0f}/* �E��*/},
	//	{{ 0.0f, 0.0f,  0.0f},{}, {1.0f, 0.0f}/* �E��*/},
	//	// �E
	//	{{ 0.0f, 0.0f,  0.0f},{}, {0.0f, 1.0f}/* ����*/},
	//	{{ 0.0f, 0.0f,  0.0f},{}, {0.0f, 0.0f}/* ����*/},
	//	{{ 0.0f, 0.0f,  0.0f},{}, {1.0f, 1.0f}/* �E��*/},
	//	{{ 0.0f, 0.0f,  0.0f},{}, {1.0f, 0.0f}/* �E��*/},
	//	// ��
	//	{{ 0.0f, 0.0f,  0.0f},{}, {0.0f, 1.0f}/* ����*/},
	//	{{ 0.0f, 0.0f,  0.0f},{}, {0.0f, 0.0f}/* ����*/},
	//	{{ 0.0f, 0.0f,  0.0f},{}, {1.0f, 1.0f}/* �E��*/},
	//	{{ 0.0f, 0.0f,  0.0f},{}, {1.0f, 0.0f}/* �E��*/},
	//	// ��
	//	{{ 0.0f, 0.0f,  0.0f},{}, {0.0f, 1.0f}/* ����*/},
	//	{{ 0.0f, 0.0f,  0.0f},{}, {0.0f, 0.0f}/* ����*/},
	//	{{ 0.0f, 0.0f,  0.0f},{}, {1.0f, 1.0f}/* �E��*/},
	//	{{ 0.0f, 0.0f,  0.0f},{}, {1.0f, 0.0f}/* �E��*/},
	//};
	//// �C���f�b�N�X�f�[�^
	//uint16_t indices[36] = {
	//	0,1,2,
	//	2,1,3,

	//	6,5,4,
	//	5,6,7,

	//	8,9,10,
	//	10,9,11,

	//	14,13,12,
	//	13,14,15,

	//	16,17,18,
	//	18,17,19,

	//	22,21,20,
	//	21,22,23
	//};

public:

	Mesh(Vertex vertices[]);
	Mesh();
	~Mesh();

	void Init(ID3D12Device* device);
	void Update(ID3D12Device* device, Input* input);
	void Draw(ID3D12GraphicsCommandList* commandList);
	
	/// <summary>
	/// ���f���쐬
	/// </summary>
	static void CreateModel();

	void LoadModel(const char* fileName);
	void LoadModel();
};
