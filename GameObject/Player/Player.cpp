﻿#include "Player.h"

void Player::Initialize(const std::vector<Model*>& models)
{
	std::vector<Model*> PlayerModel = { models[kModelIndexBody],models[kModelIndexHead],models[kModelIndexL_arm],models[kModelIndexR_arm]
	};
	BaseCharacter::Initialize(PlayerModel);
	std::vector<Model*> WeaponModel = { models[kModelIndexWeapon]
	};
	weapon_ = std::make_unique<Weapon>();
	weapon_->Initialize(WeaponModel);


	BoxCollider::Initialize();
	input = Input::GetInstance();


	WorldTransformInitalize();

	const char* groupName = "Player";
	BoxCollider::SetcollisionMask(~kCollitionAttributePlayer);
	BoxCollider::SetcollitionAttribute(kCollitionAttributePlayer);
	BoxCollider::SetParent(worldTransform_);
	BoxCollider::SetSize({3.0f,3.0f,1.0f});
	GlobalVariables::GetInstance()->CreateGroup(groupName);
	GlobalVariables::GetInstance()->AddItem(groupName,"DashSpeed",workDash_.dashSpeed_);

	moveQuaternion_ = IdentityQuaternion();
}

void Player::Update()
{	
	//jsonファイルの内容を適応
	ApplyGlobalVariables();
	//パッドの状態をゲット
	input->GetJoystickState(0,joyState);

	if (behaviorRequest_) {
		//ふるまいの変更
		behavior_ = behaviorRequest_.value();
		//各ふるまいごとに初期化
		switch (behavior_)
		{
		case Behavior::kRoot:
		default:
			BehaviorRootInit();
			break;
		case Behavior::kAttack:
			BehaviorAttackInit();
			break;
		case Behavior::kDash:
			BehaviorDashInit();
			break;
		case Behavior::kJump:
			BehaviorJumpInit();
			break;
		}
		
		behaviorRequest_ = std::nullopt;
	}
	switch (behavior_)
	{
	case Behavior::kRoot:
	default:
		BehaviorRootUpdate();
		break;
	case Behavior::kAttack:
		BehaviorAttackUpdate();
		break;
	case Behavior::kDash:
		BehaviorDashUpdate();
		break;
	case Behavior::kJump:
		BehaviorJumpUpdate();
		break;
	}



	if (worldTransform_.translation_.y <= -20.0f) {
		//地面から落ちたらリスタートする
		worldTransform_.translation_ = { 0.0f,0.0f,0.0f };
		worldTransform_.UpdateMatrix();
	}
	
	worldTransform_.quaternion = Slerp(worldTransform_.quaternion, moveQuaternion_, 0.3f);

	worldTransform_.quaternion = Normalize(worldTransform_.quaternion);

	BaseCharacter::Update();
	worldTransformBody_.UpdateMatrix();
	worldTransformHead_.UpdateMatrix();
	worldTransformL_arm_.UpdateMatrix();
	worldTransformR_arm_.UpdateMatrix();
	weapon_->Update();

	BoxCollider::Update(&worldTransform_);
	
	
}

