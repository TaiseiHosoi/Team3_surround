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
//		this->vertices[i] = vertices[i];	//���W���R�s�[
//	}
//
//}

Mesh::Material Mesh::material;

// 3D�I�u�W�F�N�g������
void InitializeObject3d(Object3d* object, ID3D12Device* device);
// 3D�I�u�W�F�N�g�X�V
void UpdateObject3d(Object3d* object, XMMATRIX& matView, XMMATRIX& matProjection);
// 3D�I�u�W�F�N�g�`��
void DrawObject3d(Object3d* object, ID3D12GraphicsCommandList* commandList, D3D12_VERTEX_BUFFER_VIEW& vbView, D3D12_INDEX_BUFFER_VIEW& ibView, UINT numIndices);



void Mesh::Init(ID3D12Device* device) {


	//���_�f�[�^�̑S�̃T�C�Y�@���@���_�f�[�^����̃T�C�Y�@*�@���_�f�[�^�̗v�f��
	UINT sizeVB = static_cast<UINT>(sizeof(vertices[0]) * vertices.size());
	//���_�o�b�t�@�̐ݒ�
	D3D12_HEAP_PROPERTIES heapProp{};		//�q�[�v�ݒ�
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;	//GPU�ւ̓]���p
	//���\�[�X�ݒ�
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeVB;	//���_�f�[�^�̑S�̂̃T�C�Y
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//���_�o�b�t�@�̐���
	ID3D12Resource* vertBuff = nullptr;
	result = device->CreateCommittedResource(
		&heapProp,	//�q�[�v�ݒ�
		D3D12_HEAP_FLAG_NONE,
		&resDesc,	//���\�[�X�ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));
	assert(SUCCEEDED(result));

	//�@���̌v�Z
	for (int i = 0; i < indices.size() / 3; i++) {

		//�O�p�`�̃C���f�b�N�X�����o���Ĉꎞ�I�ȕϐ��ɓ����
		unsigned short index0 = indices[i * 3 + 0];
		unsigned short index1 = indices[i * 3 + 1];
		unsigned short index2 = indices[i * 3 + 2];

		//�O�p�`���\�����钸�_���W���x�N�g���ɑ��
		XMVECTOR p0 = XMLoadFloat3(&vertices[index0].pos);
		XMVECTOR p1 = XMLoadFloat3(&vertices[index1].pos);
		XMVECTOR p2 = XMLoadFloat3(&vertices[index2].pos);

		//p0��p1�x�N�g���Ap0��p2�x�N�g�����v�Z(�x�N�g���̌��Z)
		XMVECTOR v1 = XMVectorSubtract(p1, p0);
		XMVECTOR v2 = XMVectorSubtract(p2, p0);

		//�O�ς͗������琂���ȃx�N�g��
		XMVECTOR norm = XMVector3Cross(v1, v2);

		//���K��
		norm = XMVector3Normalize(norm);

		//���߂��@���𒸓_�f�[�^�ɑ��
		XMStoreFloat3(&vertices[index0].normal, norm);
		XMStoreFloat3(&vertices[index1].normal, norm);
		XMStoreFloat3(&vertices[index2].normal, norm);

	}



	//GPU��̃o�b�t�@�ɑΉ��������z�������i���C����������j���擾
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(result));

	//�S���_�ɑ΂���
	for (int i = 0; i < vertices.size(); i++) {
		vertMap[i] = vertices[i];	//���W���R�s�[
	}
	//�q���������
	vertBuff->Unmap(0, nullptr);

	//GPU���z�A�h���X
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	//���_�o�b�t�@�̃T�C�Y
	vbView.SizeInBytes = sizeVB;
	//���_����̃f�[�^�T�C�Y
	vbView.StrideInBytes = sizeof(vertices[0]);
	ID3DBlob* vsBlob = nullptr;		//���_�V�F�[�_�I�u�W�F�N�g
	ID3DBlob* psBlob = nullptr;		//�s�N�Z���V�F�[�_�I�u�W�F�N�g
	ID3DBlob* errorBlob = nullptr;	//�G���[�I�u�W�F�N�g

	//���_�V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	result = D3DCompileFromFile(
		L"ObjVS.hlsl",	//�V�F�[�_�t�@�C����
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,	//�C���N���[�h�\�ɂ���
		"main", "vs_5_0",	//�G���g���[�|�C���g���A�V�F�[�_�[���f���w��
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,	//�f�o�b�O�p�ݒ�
		0,
		&vsBlob, &errorBlob);

	// �G���[�Ȃ�
	if (FAILED(result)) {
		// errorBlob����G���[���e��string�^�ɃR�s�[
		std::string error;
		error.resize(errorBlob->GetBufferSize());
		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			error.begin());
		error += "\n";
		// �G���[���e���o�̓E�B���h�E�ɕ\��
		OutputDebugStringA(error.c_str());
		assert(0);
	}

	// �s�N�Z���V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	result = D3DCompileFromFile(
		L"ObjPS.hlsl", // �V�F�[�_�t�@�C����
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // �C���N���[�h�\�ɂ���
		"main", "ps_5_0", // �G���g���[�|�C���g���A�V�F�[�_�[���f���w��
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�O�p�ݒ�
		0,
		&psBlob, &errorBlob);

	// �G���[�Ȃ�
	if (FAILED(result)) {
		// errorBlob����G���[���e��string�^�ɃR�s�[
		std::string error;
		error.resize(errorBlob->GetBufferSize());
		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			error.begin());
		error += "\n";
		// �G���[���e���o�̓E�B���h�E�ɕ\��
		OutputDebugStringA(error.c_str());
		assert(0);
	}
	// ���_���C�A�E�g
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
	{ // xyz(1�s�ŏ������ق������₷��)
	"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
	D3D12_APPEND_ALIGNED_ELEMENT,
	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	},
	{ // �@���x�N�g��(1�s�ŏ������ق������₷��)
		"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
	},
	{ //uv���W(1�s�ŏ������ق������₷��)
	"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
	D3D12_APPEND_ALIGNED_ELEMENT,
	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	},
	};

	// �C���f�b�N�X�f�[�^�S�̂̃T�C�Y
	UINT sizeIB = static_cast<UINT>(sizeof(uint16_t) * indices.size());

	// ���\�[�X�ݒ�
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeIB;			// �C���f�b�N�X��񂪓��镪�̃T�C�Y
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// �C���f�b�N�X�o�b�t�@�̐���
	ID3D12Resource* indexBuff = nullptr;
	result = device->CreateCommittedResource(
		&heapProp, // �q�[�v�ݒ�
		D3D12_HEAP_FLAG_NONE,
		&resDesc, // ���\�[�X�ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff));

	// �C���f�b�N�X�o�b�t�@���}�b�s���O
	uint16_t* indexMap = nullptr;
	result = indexBuff->Map(0, nullptr, (void**)&indexMap);
	// �S�C���f�b�N�X�ɑ΂���
	//for (int i = 0; i < indices.size(); i++)
	//{
	//	indexMap[i] = indices[i];   // �C���f�b�N�X���R�s�[
	//}
	std::copy(indices.begin(), indices.end(), indexMap);

	//�ҏW�R�}���h
	

	// �}�b�s���O����
	indexBuff->Unmap(0, nullptr);

	// �C���f�b�N�X�o�b�t�@�r���[�̍쐬
	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeIB;



	// �O���t�B�b�N�X�p�C�v���C���ݒ�
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};
	// �V�F�[�_�[�̐ݒ�
	pipelineDesc.VS.pShaderBytecode = vsBlob->GetBufferPointer();
	pipelineDesc.VS.BytecodeLength = vsBlob->GetBufferSize();
	pipelineDesc.PS.pShaderBytecode = psBlob->GetBufferPointer();
	pipelineDesc.PS.BytecodeLength = psBlob->GetBufferSize();

	// �T���v���}�X�N�̐ݒ�
	pipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // �W���ݒ�

	// ���X�^���C�U�̐ݒ�
	pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE; // �J�����O���Ȃ�
	//pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK; // �w�ʂ̃J�����O����
	pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID; // �|���S�����h��Ԃ�
	//pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME; // ���C���[�t���[��
	pipelineDesc.RasterizerState.DepthClipEnable = true; // �[�x�N���b�s���O��L����

	//// �u�����h�X�e�[�g
	//	pipelineDesc.BlendState.RenderTarget[0].RenderTargetWriteMask
	//		= D3D12_COLOR_WRITE_ENABLE_ALL;


	// �����_�[�^�[�Q�b�g�̃u�����h�ݒ�
	D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = pipelineDesc.BlendState.RenderTarget[0];
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;// RBGA�S�Ẵ`�����l����`��

	//���ʐݒ�
	blenddesc.BlendEnable = true;					// �u�����h��L���ɂ���
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;	// ���Z
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;		// �\�[�X�̒l��100�� �g��
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;	// �f�X�g�̒l��  0�� �g��

	////���Z����
	//blenddesc.BlendOp = D3D12_BLEND_OP_ADD;		// ���Z
	//blenddesc.SrcBlend = D3D12_BLEND_ONE;		// �\�[�X�̒l��100�� �g��
	//blenddesc.DestBlend = D3D12_BLEND_ONE;		// �\�[�X�̒l��100�� �g��

	//// ���Z����
	//blenddesc.BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;    // �f�X�g����\�[�X�����Z
	//blenddesc.SrcBlend = D3D12_BLEND_ONE;               // �\�[�X�̒l��100% �g��
	//blenddesc.DestBlend = D3D12_BLEND_ONE;              // �f�X�g�̒l��100% �g��

	//// �F���]
	//blenddesc.BlendOp = D3D12_BLEND_OP_ADD;             // ���Z
	//blenddesc.SrcBlend = D3D12_BLEND_INV_DEST_COLOR;    // 1.0f-�f�X�g�J���[�̒l
	//blenddesc.DestBlend = D3D12_BLEND_ZERO;             // �g��Ȃ�

	// ����������
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;             // ���Z
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;         // �\�[�X�̃A���t�@�l
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;    // 1.0f-�\�[�X�̃A���t�@�l


	// ���_���C�A�E�g�̐ݒ�
	pipelineDesc.InputLayout.pInputElementDescs = inputLayout;
	pipelineDesc.InputLayout.NumElements = _countof(inputLayout);


	// �}�`�̌`��ݒ�
	pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	// ���̑��̐ݒ�
	pipelineDesc.NumRenderTargets = 1; // �`��Ώۂ�1��
	pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0~255�w���RGBA
	pipelineDesc.SampleDesc.Count = 1; // 1�s�N�Z���ɂ�1��T���v�����O

