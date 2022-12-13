#include "Mesh.h"

#include "Error.h"
#include <math.h>
#include"WinApp.h"
#include"MathFunc.h"


Mesh::Mesh() {

}
Mesh::~Mesh() {

}
//Mesh::Mesh(Vertex vertices[]) {
//
//	for (int i = 0; i < this->vertices.size(); i++) {
//		this->vertices[i] = vertices[i];	//座標をコピー
//	}
//
//}

Mesh::Material Mesh::material;

// 3Dオブジェクト初期化
void InitializeObject3d(Object3d* object, ID3D12Device* device);
// 3Dオブジェクト更新
void UpdateObject3d(Object3d* object, XMMATRIX& matView, XMMATRIX& matProjection);
// 3Dオブジェクト描画
void DrawObject3d(Object3d* object, ID3D12GraphicsCommandList* commandList, D3D12_VERTEX_BUFFER_VIEW& vbView, D3D12_INDEX_BUFFER_VIEW& ibView, UINT numIndices);



void Mesh::Init(ID3D12Device* device) {


	//頂点データの全体サイズ　＝　頂点データ一つ分のサイズ　*　頂点データの要素数
	UINT sizeVB = static_cast<UINT>(sizeof(vertices[0]) * vertices.size());
	//頂点バッファの設定
	D3D12_HEAP_PROPERTIES heapProp{};		//ヒープ設定
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;	//GPUへの転送用
	//リソース設定
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeVB;	//頂点データの全体のサイズ
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//頂点バッファの生成
	ID3D12Resource* vertBuff = nullptr;
	result = device->CreateCommittedResource(
		&heapProp,	//ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&resDesc,	//リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));
	assert(SUCCEEDED(result));

	//法線の計算
	for (int i = 0; i < indices.size() / 3; i++) {

		//三角形のインデックスを取り出して一時的な変数に入れる
		unsigned short index0 = indices[i * 3 + 0];
		unsigned short index1 = indices[i * 3 + 1];
		unsigned short index2 = indices[i * 3 + 2];

		//三角形を構成する頂点座標をベクトルに代入
		XMVECTOR p0 = XMLoadFloat3(&vertices[index0].pos);
		XMVECTOR p1 = XMLoadFloat3(&vertices[index1].pos);
		XMVECTOR p2 = XMLoadFloat3(&vertices[index2].pos);

		//p0→p1ベクトル、p0→p2ベクトルを計算(ベクトルの減算)
		XMVECTOR v1 = XMVectorSubtract(p1, p0);
		XMVECTOR v2 = XMVectorSubtract(p2, p0);

		//外積は両方から垂直なベクトル
		XMVECTOR norm = XMVector3Cross(v1, v2);

		//正規化
		norm = XMVector3Normalize(norm);

		//求めた法線を頂点データに代入
		XMStoreFloat3(&vertices[index0].normal, norm);
		XMStoreFloat3(&vertices[index1].normal, norm);
		XMStoreFloat3(&vertices[index2].normal, norm);

	}



	//GPU上のバッファに対応した仮想メモリ（メインメモリ上）を取得
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(result));

	//全頂点に対して
	for (int i = 0; i < vertices.size(); i++) {
		vertMap[i] = vertices[i];	//座標をコピー
	}
	//繋がりを解除
	vertBuff->Unmap(0, nullptr);

	//GPU仮想アドレス
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	//頂点バッファのサイズ
	vbView.SizeInBytes = sizeVB;
	//頂点一つ分のデータサイズ
	vbView.StrideInBytes = sizeof(vertices[0]);
	ID3DBlob* vsBlob = nullptr;		//頂点シェーダオブジェクト
	ID3DBlob* psBlob = nullptr;		//ピクセルシェーダオブジェクト
	ID3DBlob* errorBlob = nullptr;	//エラーオブジェクト

	//頂点シェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"ObjVS.hlsl",	//シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,	//インクルード可能にする
		"main", "vs_5_0",	//エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,	//デバッグ用設定
		0,
		&vsBlob, &errorBlob);

	// エラーなら
	if (FAILED(result)) {
		// errorBlobからエラー内容をstring型にコピー
		std::string error;
		error.resize(errorBlob->GetBufferSize());
		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			error.begin());
		error += "\n";
		// エラー内容を出力ウィンドウに表示
		OutputDebugStringA(error.c_str());
		assert(0);
	}

	// ピクセルシェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"ObjPS.hlsl", // シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"main", "ps_5_0", // エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
		0,
		&psBlob, &errorBlob);

	// エラーなら
	if (FAILED(result)) {
		// errorBlobからエラー内容をstring型にコピー
		std::string error;
		error.resize(errorBlob->GetBufferSize());
		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			error.begin());
		error += "\n";
		// エラー内容を出力ウィンドウに表示
		OutputDebugStringA(error.c_str());
		assert(0);
	}
	// 頂点レイアウト
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
	{ // xyz(1行で書いたほうが見やすい)
	"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
	D3D12_APPEND_ALIGNED_ELEMENT,
	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	},
	{ // 法線ベクトル(1行で書いたほうが見やすい)
		"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
	},
	{ //uv座標(1行で書いたほうが見やすい)
	"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
	D3D12_APPEND_ALIGNED_ELEMENT,
	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	},
	};

	// インデックスデータ全体のサイズ
	UINT sizeIB = static_cast<UINT>(sizeof(uint16_t) * indices.size());

	// リソース設定
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeIB;			// インデックス情報が入る分のサイズ
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// インデックスバッファの生成
	ID3D12Resource* indexBuff = nullptr;
	result = device->CreateCommittedResource(
		&heapProp, // ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&resDesc, // リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff));

	// インデックスバッファをマッピング
	uint16_t* indexMap = nullptr;
	result = indexBuff->Map(0, nullptr, (void**)&indexMap);
	// 全インデックスに対して
	//for (int i = 0; i < indices.size(); i++)
	//{
	//	indexMap[i] = indices[i];   // インデックスをコピー
	//}
	std::copy(indices.begin(), indices.end(), indexMap);

	//編集コマンド
	

	// マッピング解除
	indexBuff->Unmap(0, nullptr);

	// インデックスバッファビューの作成
	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeIB;



	// グラフィックスパイプライン設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};
	// シェーダーの設定
	pipelineDesc.VS.pShaderBytecode = vsBlob->GetBufferPointer();
	pipelineDesc.VS.BytecodeLength = vsBlob->GetBufferSize();
	pipelineDesc.PS.pShaderBytecode = psBlob->GetBufferPointer();
	pipelineDesc.PS.BytecodeLength = psBlob->GetBufferSize();

	// サンプルマスクの設定
	pipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定

	// ラスタライザの設定
	pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE; // カリングしない
	//pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK; // 背面のカリングする
	pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID; // ポリゴン内塗りつぶし
	//pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME; // ワイヤーフレーム
	pipelineDesc.RasterizerState.DepthClipEnable = true; // 深度クリッピングを有効に

	//// ブレンドステート
	//	pipelineDesc.BlendState.RenderTarget[0].RenderTargetWriteMask
	//		= D3D12_COLOR_WRITE_ENABLE_ALL;


	// レンダーターゲットのブレンド設定
	D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = pipelineDesc.BlendState.RenderTarget[0];
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;// RBGA全てのチャンネルを描画

	//共通設定
	blenddesc.BlendEnable = true;					// ブレンドを有効にする
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;	// 加算
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;		// ソースの値を100％ 使う
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;	// デストの値を  0％ 使う

	////加算合成
	//blenddesc.BlendOp = D3D12_BLEND_OP_ADD;		// 加算
	//blenddesc.SrcBlend = D3D12_BLEND_ONE;		// ソースの値を100％ 使う
	//blenddesc.DestBlend = D3D12_BLEND_ONE;		// ソースの値を100％ 使う

	//// 減算合成
	//blenddesc.BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;    // デストからソースを減算
	//blenddesc.SrcBlend = D3D12_BLEND_ONE;               // ソースの値を100% 使う
	//blenddesc.DestBlend = D3D12_BLEND_ONE;              // デストの値を100% 使う

	//// 色反転
	//blenddesc.BlendOp = D3D12_BLEND_OP_ADD;             // 加算
	//blenddesc.SrcBlend = D3D12_BLEND_INV_DEST_COLOR;    // 1.0f-デストカラーの値
	//blenddesc.DestBlend = D3D12_BLEND_ZERO;             // 使わない

	// 半透明合成
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;             // 加算
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;         // ソースのアルファ値
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;    // 1.0f-ソースのアルファ値


	// 頂点レイアウトの設定
	pipelineDesc.InputLayout.pInputElementDescs = inputLayout;
	pipelineDesc.InputLayout.NumElements = _countof(inputLayout);


	// 図形の形状設定
	pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	// その他の設定
	pipelineDesc.NumRenderTargets = 1; // 描画対象は1つ
	pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0~255指定のRGBA
	pipelineDesc.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング

#pragma region デスクリプタレンジの設定

	// デスクリプタレンジの設定
	D3D12_DESCRIPTOR_RANGE descriptorRange{};
	descriptorRange.NumDescriptors = 1;         //一度の描画に使うテクスチャが1枚なので1
	descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange.BaseShaderRegister = 0;     //テクスチャレジスタ0番
	descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;


#pragma endregion デスクリプタレンジ

	// ルートパラメータの設定
	D3D12_ROOT_PARAMETER rootParams[3] = {};
	// 定数バッファ0番
	rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;   // 種類
	rootParams[0].Descriptor.ShaderRegister = 0;                   // 定数バッファ番号
	rootParams[0].Descriptor.RegisterSpace = 0;                    // デフォルト値
	rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;  // 全てのシェーダから見える
	// テクスチャレジスタ0番
	rootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;   //種類
	rootParams[1].DescriptorTable.pDescriptorRanges = &descriptorRange;		  //デスクリプタレンジ
	rootParams[1].DescriptorTable.NumDescriptorRanges = 1;              		  //デスクリプタレンジ数
	rootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;               //全てのシェーダから見える
	// 定数バッファ1番
	rootParams[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;   // 種類
	rootParams[2].Descriptor.ShaderRegister = 1;                   // 定数バッファ番号
	rootParams[2].Descriptor.RegisterSpace = 0;                    // デフォルト値
	rootParams[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;  // 全てのシェーダから見える
	



	// テクスチャサンプラーの設定
	D3D12_STATIC_SAMPLER_DESC samplerDesc{};
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;                 //横繰り返し（タイリング）
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;                 //縦繰り返し（タイリング）
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;                 //奥行繰り返し（タイリング）
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;  //ボーダーの時は黒
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;                   //全てリニア補間
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;                                 //ミップマップ最大値
	samplerDesc.MinLOD = 0.0f;                                              //ミップマップ最小値
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;           //ピクセルシェーダからのみ使用可能



	// ルートシグネチャの設定
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureDesc.pParameters = rootParams; //ルートパラメータの先頭アドレス
	rootSignatureDesc.NumParameters = _countof(rootParams);        //ルートパラメータ数
	rootSignatureDesc.pStaticSamplers = &samplerDesc;
	rootSignatureDesc.NumStaticSamplers = 1;

	// ルートシグネチャのシリアライズ
	ID3DBlob* rootSigBlob = nullptr;
	result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob, &errorBlob);
	assert(SUCCEEDED(result));
	result = device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(result));
	rootSigBlob->Release();
	// パイプラインにルートシグネチャをセット
	pipelineDesc.pRootSignature = rootSignature;

	// デプスステンシルステートの設定
	pipelineDesc.DepthStencilState.DepthEnable = true;							// 深度テストを行う
	pipelineDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;	// 書き込み許可
	pipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	pipelineDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;								// 深度値フォーマット

	// パイプランステートの生成
	result = device->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&pipelineState));
	assert(SUCCEEDED(result));

	// ヒープ設定
	D3D12_HEAP_PROPERTIES cbHeapProp{};
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;                   // GPUへの転送用
	// リソース設定
	D3D12_RESOURCE_DESC cbResourceDesc{};
	cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbResourceDesc.Width = (sizeof(ConstBufferDataMaterial) + 0xff) & ~0xff;   // 256バイトアラインメント
	cbResourceDesc.Height = 1;
	cbResourceDesc.DepthOrArraySize = 1;
	cbResourceDesc.MipLevels = 1;
	cbResourceDesc.SampleDesc.Count = 1;
	cbResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;


	// 定数バッファの生成
	result = device->CreateCommittedResource(
		&cbHeapProp, // ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&cbResourceDesc, // リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffMaterial));
	assert(SUCCEEDED(result));

	// 定数バッファのマッピング
	result = constBuffMaterial->Map(0, nullptr, (void**)&constMapMaterial); // マッピング
	assert(SUCCEEDED(result));

	{

		// ヒープ設定
		D3D12_HEAP_PROPERTIES cbHeapProp{};
		cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;                   // GPUへの転送用
		// リソース設定
		D3D12_RESOURCE_DESC cbResourceDesc{};
		cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		cbResourceDesc.Width = (sizeof(ConstBufferDataMaterial) + 0xff) & ~0xff;   // 256バイトアラインメント
		cbResourceDesc.Height = 1;
		cbResourceDesc.DepthOrArraySize = 1;
		cbResourceDesc.MipLevels = 1;
		cbResourceDesc.SampleDesc.Count = 1;
		cbResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		
		// 初期化
		InitializeObject3d(&object3d, device);

#pragma region view
		//平行投影変換
			// 単位行列を代入
		/*constMapTransform0->mat = XMMatrixIdentity();

		constMapTransform0->mat = XMMatrixOrthographicOffCenterLH(
			2.0f / window_width, window_width,
			window_height, -2.0f / window_height,
			0.0f, 1.0f
		);
		constMapTransform->mat.r[0].m128_f32[0] = 2.0f/window_width;
		constMapTransform->mat.r[1].m128_f32[1] = -2.0f / window_height;
		constMapTransform->mat.r[3].m128_f32[0] = -1.0f;
		constMapTransform->mat.r[3].m128_f32[1] = 1.0f;*/

		////透視投影変換
		//	// 射影変換行列の計算
		//matProjection =
		//	XMMatrixPerspectiveFovLH(
		//		CameraHorizon(distans),				// 上下画角45度
		//		(float)window_width / window_height,	// アスペクト比 （ 画面横幅/画面縦幅 ）
		//		0.1f, 1000.0f							// 前端、奥端
		//	);


		//// ビュー変換行列

		//eye = { 0, 0, -100 };	// 視点座標
		//target = { object3ds[0].position.x, object3ds[0].position.y, object3ds[0].position.z };	// 注視点座標
		//up = { 0, 1, 0 };		// 上方向ベクトル
		//matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));]
#pragma endregion

	}

#pragma region 画像イメージデータの作成


	TexMetadata metadata{};
	ScratchImage scratchImg{};
	// WICテクスチャのロード
	result = LoadFromWICFile(
		L"Resources/mario.png",
		WIC_FLAGS_NONE,
		&metadata, scratchImg);

	// ２枚目用に別の変数を用意しておく
	TexMetadata metadata2{};
	ScratchImage scratchImg2{};
	// WICテクスチャのロード
	result = LoadFromWICFile(
		L"Resources/reimu.png",
		WIC_FLAGS_NONE,
		&metadata2, scratchImg2);

	ScratchImage mipChain{};
	// ミップマップ生成
	result = GenerateMipMaps(
		scratchImg.GetImages(), scratchImg.GetImageCount(), scratchImg.GetMetadata(),
		TEX_FILTER_DEFAULT, 0, mipChain);
	if (SUCCEEDED(result)) {
		scratchImg = std::move(mipChain);
		metadata = scratchImg.GetMetadata();
	}

	// 読み込んだディフューズテクスチャをSRGBとして扱う
	metadata.format = MakeSRGB(metadata.format);

	ScratchImage mipChain2{};
	// ミップマップ生成
	result = GenerateMipMaps(
		scratchImg2.GetImages(), scratchImg2.GetImageCount(), scratchImg2.GetMetadata(),
		TEX_FILTER_DEFAULT, 0, mipChain2);
	if (SUCCEEDED(result)) {
		scratchImg2 = std::move(mipChain2);
		metadata2 = scratchImg2.GetMetadata();
	}

	// 読み込んだディフューズテクスチャをSRGBとして扱う
	metadata2.format = MakeSRGB(metadata2.format);


#pragma endregion 画像イメージデータの作成

#pragma region テクスチャバッファ
	// ヒープ設定
	D3D12_HEAP_PROPERTIES textureHeapProp{};
	textureHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	textureHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	textureHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

	// リソース設定
	D3D12_RESOURCE_DESC textureResourceDesc{};
	textureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureResourceDesc.Format = metadata.format;
	textureResourceDesc.Width = metadata.width;  // 幅
	textureResourceDesc.Height = (UINT)metadata.height; // 高さ
	textureResourceDesc.DepthOrArraySize = (UINT)metadata.arraySize;
	textureResourceDesc.MipLevels = (UINT)metadata.mipLevels;
	textureResourceDesc.SampleDesc.Count = 1;

	// テクスチャバッファの生成
	ID3D12Resource* texBuff = nullptr;
	result = device->CreateCommittedResource(
		&textureHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&textureResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texBuff));

	// リソース設定
	D3D12_RESOURCE_DESC textureResourceDesc2{};
	textureResourceDesc2.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureResourceDesc2.Format = metadata2.format;
	textureResourceDesc2.Width = metadata2.width;  // 幅
	textureResourceDesc2.Height = (UINT)metadata2.height; // 高さ
	textureResourceDesc2.DepthOrArraySize = (UINT)metadata2.arraySize;
	textureResourceDesc2.MipLevels = (UINT)metadata2.mipLevels;
	textureResourceDesc2.SampleDesc.Count = 1;
	// テクスチャバッファの生成
	ID3D12Resource* texBuff2 = nullptr;
	result = device->CreateCommittedResource(
		&textureHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&textureResourceDesc2,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texBuff2));

	// 全ミップマップについて
	for (size_t i = 0; i < metadata.mipLevels; i++) {
		// ミップマップレベルを指定してイメージを取得
		const Image* img = scratchImg.GetImage(i, 0, 0);
		// テクスチャバッファにデータ転送
		result = texBuff->WriteToSubresource(
			(UINT)i,
			nullptr,              // 全領域へコピー
			img->pixels,          // 元データアドレス
			(UINT)img->rowPitch,  // 1ラインサイズ
			(UINT)img->slicePitch // 1枚サイズ
		);
		assert(SUCCEEDED(result));
	}
	// 全ミップマップについて
	for (size_t i = 0; i < metadata2.mipLevels; i++) {
		// ミップマップレベルを指定してイメージを取得
		const Image* img = scratchImg2.GetImage(i, 0, 0);
		// テクスチャバッファにデータ転送
		result = texBuff2->WriteToSubresource(
			(UINT)i,
			nullptr,              // 全領域へコピー
			img->pixels,          // 元データアドレス
			(UINT)img->rowPitch,  // 1ラインサイズ
			(UINT)img->slicePitch // 1枚サイズ
		);
		assert(SUCCEEDED(result));
	}


