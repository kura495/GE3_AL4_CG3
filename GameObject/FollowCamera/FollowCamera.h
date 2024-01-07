﻿#pragma once
#include "Input.h"
#include "Base/Utility/GlobalVariables.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "MatrixCalc.h"
#include "VectorCalc.h"
#include "Calc.h"

//前方宣言
class LockOn;
//カメラ補間用ワーク
struct WorkInterpolation {
	//追従対象の残像座標
	Vector3 interTarget_ = {};
	//追従対象のY軸
	float targetAngleY_;
	//カメラ補間の媒介変数
	float interParameter_;
};

class FollowCamera {
public:
	void Initalize();
	void Update();

	void SetTarget(const WorldTransform* target);
	const ViewProjection& GetViewProjection() { return viewProjection_; }

private:
	//jsonファイルの値を適応
	void ApplyGlobalVariables();
	//ビュープロジェクション
	ViewProjection viewProjection_;
	//追従対象
	const WorldTransform* target_ = nullptr;
	// ゲームパッド
	XINPUT_STATE joyState;

	Vector2 rotate_ = { 0.0f,0.0f };

	float parameter_t = 0.0f;
	
	//追従対象の座標・角度を再設定
	void Reset();
	WorkInterpolation workInter;
	//追従対象からのオフセットを計算する
	Vector3 OffsetCalc();

};