#pragma region �f�X�N���v�^�����W�̐ݒ�

	// �f�X�N���v�^�����W�̐ݒ�
	D3D12_DESCRIPTOR_RANGE descriptorRange{};
	descriptorRange.NumDescriptors = 1;         //��x�̕`��Ɏg���e�N�X�`����1���Ȃ̂�1
	descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange.BaseShaderRegister = 0;     //�e�N�X�`�����W�X�^0��
	descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;


#pragma endregion �f�X�N���v�^�����W

	// ���[�g�p�����[�^�̐ݒ�
	D3D12_ROOT_PARAMETER rootParams[3] = {};
	// �萔�o�b�t�@0��
	rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;   // ���
	rootParams[0].Descriptor.ShaderRegister = 0;                   // �萔�o�b�t�@�ԍ�
	rootParams[0].Descriptor.RegisterSpace = 0;                    // �f�t�H���g�l
	rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;  // �S�ẴV�F�[�_���猩����
	// �e�N�X�`�����W�X�^0��
	rootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;   //���
	rootParams[1].DescriptorTable.pDescriptorRanges = &descriptorRange;		  //�f�X�N���v�^�����W
	rootParams[1].DescriptorTable.NumDescriptorRanges = 1;              		  //�f�X�N���v�^�����W��
	rootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;               //�S�ẴV�F�[�_���猩����
	// �萔�o�b�t�@1��
	rootParams[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;   // ���
	rootParams[2].Descriptor.ShaderRegister = 1;                   // �萔�o�b�t�@�ԍ�
	rootParams[2].Descriptor.RegisterSpace = 0;                    // �f�t�H���g�l
	rootParams[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;  // �S�ẴV�F�[�_���猩����
	



	// �e�N�X�`���T���v���[�̐ݒ�
	D3D12_STATIC_SAMPLER_DESC samplerDesc{};
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;                 //���J��Ԃ��i�^�C�����O�j
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;                 //�c�J��Ԃ��i�^�C�����O�j
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;                 //���s�J��Ԃ��i�^�C�����O�j
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;  //�{�[�_�[�̎��͍�
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;                   //�S�ă��j�A���
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;                                 //�~�b�v�}�b�v�ő�l
	samplerDesc.MinLOD = 0.0f;                                              //�~�b�v�}�b�v�ŏ��l
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;           //�s�N�Z���V�F�[�_����̂ݎg�p�\



	// ���[�g�V�O�l�`���̐ݒ�
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureDesc.pParameters = rootParams; //���[�g�p�����[�^�̐擪�A�h���X
	rootSignatureDesc.NumParameters = _countof(rootParams);        //���[�g�p�����[�^��
	rootSignatureDesc.pStaticSamplers = &samplerDesc;
	rootSignatureDesc.NumStaticSamplers = 1;

	// ���[�g�V�O�l�`���̃V���A���C�Y
	ID3DBlob* rootSigBlob = nullptr;
	result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob, &errorBlob);
	assert(SUCCEEDED(result));
	result = device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(result));
	rootSigBlob->Release();
	// �p�C�v���C���Ƀ��[�g�V�O�l�`�����Z�b�g
	pipelineDesc.pRootSignature = rootSignature;

	// �f�v�X�X�e���V���X�e�[�g�̐ݒ�
	pipelineDesc.DepthStencilState.DepthEnable = true;							// �[�x�e�X�g���s��
	pipelineDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;	// �������݋���
	pipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	pipelineDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;								// �[�x�l�t�H�[�}�b�g

	// �p�C�v�����X�e�[�g�̐���
	result = device->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&pipelineState));
	assert(SUCCEEDED(result));

	// �q�[�v�ݒ�
	D3D12_HEAP_PROPERTIES cbHeapProp{};
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;                   // GPU�ւ̓]���p
	// ���\�[�X�ݒ�
	D3D12_RESOURCE_DESC cbResourceDesc{};
	cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbResourceDesc.Width = (sizeof(ConstBufferDataMaterial) + 0xff) & ~0xff;   // 256�o�C�g�A���C�������g
	cbResourceDesc.Height = 1;
	cbResourceDesc.DepthOrArraySize = 1;
	cbResourceDesc.MipLevels = 1;
	cbResourceDesc.SampleDesc.Count = 1;
	cbResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;


	// �萔�o�b�t�@�̐���
	result = device->CreateCommittedResource(
		&cbHeapProp, // �q�[�v�ݒ�
		D3D12_HEAP_FLAG_NONE,
		&cbResourceDesc, // ���\�[�X�ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffMaterial));
	assert(SUCCEEDED(result));

	// �萔�o�b�t�@�̃}�b�s���O
	result = constBuffMaterial->Map(0, nullptr, (void**)&constMapMaterial); // �}�b�s���O
	assert(SUCCEEDED(result));

	{

		// �q�[�v�ݒ�
		D3D12_HEAP_PROPERTIES cbHeapProp{};
		cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;                   // GPU�ւ̓]���p
		// ���\�[�X�ݒ�
		D3D12_RESOURCE_DESC cbResourceDesc{};
		cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		cbResourceDesc.Width = (sizeof(ConstBufferDataMaterial) + 0xff) & ~0xff;   // 256�o�C�g�A���C�������g
		cbResourceDesc.Height = 1;
		cbResourceDesc.DepthOrArraySize = 1;
		cbResourceDesc.MipLevels = 1;
		cbResourceDesc.SampleDesc.Count = 1;
		cbResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		
		// ������
		InitializeObject3d(&object3d, device);

#pragma region view
		//���s���e�ϊ�
			// �P�ʍs�����
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

		////�������e�ϊ�
		//	// �ˉe�ϊ��s��̌v�Z
		//matProjection =
		//	XMMatrixPerspectiveFovLH(
		//		CameraHorizon(distans),				// �㉺��p45�x
		//		(float)window_width / window_height,	// �A�X�y�N�g�� �i ��ʉ���/��ʏc�� �j
		//		0.1f, 1000.0f							// �O�[�A���[
		//	);


		//// �r���[�ϊ��s��

		//eye = { 0, 0, -100 };	// ���_���W
		//target = { object3ds[0].position.x, object3ds[0].position.y, object3ds[0].position.z };	// �����_���W
		//up = { 0, 1, 0 };		// ������x�N�g��
		//matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));]
#pragma endregion

	}

#pragma region �摜�C���[�W�f�[�^�̍쐬


	TexMetadata metadata{};
	ScratchImage scratchImg{};
	// WIC�e�N�X�`���̃��[�h
	result = LoadFromWICFile(
		L"Resources/mario.png",
		WIC_FLAGS_NONE,
		&metadata, scratchImg);

	// �Q���ڗp�ɕʂ̕ϐ���p�ӂ��Ă���
	TexMetadata metadata2{};
	ScratchImage scratchImg2{};
	// WIC�e�N�X�`���̃��[�h
	result = LoadFromWICFile(
		L"Resources/reimu.png",
		WIC_FLAGS_NONE,
		&metadata2, scratchImg2);

	ScratchImage mipChain{};
	// �~�b�v�}�b�v����
	result = GenerateMipMaps(
		scratchImg.GetImages(), scratchImg.GetImageCount(), scratchImg.GetMetadata(),
		TEX_FILTER_DEFAULT, 0, mipChain);
	if (SUCCEEDED(result)) {
		scratchImg = std::move(mipChain);
		metadata = scratchImg.GetMetadata();
	}

	// �ǂݍ��񂾃f�B�t���[�Y�e�N�X�`����SRGB�Ƃ��Ĉ���
	metadata.format = MakeSRGB(metadata.format);

	ScratchImage mipChain2{};
	// �~�b�v�}�b�v����
	result = GenerateMipMaps(
		scratchImg2.GetImages(), scratchImg2.GetImageCount(), scratchImg2.GetMetadata(),
		TEX_FILTER_DEFAULT, 0, mipChain2);
	if (SUCCEEDED(result)) {
		scratchImg2 = std::move(mipChain2);
		metadata2 = scratchImg2.GetMetadata();
	}

	// �ǂݍ��񂾃f�B�t���[�Y�e�N�X�`����SRGB�Ƃ��Ĉ���
	metadata2.format = MakeSRGB(metadata2.format);


#pragma endregion �摜�C���[�W�f�[�^�̍쐬

#pragma region �e�N�X�`���o�b�t�@
	// �q�[�v�ݒ�
	D3D12_HEAP_PROPERTIES textureHeapProp{};
	textureHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	textureHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	textureHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

	// ���\�[�X�ݒ�
	D3D12_RESOURCE_DESC textureResourceDesc{};
	textureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureResourceDesc.Format = metadata.format;
	textureResourceDesc.Width = metadata.width;  // ��
	textureResourceDesc.Height = (UINT)metadata.height; // ����
	textureResourceDesc.DepthOrArraySize = (UINT)metadata.arraySize;
	textureResourceDesc.MipLevels = (UINT)metadata.mipLevels;
	textureResourceDesc.SampleDesc.Count = 1;

	// �e�N�X�`���o�b�t�@�̐���
	ID3D12Resource* texBuff = nullptr;
	result = device->CreateCommittedResource(
		&textureHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&textureResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texBuff));

	// ���\�[�X�ݒ�
	D3D12_RESOURCE_DESC textureResourceDesc2{};
	textureResourceDesc2.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureResourceDesc2.Format = metadata2.format;
	textureResourceDesc2.Width = metadata2.width;  // ��
	textureResourceDesc2.Height = (UINT)metadata2.height; // ����
	textureResourceDesc2.DepthOrArraySize = (UINT)metadata2.arraySize;
	textureResourceDesc2.MipLevels = (UINT)metadata2.mipLevels;
	textureResourceDesc2.SampleDesc.Count = 1;
	// �e�N�X�`���o�b�t�@�̐���
	ID3D12Resource* texBuff2 = nullptr;
	result = device->CreateCommittedResource(
		&textureHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&textureResourceDesc2,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texBuff2));

	// �S�~�b�v�}�b�v�ɂ���
	for (size_t i = 0; i < metadata.mipLevels; i++) {
		// �~�b�v�}�b�v���x�����w�肵�ăC���[�W���擾
		const Image* img = scratchImg.GetImage(i, 0, 0);
		// �e�N�X�`���o�b�t�@�Ƀf�[�^�]��
		result = texBuff->WriteToSubresource(
			(UINT)i,
			nullptr,              // �S�̈�փR�s�[
			img->pixels,          // ���f�[�^�A�h���X
			(UINT)img->rowPitch,  // 1���C���T�C�Y
			(UINT)img->slicePitch // 1���T�C�Y
		);
		assert(SUCCEEDED(result));
	}
	// �S�~�b�v�}�b�v�ɂ���
	for (size_t i = 0; i < metadata2.mipLevels; i++) {
		// �~�b�v�}�b�v���x�����w�肵�ăC���[�W���擾
		const Image* img = scratchImg2.GetImage(i, 0, 0);
		// �e�N�X�`���o�b�t�@�Ƀf�[�^�]��
		result = texBuff2->WriteToSubresource(
			(UINT)i,
			nullptr,              // �S�̈�փR�s�[
			img->pixels,          // ���f�[�^�A�h���X
			(UINT)img->rowPitch,  // 1���C���T�C�Y
			(UINT)img->slicePitch // 1���T�C�Y
		);
		assert(SUCCEEDED(result));
	}