void Player::Draw(const ViewProjection& viewProjection)
{
	models_[kModelIndexBody]->Draw(worldTransformBody_, viewProjection);
	models_[kModelIndexHead]->Draw(worldTransformHead_, viewProjection);
	models_[kModelIndexL_arm]->Draw(worldTransformL_arm_, viewProjection);
	models_[kModelIndexR_arm]->Draw(worldTransformR_arm_, viewProjection);

	if(behavior_ == Behavior::kAttack){
		weapon_->Draw(viewProjection);
	}
}
void Player::OnCollision(const uint32_t collisionAttribute)
{
	if (collisionAttribute == kCollitionAttributeEnemy) {
		//敵に当たったらリスタートする
		worldTransform_.translation_ = { 0.0f,0.0f,0.0f };
		worldTransform_.UpdateMatrix();
		behaviorRequest_ = Behavior::kRoot;
	}
	else if (collisionAttribute == kCollitionAttributeFloor) {
		IsOnGraund = true;
	}
	else if (collisionAttribute == kCollitionAttributeMoveFloor) {
		IsOnGraund = true;
	}
	else if (collisionAttribute == kCollitionAttributeGoal) {
		//ゴールしたらリスタートする
		worldTransform_.translation_ = { 0.0f,0.0f,0.0f };
		worldTransform_.UpdateMatrix();
		behaviorRequest_ = Behavior::kRoot;
	}
	else {
		return;
	}
	
}
void Player::SetParent(const WorldTransform* parent)
{
	// 親子関係を結ぶ
	if (!worldTransform_.parent_) {
		worldTransform_.parent_ = parent;
		Vector3 Pos = Subtract(worldTransform_.GetTranslateFromMatWorld(), parent->GetTranslateFromMatWorld());
		worldTransform_.translation_ = Pos;
		worldTransform_.UpdateMatrix();
	}
}
void Player::WorldTransformInitalize()
{
	worldTransformBody_.Initialize();
	worldTransformHead_.Initialize();
	worldTransformL_arm_.Initialize();
	worldTransformR_arm_.Initialize();
	worldTransform_Weapon_.Initialize();
	//腕の位置調整
	worldTransformL_arm_.translation_.y = 1.4f;
	worldTransformR_arm_.translation_.y = 1.4f;
	//武器の位置調整

	worldTransformHead_.parent_ = &worldTransformBody_;
	worldTransformL_arm_.parent_ = &worldTransformBody_;
	worldTransformR_arm_.parent_ = &worldTransformBody_;
	worldTransform_Weapon_.parent_ = &worldTransformBody_;
	worldTransformBody_.parent_ = &worldTransform_;

	weapon_->SetParent(worldTransform_Weapon_);
}
void Player::Move()
{
		//移動量
		if (joyState.Gamepad.sThumbLX == 0 && joyState.Gamepad.sThumbLY == 0) {
			return;
		}
		move = {
			(float)joyState.Gamepad.sThumbLX / SHRT_MAX, 0.0f,
			(float)joyState.Gamepad.sThumbLY / SHRT_MAX };
		//正規化をして斜めの移動量を正しくする
		move = Normalize(move);
		move.x =move.x * speed;
		move.y =move.y * speed;
		move.z =move.z * speed;
		//カメラの正面方向に移動するようにする
		//回転行列を作る
		Matrix4x4 rotateMatrix = MakeRotateMatrix(viewProjection_->rotation_);
		//移動ベクトルをカメラの角度だけ回転
		move = TransformNormal(move, rotateMatrix);
		//移動
		worldTransform_.translation_ = Add(worldTransform_.translation_, move);
		//プレイヤーの向きを移動方向に合わせる
		//playerのY軸周り角度(θy)
		move = Normalize(move);
		Vector3 cross = Normalize(Cross({ 0.0f,0.0f,1.0f }, move));
		float dot = Dot({ 0.0f,0.0f,1.0f }, move);
		moveQuaternion_ = MakeRotateAxisAngleQuaternion(cross, std::acos(dot));
		
}

void Player::ApplyGlobalVariables()
{
	const char* groupName = "Player";
	workDash_.dashSpeed_ = GlobalVariables::GetInstance()->GetfloatValue(groupName, "DashSpeed");
}

void Player::BehaviorRootInit()
{
	InitializeFloatingGimmick();
	worldTransformL_arm_.quaternion = IdentityQuaternion();
	worldTransformR_arm_.quaternion = IdentityQuaternion();
	weapon_->RootInit();
	DownForce = 0.05f;
}

void Player::BehaviorRootUpdate()
{
	UpdateFloatingGimmick();
	Move();
	PullDown();
	//RTで攻撃
	if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) {
		behaviorRequest_ = Behavior::kAttack;
	}
	//Aでダッシュ
	if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_A) {
		behaviorRequest_ = Behavior::kDash;
	}
	//Bでジャンプ
	if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_B) {
		behaviorRequest_ = Behavior::kJump;
	}
	
}

void Player::BehaviorAttackInit()
{
	//腕の位置
	Vector3 cross = Normalize(Cross({ 0.0f,0.0f,1.0f }, {0.0f,1.0f,0.0f}));
	float dot = Dot({ 0.0f,0.0f,1.0f }, {0.0f,0.0f,-1.0f});
	Quaternion quaternion_ = MakeRotateAxisAngleQuaternion(cross, std::acos(dot));
	worldTransformL_arm_.quaternion = quaternion_;
	worldTransformR_arm_.quaternion = quaternion_;
	//武器の位置
	quaternion_ = MakeRotateAxisAngleQuaternion(cross, 0);
	worldTransform_Weapon_.quaternion = quaternion_;

	attackAnimationFrame = 0;
	weapon_->AttackInit();
}