#pragma endregion テクスチャバッファ

#pragma region デスクリプターヒープ

	// SRVの最大個数
	const size_t kMaxSRVCount = 2056;

	// デスクリプタヒープの設定
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//シェーダから見えるように
	srvHeapDesc.NumDescriptors = kMaxSRVCount;

	// 設定を元にSRV用デスクリプタヒープを生成

	result = device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srvHeap));
	assert(SUCCEEDED(result));

	// SRVヒープを戦闘ハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE srvHandle = srvHeap->GetCPUDescriptorHandleForHeapStart();





#pragma endregion デスクリプターヒープ

#pragma region シェーダリソースビュー

	// シェーダリソースビュー設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};			// 設定構造体
	srvDesc.Format = resDesc.Format;	//
	srvDesc.Shader4ComponentMapping =
		D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;// 2Dテクスチャ
	srvDesc.Texture2D.MipLevels = resDesc.MipLevels;

	// ハンドルの指す位置にシェーダーリソースビュー作成
	device->CreateShaderResourceView(texBuff, &srvDesc, srvHandle);

	// 一つハンドルを進める
	incrementSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	srvHandle.ptr += incrementSize;

	// シェーダーリソースビュー設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc2{};
	srvDesc2.Format = textureResourceDesc2.Format;
	srvDesc2.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc2.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc2.Texture2D.MipLevels = textureResourceDesc2.MipLevels;

	//ハンドルの指す位置にシェーダーリソースビュー作成
	device->CreateShaderResourceView(texBuff2, &srvDesc2, srvHandle);

}