#pragma endregion �e�N�X�`���o�b�t�@

#pragma region �f�X�N���v�^�[�q�[�v

	// SRV�̍ő��
	const size_t kMaxSRVCount = 2056;

	// �f�X�N���v�^�q�[�v�̐ݒ�
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//�V�F�[�_���猩����悤��
	srvHeapDesc.NumDescriptors = kMaxSRVCount;

	// �ݒ������SRV�p�f�X�N���v�^�q�[�v�𐶐�

	result = device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srvHeap));
	assert(SUCCEEDED(result));

	// SRV�q�[�v��퓬�n���h�����擾
	D3D12_CPU_DESCRIPTOR_HANDLE srvHandle = srvHeap->GetCPUDescriptorHandleForHeapStart();





#pragma endregion �f�X�N���v�^�[�q�[�v

#pragma region �V�F�[�_���\�[�X�r���[

	// �V�F�[�_���\�[�X�r���[�ݒ�
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};			// �ݒ�\����
	srvDesc.Format = resDesc.Format;	//
	srvDesc.Shader4ComponentMapping =
		D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;// 2D�e�N�X�`��
	srvDesc.Texture2D.MipLevels = resDesc.MipLevels;

	// �n���h���̎w���ʒu�ɃV�F�[�_�[���\�[�X�r���[�쐬
	device->CreateShaderResourceView(texBuff, &srvDesc, srvHandle);

	// ��n���h����i�߂�
	incrementSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	srvHandle.ptr += incrementSize;

	// �V�F�[�_�[���\�[�X�r���[�ݒ�
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc2{};
	srvDesc2.Format = textureResourceDesc2.Format;
	srvDesc2.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc2.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc2.Texture2D.MipLevels = textureResourceDesc2.MipLevels;

	//�n���h���̎w���ʒu�ɃV�F�[�_�[���\�[�X�r���[�쐬
	device->CreateShaderResourceView(texBuff2, &srvDesc2, srvHandle);

}

