﻿#include "WorldTransform.h"

void WorldTransform::Initialize()
{
	matWorld_ = CreateIdentity4x4();
	CreateConstBuffer();
	Map();
	TransferMatrix();
}

void WorldTransform::CreateConstBuffer()
{
	constBuff_ = DirectXCommon::GetInstance()->CreateBufferResource(sizeof(ConstBufferDataWorldTransform));
}

void WorldTransform::Map()
{
	constBuff_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&constMap));
}

void WorldTransform::TransferMatrix()
{
	constMap->matWorld = matWorld_;
}

void WorldTransform::UpdateMatrix()
{
	Matrix4x4 AffineMatrix = MakeAffineMatrix(scale_, rotation_, translation_);
	matWorld_ = AffineMatrix;
	//親があれば親のワールド行列を掛ける
	if (parent_) {
		matWorld_ = Multiply(matWorld_, parent_->matWorld_);
	}

	TransferMatrix();
}
