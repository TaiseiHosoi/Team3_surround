#include"WinApp.h"
#include"DirectXCommon.h"
#include"Error.h"
#include"Mesh.h"
#include"input.h"


const float PI = 3.14f;

//using namespace Microsoft::WRL;
template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

//@brief コンソール画面にフォーマット付き文字列の表示
//@param format フォーマット(%dとか%fとかの)
//@param 可変長引数
//@remarks この関数はデバック用です。デバッグ時にしか動作しません
//void DebugOutputFormatString(const char* format, ...) {
//#ifdef _DEBUG
//	va_list valist;
//	va_start(valist, format);
//	vprintf(format, valist);
//	va_end(valist);
//#endif
//}



//Windowsアプリでのエントリーポイント（main関数）
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
//コンソールへの文字出力
{


	//OutputDebugStringA("Hello,DrectX!!/n");

	//ポインタ宣言
	WinApp* winApp_ = nullptr;
	winApp_ = new WinApp;


	DirectXCommon* dxCommon_ = nullptr;
	dxCommon_ = new DirectXCommon;

	//winApp初期化
	winApp_->Initialize();

	MSG msg = {};
	//DirectX初期化処理　ここから
	dxCommon_->Initialize(winApp_);

	Input* input_ = nullptr;
	input_ = new Input;
	input_->Initialize(winApp_);
	
	//頂点データ
	Vertex vertices[] = {
		//前
		//x      y     z       u     v
		{{-5.0f, -5.0f, -5.0f}, {}, {0.0f, 1.0f}}, //左下
		{{-5.0f, 5.0f, -5.0f}, {}, {0.0f, 0.0f}}, //左上
		{{5.0f, -5.0f, -5.0f}, {}, {1.0f, 1.0f}}, //右下
		{{5.0f, 5.0f, -5.0f}, {}, {1.0f, 0.0f}}, //右上

		//後ろ
		//x      y     z       u     v
		{{-5.0f, -5.0f, 5.0f},{}, {0.0f, 1.0f}}, //左下
		{{-5.0f, 5.0f, 5.0f},{}, {0.0f, 0.0f}}, //左上
		{{5.0f, -5.0f, 5.0f},{}, {1.0f, 1.0f}}, //右下
		{{5.0f, 5.0f, 5.0f}, {}, {1.0f, 0.0f}}, //右上

		//左
		//x      y     z       u     v
		{{-5.0f, -5.0f, -5.0f}, {}, {0.0f, 1.0f}}, //左下
		{{-5.0f, -5.0f, 5.0f},{}, {0.0f, 0.0f}}, //左上
		{{-5.0f, 5.0f, -5.0f},{}, {1.0f, 1.0f}}, //右下
		{{-5.0f, 5.0f, 5.0f}, {}, {1.0f, 0.0f}}, //右上

		//右
		{{5.0f, -5.0f, -5.0f},{}, {0.0f, 1.0f}}, //左下
		{{5.0f, -5.0f, 5.0f}, {}, {0.0f, 0.0f}}, //左上
		{{5.0f, 5.0f, -5.0f}, {}, {1.0f, 1.0f}}, //右下
		{{5.0f, 5.0f, 5.0f}, {}, {1.0f, 0.0f}}, //右上

		//下
		{{-5.0f, -5.0f, -5.0f},{}, {0.0f, 1.0f}}, //左下
		{{5.0f, -5.0f, -5.0f},{}, {0.0f, 0.0f}}, //左上
		{{-5.0f, -5.0f, 5.0f},{}, {1.0f, 1.0f}}, //右下
		{{5.0f, -5.0f, 5.0f},{}, {1.0f, 0.0f}}, //右上

		//上
		{{-5.0f, 5.0f, -5.0f},{}, {0.0f, 1.0f}}, //左下
		{{5.0f, 5.0f, -5.0f},{}, {0.0f, 0.0f}}, //左上
		{{-5.0f, 5.0f, 5.0f},{}, {1.0f, 1.0f}}, //右下
		{{5.0f, 5.0f, 5.0f},{}, {1.0f, 0.0f}}, //右上


	};

	Mesh* mesh_ = nullptr;
	mesh_ = new Mesh();
	mesh_->LoadModel("Resources/bumeObj/bume.obj");
	mesh_->Init(dxCommon_->GetDevice());
	

	
	while (true) {
		if (winApp_->ProcessMessage()) {
			break;
		}

		//更新
		input_->Update();

		
		mesh_->Update(dxCommon_->GetDevice(), input_);
		
		//描画
		dxCommon_->PreDraw();

		mesh_->Draw(dxCommon_->GetCommandList());

		dxCommon_->PostDraw();

	}

	winApp_->Finalize();

	//元データ解放
	//delete[] imageData;

	return 0;
}