void Mesh::Update(ID3D12Device* device, Input* input) {
	// NULL�|�C���^�`�F�b�N
	assert(input);


	


	//�������e�ϊ�
		//�ˉe�ϊ��s��̌v�Z
	matProjection =
		XMMatrixPerspectiveFovLH(
			MathFunc::FieldOfViewY(focalLengs, sensor),
			(float)WinApp::window_width / WinApp::window_height,	// �A�X�y�N�g�� �i ��ʉ���/��ʏc�� �j
			0.1f, 1500.0f// �O�[�A���[
		);
	// �r���[�ϊ��s��

	eye = { 0,0,1000 };	// ���_���W
	target = { -20,0,0 };	// �����_���W
	up = { 0, 1, 0 };		// ������x�N�g��
	matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));

	if (input->PushKey(DIK_D)) {
		object3d.rotation.y += 0.1f;
	}
	
		UpdateObject3d(&object3d, matView, matProjection);
		
	


	// �l���������ނƎ����I�ɓ]�������
	constMapMaterial->color = XMFLOAT4(R.x, G.x, B.x, 1.0f);	// RGBA�Ŕ������̐�



}

void Mesh::Draw(ID3D12GraphicsCommandList* commandList) {


	// �p�C�v���C���X�e�[�g�ƃ��[�g�V�O�l�`���̐ݒ�R�}���h
	commandList->SetPipelineState(pipelineState);
	commandList->SetGraphicsRootSignature(rootSignature);

	// �v���~�e�B�u�`��̐ݒ�R�}���h
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // �O�p�`���X�g
	//commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP); // �O�p�`�X�g���b�v

	// ���_�o�b�t�@�r���[�̐ݒ�R�}���h
	commandList->IASetVertexBuffers(0, 1, &vbView);

	// �萔�o�b�t�@�r���[(CBV)�̐ݒ�R�}���h
	commandList->SetGraphicsRootConstantBufferView(0, constBuffMaterial->GetGPUVirtualAddress());

	// SRV�q�[�v�̐ݒ�R�}���h
	commandList->SetDescriptorHeaps(1, &srvHeap);
	// SRV�q�[�v�̐擪�n���h�����擾�iSRV���w���Ă���͂��j
	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle = srvHeap->GetGPUDescriptorHandleForHeapStart();
	// SRV�q�[�v�̐擪�ɂ���SRV�����[�g�p�����[�^1�Ԃɐݒ�
	commandList->SetGraphicsRootDescriptorTable(1, srvGpuHandle);

	// 2���ڂ��w�������悤�ɂ���srv�̃n���h�������[�g�p�����[�^1�Ԃɐݒ�
	srvGpuHandle.ptr += incrementSize;
	commandList->SetGraphicsRootDescriptorTable(1, srvGpuHandle);


	// �C���f�b�N�X�o�b�t�@�r���[�̐ݒ�R�}���h
	commandList->IASetIndexBuffer(&ibView);

	// �`��R�}���h (���_�o�b�t�@�̂�)

	//commandList->DrawInstanced(vertices.size(), 1, 0, 0); // �S�Ă̒��_���g���ĕ`��

	// �S�I�u�W�F�N�g�ɂ��ď���
	DrawObject3d(&object3d, commandList, vbView, ibView, indices.size());
	

}

