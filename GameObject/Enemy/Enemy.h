﻿#pragma once
#include "GameObject/BaseCharacter/BaseCharacter.h"

class Player;
class Enemy : public BaseCharacter,public BoxCollider
{
public:
	Enemy();
	~Enemy();
 
	void Initialize(const std::vector<Model*>& models)override;
	void Update()override;
	void Draw(const ViewProjection& viewProjection)override;
	void OnCollision(const Collider* collider)override;
	void SetPos(Vector3 pos) { setPos_ = pos; worldTransform_.translation_ = pos; }
	Vector3 GetPos() const {
	return	{ worldTransform_.matWorld_.m[3][0], worldTransform_.matWorld_.m[3][1]
			, worldTransform_.matWorld_.m[3][2]};
	}
	void setPlayer(Player* player) { player_ = player; }
	bool GetIsAlive() const { return IsAlive; }
	bool GetIsHit() const { return IsHit; }

	void Reset() {
		worldTransform_.translation_ = setPos_; IsAlive = true; IsHit = false; deathAnimationVelocity
			= { 0.0f,0.0f,0.0f };
	}
private:
	void SetParent(const WorldTransform* parent);
	void SoulRotationGimmick();
	//各パーツのローカル座標
	WorldTransform worldTransformBody_;
	WorldTransform worldTransformSoul_;

	Player* player_;

	bool IsAlive = true;
	const int kRespownTime = 120;
	int RespownTimeCount = 0;

	Vector3 setPos_;

	Vector3 Scale_ = { 1.0f,1.0f,1.0f };

	Vector3 deathAnimationVelocity;
	//回転の媒介変数
	float rotate_t = 0.0f;
	float t = 1.0f;
	uint32_t hitCount = 0;
	bool IsHit = false;

	bool IsMove = false;

};
