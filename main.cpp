#include"WinApp.h"
#include"DirectXCommon.h"
#include"Error.h"
#include"Mesh.h"
#include"input.h"
#include"Audio.h"
#include"Object3d.h"
#include"Sprite.h"
#include"GameScene.h"
#include"PostEffect.h"
#include"EnmayPostEffect.h"
#include "SkyPostEffect.h"


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
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int)
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

	Input* input_ = Input::GetInstance();
	input_->Initialize(winApp_);
	
	// 3Dオブジェクト静的初期化
	Object3d::StaticInitialize(dxCommon_->GetDevice(), WinApp::window_width, WinApp::window_height);

	PostEffect* posteffect = nullptr;
	posteffect = new PostEffect();
	posteffect->Initialize(dxCommon_->GetDevice());
	EnemyPostEffect* enemyPostErffect = nullptr;
	enemyPostErffect = new EnemyPostEffect();
	enemyPostErffect->Initialize(dxCommon_->GetDevice());
	SkyPostEffect* skyPos = nullptr;
	skyPos = new SkyPostEffect();
	skyPos->Initialize(dxCommon_->GetDevice());

	////OBJからモデルデータを読み込む
	//Model* model = Model::LoadFormOBJ("cube");

	//Object3d* object3d = Object3d::Create();
	//object3d->SetModel(model);

	//object3d->Update();
	
	GameScene* gamescne = nullptr;
	gamescne = new GameScene();
	gamescne->Initialize(dxCommon_);
	while (true) {
		if (winApp_->ProcessMessage()) {
			break;
		}

		//更新
		input_->Update();
		gamescne->Update();
		//object3d->Update();
		
		//mesh_->Update(dxCommon_->GetDevice(), input_);
		
		posteffect->PreDrawScene(dxCommon_->GetCommandList());

		gamescne->Draw2();
		

		posteffect->PostDrawScene(dxCommon_->GetCommandList());

		enemyPostErffect->PreDrawScene(dxCommon_->GetCommandList());

		gamescne->EnemyDraw();

		enemyPostErffect->PostDrawScene(dxCommon_->GetCommandList());

		skyPos->PreDrawScene(dxCommon_->GetCommandList());

		gamescne->skyDraw();

		skyPos->PostDrawScene(dxCommon_->GetCommandList());

		//描画
		dxCommon_->PreDraw();
		
	

		//Object3d::PreDraw(dxCommon_->GetCommandList());
		skyPos->Draw(dxCommon_->GetCommandList());
		enemyPostErffect->Draw(dxCommon_->GetCommandList());
		posteffect->Draw(dxCommon_->GetCommandList());
		gamescne->Draw();
		
		//object3d->Draw();

		//Object3d::PostDraw();

		//mesh_->Draw(dxCommon_->GetCommandList());

		dxCommon_->PostDraw();

	}
	////3Dオブジェクトの解放
	//delete object3d;
	////3Dモデル開放
	//delete model;
	//入力開放
	

	winApp_->Finalize();
	//WindowsAPI解放
	delete winApp_;
	//元データ解放
	//delete[] imageData;

	return 0;
}