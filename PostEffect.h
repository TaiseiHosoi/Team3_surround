#pragma once
#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>


class PostEffect
{
private: // エイリアス
// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public: // サブクラス
	/// <summary>
	/// 頂点データ構造体
	/// </summary>
	struct VertexPosUv
	{
		XMFLOAT3 pos; // xyz座標
		XMFLOAT2 uv;  // uv座標
	};

	/// <summary>
	/// 定数バッファ用データ構造体
	/// </summary>
	struct ConstBufferData
	{
		XMFLOAT4 color;	// 色 (RGBA)
		XMMATRIX mat;	// ３Ｄ変換行列
	};

	// 頂点数
	static const int vertNum = 4;

public:
	
	/// <summary>
	/// コンストラクタ
	/// </summary>
	PostEffect();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(ID3D12Device* device);

	/// <summary>
	/// 描画コマンドの発行
	/// </summary>
	/// <param name="cmdList">コマンドリスト</param>
	void Draw(ID3D12GraphicsCommandList* cmdList);

	/// <summary>
	/// シーン描画前処理
	/// </summary>
	/// <param name="cmdList"></param>
	void PreDrawScene(ID3D12GraphicsCommandList* cmdList);

	/// <summary>
	/// シーン描画後処理
	/// </summary>
	/// <param name="cmdList"></param>
	void PostDrawScene(ID3D12GraphicsCommandList* cmdList);

	/// <summary>
	/// パイプライン生成
	/// </summary>
	void CreateGraphicsPipelineState(ID3D12Device* device);

	//画面クリアカラー
	static const float clearColor[4];

private:
	//メンバ変数
	Microsoft::WRL::ComPtr<ID3D12Resource> texBuff;
	//SRV用のデスクリプタヒープ
	Microsoft::WRL::ComPtr <ID3D12DescriptorHeap> descHeapSRV;

	//深度バッファ
	Microsoft::WRL::ComPtr <ID3D12Resource> depthBuff;
	//RTV用デスクリプタヒープ
	Microsoft::WRL::ComPtr <ID3D12DescriptorHeap> descHeapRTV;
	//DSV用デスクリプタヒープ
	Microsoft::WRL::ComPtr <ID3D12DescriptorHeap> descHeapDSV;


	//グラフィックスパイプライン
	Microsoft::WRL::ComPtr <ID3D12PipelineState> pipelineState;
	//ルートシグネチャ
	Microsoft::WRL::ComPtr <ID3D12RootSignature> rootSignature;
	// デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descHeap;

	// 頂点バッファ
	ComPtr<ID3D12Resource> vertBuff;
	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	// 定数バッファ
	ComPtr<ID3D12Resource> constBuff;
	// ワールド行列
	XMMATRIX matWorld{};
	// Z軸回りの回転角
	float rotation = 0.0f;
	// 座標
	XMFLOAT2 position{};
	// 色
	XMFLOAT4 color = { 1, 1, 1, 1 };

	// テクスチャの最大枚数
	const int srvCount = 512;

};