void Player::BehaviorAttackUpdate()
{
	Vector3 cross = Normalize(Cross({ 0.0f,0.0f,1.0f }, { 0.0f,1.0f,0.0f }));
	Quaternion ArmMovequaternion = worldTransformL_arm_.quaternion;
		
	float dot = Dot({ 0.0f,1.0f,0.0f }, {0.0f,0.0f,0.0f});
	
	ArmMovequaternion = MakeRotateAxisAngleQuaternion(cross, std::acos(dot));
	ArmMovequaternion = Normalize(ArmMovequaternion);
	
	cross = Normalize(Cross({ 0.0f,0.0f,1.0f }, { 0.0f,-1.0f,0.0f }));
	Quaternion WeaponMovequaternion = worldTransform_Weapon_.quaternion;
	
	dot = Dot({ 0.0f,1.0f,0.0f }, { 0.0f,0.0f,0.0f });

	WeaponMovequaternion = MakeRotateAxisAngleQuaternion(cross, std::acos(dot));
	WeaponMovequaternion = Normalize(WeaponMovequaternion);

	if (attackAnimationFrame < 5) {
		// 腕の挙動
		float t = 0.2f;
		
		worldTransformL_arm_.quaternion = Slerp(worldTransformL_arm_.quaternion, ArmMovequaternion, t);
		worldTransformR_arm_.quaternion = Slerp(worldTransformR_arm_.quaternion, ArmMovequaternion, t);
		// 武器の挙動
		worldTransform_Weapon_.quaternion = Slerp(worldTransform_Weapon_.quaternion, WeaponMovequaternion, t);
	}
	else if (attackAnimationFrame < 20) {
		// 腕の挙動
		float t = 0.1f;

		worldTransformL_arm_.quaternion = Slerp(worldTransformL_arm_.quaternion, ArmMovequaternion, t);
		worldTransformR_arm_.quaternion = Slerp(worldTransformR_arm_.quaternion, ArmMovequaternion, t);
		// 武器の挙動
		worldTransform_Weapon_.quaternion = Slerp(worldTransform_Weapon_.quaternion,WeaponMovequaternion, t);
	}
	else {
		behaviorRequest_ = Behavior::kRoot;
	}


	worldTransform_Weapon_.UpdateMatrix();
	weapon_->Update();
	attackAnimationFrame++;
}

void Player::BehaviorDashInit()
{
	workDash_.dashParameter_ = 0;
	//プレイヤーの旋回の補間を切って一瞬で目標角度をに付くようにしている	
	worldTransform_.rotation_.y = targetAngle;
}

void Player::BehaviorDashUpdate()
{
	Vector3 DashVector = { 0.0f,0.0f,1.0f };
	//正規化をして斜めの移動量を正しくする
	DashVector.z = Normalize(DashVector).z * workDash_.dashSpeed_;
	//プレイヤーの正面方向に移動するようにする
	//回転行列を作る
	Matrix4x4 rotateMatrix = MakeRotateMatrix(worldTransform_.rotation_);
	//移動ベクトルをカメラの角度だけ回転
	DashVector = TransformNormal(DashVector, rotateMatrix);
	//移動
	worldTransform_.translation_ = Add(worldTransform_.translation_, DashVector);
	if (++workDash_.dashParameter_ >= behaviorDashTime) {
		behaviorRequest_ = Behavior::kRoot;
	}
}

void Player::BehaviorJumpInit()
{
	worldTransformBody_.translation_.y = 0;
	worldTransformL_arm_.quaternion.x = 0;
	worldTransformR_arm_.quaternion.x = 0;

	//ジャンプ初速
	const float kJumpFirstSpeed = 1.0f;

	move.y = kJumpFirstSpeed;

}

void Player::BehaviorJumpUpdate()
{
	//移動
	worldTransform_.translation_ += move;
	//重力加速度
	const float kGravity = 0.05f;
	//加速度ベクトル
	Vector3 accelerationVector = {0.0f,-kGravity,0.0f};
	//加速する
	move += accelerationVector;

	if (worldTransform_.translation_.y <= 0.0f) {
		worldTransform_.translation_.y = 0.0f;
		//ジャンプ終了
		behaviorRequest_ = Behavior::kRoot;
	}
}

void Player::InitializeFloatingGimmick() {
	floatingParameter_ = 0.0f;
}

void Player::UpdateFloatingGimmick() {
	// 浮遊移動のサイクル<frame>
	const uint16_t T = (uint16_t)floatcycle_;
	// 1フレームでのパラメータ加算値
	const float step = 2.0f * (float)std::numbers::pi / T;
	// パラメータを1ステップ分加算
	floatingParameter_ += step;
	// 2πを超えたら0に戻す
	floatingParameter_ = (float)std::fmod(floatingParameter_, 2.0f * std::numbers::pi);
	// 浮遊の振幅<m>
	const float floatingAmplitude = floatingAmplitude_;
	// 浮遊を座標に反映
	worldTransformBody_.translation_.y = std::sin(floatingParameter_) * floatingAmplitude;

}

void Player::PullDown()
{
	if (IsOnGraund) {
		IsOnGraund = false;
		return;
	}
	else {
		
		//重力加速度
		const float kGravity = 0.05f;
		//加速度ベクトル
		Vector3 accelerationVector = { 0.0f,-kGravity,0.0f };
		//移動
		worldTransform_.translation_.y += DownForce;

		DownForce += accelerationVector.y;
	}
}