void Mesh::CreateModel()
{
}

void Mesh::LoadModel(const char* fileName) {

	if (fileName) {

		//�J�������t�@�C���̊g���q���擾
		size_t fileNameLen = strlen(fileName);
		strcpy_s(fileType, fileName + fileNameLen - 4);


#pragma region//.obj�t�@�C���̏ꍇ
		if (strcmp(fileType, ".obj") == 0) {

			//�t�@�C���X�g���[��
			std::fstream file;
			//.obj�t�@�C�����J��
			file.open(fileName);
			//�t�@�C���I�[�v�����s���`�F�b�N
			if (file.fail()) {
				assert(0);
			}

			std::vector<XMFLOAT3>positions;	//���_���W
			std::vector<XMFLOAT3>normals;	//�@���x�N�g��
			std::vector<XMFLOAT2>texcoords;	//�e�N�X�`��UV
			//1�s���ǂݍ���
			std::string line;
			while (getline(file, line)) {

				//1�s���̕�������X�g���[���ɕϊ����ĉ�͂��₷������
				std::istringstream line_stream(line);

				//���p�X�y�[�X��؂�ōs�̐擪�������擾
				std::string key;
				getline(line_stream, key, ' ');

				//�擪������v�Ȃ璸�_���W
				if (key == "v") {

					//X,Y,Z���W�ǂݍ���
					XMFLOAT3 position{};
					line_stream >> position.x;
					line_stream >> position.y;
					line_stream >> position.z;

					//���W�f�[�^�ɒǉ�
					positions.emplace_back(position);

				}

				//�擪������vt�Ȃ�e�N�X�`��
				if (key == "vt") {
					//U,V�����ǂݍ���
					XMFLOAT2 texcoord{};
					line_stream >> texcoord.x;
					line_stream >> texcoord.y;

					//V�������]
					texcoord.y = 1.0f - texcoord.y;

					//�e�N�X�`�����W�f�[�^�ɒǉ�
					texcoords.emplace_back(texcoord);

				}

				if (key == "vn") {
					//X,Y,Z�����ǂݍ���
					XMFLOAT3 normal{};
					line_stream >> normal.x;
					line_stream >> normal.y;
					line_stream >> normal.z;

					//�@���x�N�g���f�[�^�ɒǉ�
					normals.emplace_back(normal);

				}

				//�擪������f�Ȃ�|���S��(�O�p�`)
				if (key == "f") {

					//���p�X�y�[�X��؂�ōs�̑�����ǂݍ���
					std::string index_string;
					while (getline(line_stream, index_string, ' ')) {

						//���_�C���f�b�N�X1���̕�������X�g���[���ɕϊ����ĉ�͂��₷������
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

						//�C���f�b�N�X�f�[�^�̒ǉ�
						indices.emplace_back((unsigned short)indices.size());
					}
				}
			}
			file.close();
		}
#pragma endregion
		else {
			//�Ή����Ă��Ȃ����f���̓G���[��f��
			assert(nullptr);
		}
	}
	else {
	LoadModel();
	}
}

