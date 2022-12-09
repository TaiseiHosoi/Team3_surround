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

//頂点データ構造体
struct Vertex
{
	XMFLOAT3 pos; //xyz座標
	XMFLOAT3 normal;//法線ベクトル
	XMFLOAT2 uv;  //uv座標
};

//定数バッファ用データ構造体（マテリアル）
struct ConstBufferDataMaterial {
	XMFLOAT4 color;	//色(RGBA)
};

//定数バッファ用データ構造体
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
	// リザルト
	HRESULT result;
	// 頂点バッファビューの作成
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	// グラフィックスパイプライン設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};
	// インデックスバッファビューの作成
	D3D12_INDEX_BUFFER_VIEW ibView{};
	// ルートシグネチャ
	ID3D12RootSignature* rootSignature = nullptr;
	// パイプランステートの生成
	ID3D12PipelineState* pipelineState = nullptr;
	// 定数バッファ
	ID3D12Resource* constBuffMaterial = nullptr;
	// 定数バッファのマッピング
	ConstBufferDataMaterial* constMapMaterial = nullptr;

	//3Dオブジェクトの数
	//static const size_t kObjectCount = 50;
	// 3Dオブジェクトの配列
	Object3d object3d;

	//GPU上のバッファに対応した仮想メモリ
	Vertex* vertMap = nullptr;
	// 設定を元にSRV用デスクリプタヒープを生成
	ID3D12DescriptorHeap* srvHeap = nullptr;
	// 
	UINT incrementSize;

	// ワールド変換行列
	XMMATRIX matWorld;
	XMMATRIX matWorld1;

	//座標
	XMFLOAT3 scale = { 1.0f,1.0f,1.0f };
	XMFLOAT3 position = { 0.0f,0.0f,0.0f };
	XMFLOAT3 rotation = { 0.0f,0.0f,0.0f };



	// 射影変換行列の計算
	XMMATRIX matProjection;

	// ビュー変換行列
	XMMATRIX matView;
	XMFLOAT3 eye;	// 視点座標
	XMFLOAT3 target;	// 注視点座標
	XMFLOAT3 up;		// 上方向ベクトル

	//カメラ用変数
	float focalLengs = 200;
	float sensor = 24;
	float angle = 0.0f;

	Vector3 R = { 0.0f,0 ,0 };
	Vector3 G = { 0.5f,0 ,0 };
	Vector3 B = { 1.0f,0 ,0 };

	//モデルのファイル形式
	char fileType[5] = { 0 };

	// 頂点データ配列
	std::vector<Vertex> vertices;
	// 頂点インデックス配列
	std::vector<unsigned short> indices;
	


public:

	//// 頂点データ
	//Vertex vertices[24] = {// 前
	//	{{ 0.0f, 0.0f,  0.0f},{}, {0.0f, 0.0f}/* 左下*/},
	//	{{ 0.0f, 0.0f,  0.0f},{}, {0.0f, 0.0f}/* 左上*/},
	//	{{ 0.0f, 0.0f,  0.0f},{}, {0.0f, 0.0f}/* 右下*/},
	//	{{ 0.0f, 0.0f,  0.0f},{}, {0.0f, 0.0f}/* 右上*/},
	//	// 後ろ
	//	{{ 0.0f, 0.0f,  0.0f},{}, {0.0f, 0.0f}/* 左下*/},
	//	{{ 0.0f, 0.0f,  0.0f},{}, {0.0f, 0.0f}/* 左上*/},
	//	{{ 0.0f, 0.0f,  0.0f},{}, {0.0f, 0.0f}/* 右下*/},
	//	{{ 0.0f, 0.0f,  0.0f},{}, {0.0f, 0.0f}/* 右上*/},
	//	// 左
	//	{{ 0.0f, 0.0f,  0.0f},{}, {0.0f, 1.0f}/* 左下*/},
	//	{{ 0.0f, 0.0f,  0.0f},{}, {0.0f, 0.0f}/* 左上*/},
	//	{{ 0.0f, 0.0f,  0.0f},{}, {1.0f, 1.0f}/* 右下*/},
	//	{{ 0.0f, 0.0f,  0.0f},{}, {1.0f, 0.0f}/* 右上*/},
	//	// 右
	//	{{ 0.0f, 0.0f,  0.0f},{}, {0.0f, 1.0f}/* 左下*/},
	//	{{ 0.0f, 0.0f,  0.0f},{}, {0.0f, 0.0f}/* 左上*/},
	//	{{ 0.0f, 0.0f,  0.0f},{}, {1.0f, 1.0f}/* 右下*/},
	//	{{ 0.0f, 0.0f,  0.0f},{}, {1.0f, 0.0f}/* 右上*/},
	//	// 下
	//	{{ 0.0f, 0.0f,  0.0f},{}, {0.0f, 1.0f}/* 左下*/},
	//	{{ 0.0f, 0.0f,  0.0f},{}, {0.0f, 0.0f}/* 左上*/},
	//	{{ 0.0f, 0.0f,  0.0f},{}, {1.0f, 1.0f}/* 右下*/},
	//	{{ 0.0f, 0.0f,  0.0f},{}, {1.0f, 0.0f}/* 右上*/},
	//	// 上
	//	{{ 0.0f, 0.0f,  0.0f},{}, {0.0f, 1.0f}/* 左下*/},
	//	{{ 0.0f, 0.0f,  0.0f},{}, {0.0f, 0.0f}/* 左上*/},
	//	{{ 0.0f, 0.0f,  0.0f},{}, {1.0f, 1.0f}/* 右下*/},
	//	{{ 0.0f, 0.0f,  0.0f},{}, {1.0f, 0.0f}/* 右上*/},
	//};
	//// インデックスデータ
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
	/// モデル作成
	/// </summary>
	static void CreateModel();

	void LoadModel(const char* fileName);
	void LoadModel();
};