void Mesh::Update(ID3D12Device* device, Input* input) {
	// NULLポインタチェック
	assert(input);


	


	//透視投影変換
		//射影変換行列の計算
	matProjection =
		XMMatrixPerspectiveFovLH(
			MathFunc::FieldOfViewY(focalLengs, sensor),
			(float)WinApp::window_width / WinApp::window_height,	// アスペクト比 （ 画面横幅/画面縦幅 ）
			0.1f, 1500.0f// 前端、奥端
		);
	// ビュー変換行列

	eye = { 0,0,1000 };	// 視点座標
	target = { -20,0,0 };	// 注視点座標
	up = { 0, 1, 0 };		// 上方向ベクトル
	matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));

	if (input->PushKey(DIK_D)) {
		object3d.rotation.y += 0.1f;
	}
	
		UpdateObject3d(&object3d, matView, matProjection);
		
	


	// 値を書き込むと自動的に転送される
	constMapMaterial->color = XMFLOAT4(R.x, G.x, B.x, 1.0f);	// RGBAで半透明の赤



}

void Mesh::Draw(ID3D12GraphicsCommandList* commandList) {


	// パイプラインステートとルートシグネチャの設定コマンド
	commandList->SetPipelineState(pipelineState);
	commandList->SetGraphicsRootSignature(rootSignature);

	// プリミティブ形状の設定コマンド
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // 三角形リスト
	//commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP); // 三角形ストリップ

	// 頂点バッファビューの設定コマンド
	commandList->IASetVertexBuffers(0, 1, &vbView);

	// 定数バッファビュー(CBV)の設定コマンド
	commandList->SetGraphicsRootConstantBufferView(0, constBuffMaterial->GetGPUVirtualAddress());

	// SRVヒープの設定コマンド
	commandList->SetDescriptorHeaps(1, &srvHeap);
	// SRVヒープの先頭ハンドルを取得（SRVを指しているはず）
	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle = srvHeap->GetGPUDescriptorHandleForHeapStart();
	// SRVヒープの先頭にあるSRVをルートパラメータ1番に設定
	commandList->SetGraphicsRootDescriptorTable(1, srvGpuHandle);

	// 2枚目を指し示すようにしたsrvのハンドルをルートパラメータ1番に設定
	srvGpuHandle.ptr += incrementSize;
	commandList->SetGraphicsRootDescriptorTable(1, srvGpuHandle);


	// インデックスバッファビューの設定コマンド
	commandList->IASetIndexBuffer(&ibView);

	// 描画コマンド (頂点バッファのみ)

	//commandList->DrawInstanced(vertices.size(), 1, 0, 0); // 全ての頂点を使って描画

	// 全オブジェクトについて処理
	DrawObject3d(&object3d, commandList, vbView, ibView, indices.size());
	

}