void Mesh::LoadModel() {

	//���_�f�[�^
	vertices = {
		//�O
		{{-5.0f , -5.0f , -5.0f} , {} , {0.0f , 1.0f}} ,//���� �C���f�b�N�X0
		{{-5.0f , 5.0f , -5.0f} , {} , {0.0f , 0.0f}} ,//���� �C���f�b�N�X1
		{{5.0f , -5.0f , -5.0f} , {} , {1.0f , 1.0f}} ,//�E�� �C���f�b�N�X2
		{{5.0f , 5.0f , -5.0f} , {} , {1.0f , 0.0f}} ,//�E�� �C���f�b�N�X3
		//��
		{{-5.0f , -5.0f , 5.0f} , {} , {0.0f , 1.0f}} ,//���� �C���f�b�N�X0
		{{-5.0f , 5.0f , 5.0f} , {} , {0.0f , 0.0f}} ,//���� �C���f�b�N�X1
		{{5.0f , -5.0f , 5.0f} , {} , {1.0f , 1.0f}} ,//�E�� �C���f�b�N�X2
		{{5.0f , 5.0f , 5.0f} , {} , {1.0f , 0.0f}} ,//�E�� �C���f�b�N�X3
		//��
		{{-5.0f , -5.0f , -5.0f} , {} , {0.0f , 1.0f}} ,//���� �C���f�b�N�X0
		{{-5.0f , -5.0f , 5.0f} , {} , {0.0f , 0.0f}} ,//���� �C���f�b�N�X1
		{{-5.0f , 5.0f , -5.0f} , {} , {1.0f , 1.0f}} ,//�E�� �C���f�b�N�X2
		{{-5.0f , 5.0f , 5.0f} , {} , {1.0f , 0.0f}} ,//�E�� �C���f�b�N�X3
		//�E
		{{5.0f , -5.0f , -5.0f} , {} , {0.0f , 1.0f}} ,//���� �C���f�b�N�X0
		{{5.0f , -5.0f , 5.0f} , {} , {0.0f , 0.0f}} ,//���� �C���f�b�N�X1
		{{5.0f , 5.0f , -5.0f} , {} , {1.0f , 1.0f}} ,//�E�� �C���f�b�N�X2
		{{5.0f , 5.0f , 5.0f} , {} , {1.0f , 0.0f}} ,//�E�� �C���f�b�N�X3
		//��
		{{-5.0f , 5.0f , -5.0f} , {} , {0.0f , 1.0f}} ,//���� �C���f�b�N�X0
		{{5.0f , 5.0f , -5.0f} , {} , {0.0f , 0.0f}} ,//���� �C���f�b�N�X1
		{{-5.0f , 5.0f , 5.0f} , {} , {1.0f , 1.0f}} ,//�E�� �C���f�b�N�X2
		{{5.0f , 5.0f , 5.0f} , {} , {1.0f , 0.0f}} ,//�E�� �C���f�b�N�X3
		//��
		{{-5.0f , -5.0f , -5.0f} , {} , {0.0f , 1.0f}} ,//���� �C���f�b�N�X0
		{{5.0f , -5.0f , -5.0f} , {} , {0.0f , 0.0f}} ,//���� �C���f�b�N�X1
		{{-5.0f , -5.0f , 5.0f} , {} , {1.0f , 1.0f}} ,//�E�� �C���f�b�N�X2
		{{5.0f , -5.0f , 5.0f} , {} , {1.0f , 0.0f}} ,//�E�� �C���f�b�N�X3

	};

	//�C���f�b�N�X�f�[�^
	indices = {
		//�O
		0 , 1 , 2 , //�O�p�`1��
		1 , 3 , 2 , //�O�p�`2��
		//���
		4 , 6 , 5 ,
		5 , 6 , 7 ,
		//��
		8 , 9 , 10 ,
		9 , 11 , 10 ,
		//�E
		12 , 14 , 13 ,
		13 , 14 , 15 ,
		//��
		16 , 18 , 17 ,
		17 , 18 , 19 ,
		//��
		20 , 21 , 22 ,
		21 , 23 , 22 ,
	};

}


