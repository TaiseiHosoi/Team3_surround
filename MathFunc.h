#pragma once
#include "Matrix4.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Object3d.h"
//#include <WorldTransform.h>


namespace MathFunc {

	//円周率
	const float PI = 3.141592654f;
	// 度数法->ラジアン変換
	float Dig2Rad(float value);
	// ラジアン->度数法変換 
	float Rad2Dig(float value);

	Matrix4 Initialize();
	Matrix4 Scale(Vector3 scale);
	Matrix4 Rotation(Vector3 rotation, int X_1_Y_2_Z_3_XYZ_6);
	Matrix4 Move(Vector3 move);
	//void  affin(WorldTransform& affin);

	Vector3 MatVector(const Vector3 vector3, const Matrix4 matrix4);

	Vector3 GetWorldtransform(const Matrix4 matrix4);

	Vector3 AddVector(const Vector3 v1, const Vector3 v2);

	Vector3 bVelocity(Vector3& velocity, Object3d& worldTransform);
	//座標変換
	Vector3 wDivision(const Vector3& v, const Matrix4& m);

	float FieldOfViewY(float focalLengs, float sensor);

	Matrix4 ConvertXMMATtoMat4(XMMATRIX XMMatrix);
	XMMATRIX ConvertMat4toXMMat(Matrix4 m);

	namespace Ease {
		double In(double start, double end, double time, double max_time);

		double Out(double start, double end, double time, double max_time);

		double InOut(double start, double end, double time, double max_time);

		double In_Back(double start, double end, double time, double max_time);

		double Out_Back(double start, double end, double time, double max_time);

		double InOut_Back(double start, double end, double time, double max_time);

		double Out_Bounce(double start, double end, double time, double max_time);

		double In_Bounce(double start, double end, double time, double max_time);

		double InOut_Bounce(double start, double end, double time, double max_time);
	}
};