void Mesh::CreateModel()
{
}

void Mesh::LoadModel(const char* fileName) {

	if (fileName) {

		//開きたいファイルの拡張子を取得
		size_t fileNameLen = strlen(fileName);
		strcpy_s(fileType, fileName + fileNameLen - 4);


#pragma region//.objファイルの場合
		if (strcmp(fileType, ".obj") == 0) {

			//ファイルストリーム
			std::fstream file;
			//.objファイルを開く
			file.open(fileName);
			//ファイルオープン失敗をチェック
			if (file.fail()) {
				assert(0);
			}

			std::vector<XMFLOAT3>positions;	//頂点座標
			std::vector<XMFLOAT3>normals;	//法線ベクトル
			std::vector<XMFLOAT2>texcoords;	//テクスチャUV
			//1行ずつ読み込む
			std::string line;
			while (getline(file, line)) {

				//1行分の文字列をストリームに変換して解析しやすくする
				std::istringstream line_stream(line);

				//半角スペース区切りで行の先頭文字を取得
				std::string key;
				getline(line_stream, key, ' ');

				//先頭文字列がvなら頂点座標
				if (key == "v") {

					//X,Y,Z座標読み込み
					XMFLOAT3 position{};
					line_stream >> position.x;
					line_stream >> position.y;
					line_stream >> position.z;

					//座標データに追加
					positions.emplace_back(position);

				}

				//先頭文字列がvtならテクスチャ
				if (key == "vt") {
					//U,V成分読み込み
					XMFLOAT2 texcoord{};
					line_stream >> texcoord.x;
					line_stream >> texcoord.y;

					//V方向反転
					texcoord.y = 1.0f - texcoord.y;

					//テクスチャ座標データに追加
					texcoords.emplace_back(texcoord);

				}

				if (key == "vn") {
					//X,Y,Z成分読み込み
					XMFLOAT3 normal{};
					line_stream >> normal.x;
					line_stream >> normal.y;
					line_stream >> normal.z;

					//法線ベクトルデータに追加
					normals.emplace_back(normal);

				}

				//先頭文字列がfならポリゴン(三角形)
				if (key == "f") {

					//半角スペース区切りで行の続きを読み込む
					std::string index_string;
					while (getline(line_stream, index_string, ' ')) {

						//頂点インデックス1個分の文字列をストリームに変換して解析しやすくする
						std::istringstream index_stream(index_string);
						unsigned short indexPosition, indexNormal, indexTexcoord;
						index_stream >> indexPosition;
						index_stream.seekg(1, std::ios_base::cur);
						index_stream >> indexTexcoord;
						index_stream.seekg(1, std::ios_base::cur);
						index_stream >> indexNormal;

						Vertex vertex{};
						vertex.pos = positions[indexPosition - 1];
						vertex.normal = normals[indexNormal - 1];
						vertex.uv = texcoords[indexTexcoord - 1];
						vertices.emplace_back(vertex);

						//インデックスデータの追加
						indices.emplace_back((unsigned short)indices.size());
					}
				}
			}
			file.close();
		}
#pragma endregion
		else {
			//対応していないモデルはエラーを吐く
			assert(nullptr);
		}
	}
	else {
	LoadModel();
	}
}