void InitializeObject3d(Object3d* object, ID3D12Device* device) {

	HRESULT result;

	// �萔�o�b�t�@�̃q�[�v�ݒ�
	D3D12_HEAP_PROPERTIES heapProp{};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	// �萔�o�b�t�@�̃��\�[�X�ݒ�
	D3D12_RESOURCE_DESC resdesc{};
	resdesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resdesc.Width = (sizeof(ConstBufferDataTransform) + 0xff) & ~0xff;
	resdesc.Height = 1;
	resdesc.DepthOrArraySize = 1;
	resdesc.MipLevels = 1;
	resdesc.SampleDesc.Count = 1;
	resdesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// �萔�o�b�t�@�̐���
	result = device->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resdesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&object->constBuffB0));
	assert(SUCCEEDED(result));

	// �萔�o�b�t�@�̃}�b�s���O
	result = object->constBuffB0->Map(0, nullptr, (void**)&object->constMapTransform);
	assert(SUCCEEDED(result));

	// B1�p
	resdesc = CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB1) + 0xff) &~0xff);
	// �萔�o�b�t�@�̐���
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

	// �X�P�[���A��]�A���s�ړ��s��̌v�Z
	matScale = XMMatrixScaling(object->scale.x, object->scale.y, object->scale.z);
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(object->rotation.z);
	matRot *= XMMatrixRotationX(object->rotation.x);
	matRot *= XMMatrixRotationY(object->rotation.y);
	matTrans = XMMatrixTranslation(object->position.x, object->position.y, object->position.z);

	// ���[���h�s��̍���
	object->matWorld = XMMatrixIdentity();	// �ό`�����Z�b�g
	object->matWorld *= matScale;			// ���[���h�s��ɃX�P�[�����O�𔽉f
	object->matWorld *= matRot;				// ���[���h�s��ɉ�]�𔽉f
	object->matWorld *= matTrans;			// ���[���h�s��ɕ��s�ړ��𔽉f

	// �e�I�u�W�F�N�g�������
	if (object->parent != nullptr) {
		// �e�I�u�W�F�N�g�̃��[���h�s���������
		object->matWorld *= object->parent->matWorld;
	}

	// �萔�o�b�t�@�փf�[�^�]��
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

// 3D�I�u�W�F�N�g�`��
void DrawObject3d(
	Object3d* object,
	ID3D12GraphicsCommandList* commandList,
	D3D12_VERTEX_BUFFER_VIEW& vbView,
	D3D12_INDEX_BUFFER_VIEW& ibView,
	UINT numIndices)
{
	// ���_�o�b�t�@�̐ݒ�
	commandList->IASetVertexBuffers(0, 1, &vbView);
	// �C���f�b�N�X�o�b�t�@�̐ݒ�
	commandList->IASetIndexBuffer(&ibView);
	// �萔�o�b�t�@�r���[�iCBV�j�̐ݒ�R�}���h
	commandList->SetGraphicsRootConstantBufferView(2, object->constBuffB0->GetGPUVirtualAddress());
	// �`��R�}���h
	commandList->DrawIndexedInstanced(numIndices, 1, 0, 0, 0);

}