void Mesh::LoadModel() {

	//頂点データ
	vertices = {
		//前
		{{-5.0f , -5.0f , -5.0f} , {} , {0.0f , 1.0f}} ,//左下 インデックス0
		{{-5.0f , 5.0f , -5.0f} , {} , {0.0f , 0.0f}} ,//左上 インデックス1
		{{5.0f , -5.0f , -5.0f} , {} , {1.0f , 1.0f}} ,//右下 インデックス2
		{{5.0f , 5.0f , -5.0f} , {} , {1.0f , 0.0f}} ,//右上 インデックス3
		//後
		{{-5.0f , -5.0f , 5.0f} , {} , {0.0f , 1.0f}} ,//左下 インデックス0
		{{-5.0f , 5.0f , 5.0f} , {} , {0.0f , 0.0f}} ,//左上 インデックス1
		{{5.0f , -5.0f , 5.0f} , {} , {1.0f , 1.0f}} ,//右下 インデックス2
		{{5.0f , 5.0f , 5.0f} , {} , {1.0f , 0.0f}} ,//右上 インデックス3
		//左
		{{-5.0f , -5.0f , -5.0f} , {} , {0.0f , 1.0f}} ,//左下 インデックス0
		{{-5.0f , -5.0f , 5.0f} , {} , {0.0f , 0.0f}} ,//左上 インデックス1
		{{-5.0f , 5.0f , -5.0f} , {} , {1.0f , 1.0f}} ,//右下 インデックス2
		{{-5.0f , 5.0f , 5.0f} , {} , {1.0f , 0.0f}} ,//右上 インデックス3
		//右
		{{5.0f , -5.0f , -5.0f} , {} , {0.0f , 1.0f}} ,//左下 インデックス0
		{{5.0f , -5.0f , 5.0f} , {} , {0.0f , 0.0f}} ,//左上 インデックス1
		{{5.0f , 5.0f , -5.0f} , {} , {1.0f , 1.0f}} ,//右下 インデックス2
		{{5.0f , 5.0f , 5.0f} , {} , {1.0f , 0.0f}} ,//右上 インデックス3
		//上
		{{-5.0f , 5.0f , -5.0f} , {} , {0.0f , 1.0f}} ,//左下 インデックス0
		{{5.0f , 5.0f , -5.0f} , {} , {0.0f , 0.0f}} ,//左上 インデックス1
		{{-5.0f , 5.0f , 5.0f} , {} , {1.0f , 1.0f}} ,//右下 インデックス2
		{{5.0f , 5.0f , 5.0f} , {} , {1.0f , 0.0f}} ,//右上 インデックス3
		//下
		{{-5.0f , -5.0f , -5.0f} , {} , {0.0f , 1.0f}} ,//左下 インデックス0
		{{5.0f , -5.0f , -5.0f} , {} , {0.0f , 0.0f}} ,//左上 インデックス1
		{{-5.0f , -5.0f , 5.0f} , {} , {1.0f , 1.0f}} ,//右下 インデックス2
		{{5.0f , -5.0f , 5.0f} , {} , {1.0f , 0.0f}} ,//右上 インデックス3

	};

	//インデックスデータ
	indices = {
		//前
		0 , 1 , 2 , //三角形1つ目
		1 , 3 , 2 , //三角形2つ目
		//後ろ
		4 , 6 , 5 ,
		5 , 6 , 7 ,
		//左
		8 , 9 , 10 ,
		9 , 11 , 10 ,
		//右
		12 , 14 , 13 ,
		13 , 14 , 15 ,
		//上
		16 , 18 , 17 ,
		17 , 18 , 19 ,
		//下
		20 , 21 , 22 ,
		21 , 23 , 22 ,
	};

}


void InitializeObject3d(Object3d* object, ID3D12Device* device) {

	HRESULT result;

	// 定数バッファのヒープ設定
	D3D12_HEAP_PROPERTIES heapProp{};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	// 定数バッファのリソース設定
	D3D12_RESOURCE_DESC resdesc{};
	resdesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resdesc.Width = (sizeof(ConstBufferDataTransform) + 0xff) & ~0xff;
	resdesc.Height = 1;
	resdesc.DepthOrArraySize = 1;
	resdesc.MipLevels = 1;
	resdesc.SampleDesc.Count = 1;
	resdesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// 定数バッファの生成
	result = device->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resdesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&object->constBuffB0));
	assert(SUCCEEDED(result));

	// 定数バッファのマッピング
	result = object->constBuffB0->Map(0, nullptr, (void**)&object->constMapTransform);
	assert(SUCCEEDED(result));

	// B1用
	resdesc = CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB1) + 0xff) &~0xff);
	// 定数バッファの生成
	result = device->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resdesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&object->constBuffB1)
	);



}

void UpdateObject3d(Object3d* object, XMMATRIX& matView, XMMATRIX& matProjection) {
	XMMATRIX matScale, matRot, matTrans;

	// スケール、回転、平行移動行列の計算
	matScale = XMMatrixScaling(object->scale.x, object->scale.y, object->scale.z);
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(object->rotation.z);
	matRot *= XMMatrixRotationX(object->rotation.x);
	matRot *= XMMatrixRotationY(object->rotation.y);
	matTrans = XMMatrixTranslation(object->position.x, object->position.y, object->position.z);

	// ワールド行列の合成
	object->matWorld = XMMatrixIdentity();	// 変形をリセット
	object->matWorld *= matScale;			// ワールド行列にスケーリングを反映
	object->matWorld *= matRot;				// ワールド行列に回転を反映
	object->matWorld *= matTrans;			// ワールド行列に平行移動を反映

	// 親オブジェクトがあれば
	if (object->parent != nullptr) {
		// 親オブジェクトのワールド行列をかける
		object->matWorld *= object->parent->matWorld;
	}

	// 定数バッファへデータ転送
	object->constMapTransform->mat = object->matWorld * matView * matProjection;

	//B1
	HRESULT result;
	ConstBufferDataB1* constMap1 = nullptr;
	result = object->constBuffB1->Map(0, nullptr, (void**)&constMap1);
	constMap1->ambient = Mesh::material.ambient;
	constMap1->ambient = Mesh::material.diffuse;
	constMap1->ambient = Mesh::material.specular;
	constMap1->alpha = Mesh::material.alpha;
	object->constBuffB1->Unmap(0, nullptr);
	


}

// 3Dオブジェクト描画
void DrawObject3d(
	Object3d* object,
	ID3D12GraphicsCommandList* commandList,
	D3D12_VERTEX_BUFFER_VIEW& vbView,
	D3D12_INDEX_BUFFER_VIEW& ibView,
	UINT numIndices)
{
	// 頂点バッファの設定
	commandList->IASetVertexBuffers(0, 1, &vbView);
	// インデックスバッファの設定
	commandList->IASetIndexBuffer(&ibView);
	// 定数バッファビュー（CBV）の設定コマンド
	commandList->SetGraphicsRootConstantBufferView(2, object->constBuffB0->GetGPUVirtualAddress());
	// 描画コマンド
	commandList->DrawIndexedInstanced(numIndices, 1, 0, 0, 0);

}
