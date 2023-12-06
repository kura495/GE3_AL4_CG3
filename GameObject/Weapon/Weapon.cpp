#include "Weapon.h"

void Weapon::Initialize(const std::vector<Model*>& models)
{
	BaseCharacter::Initialize(models);
	BoxCollider::Initialize();
	BoxCollider::SetcollisionMask(~kCollitionAttributeWeapon&&~kCollitionAttributeEnemy);
	BoxCollider::SetSize({10.0f,5.0f,5.0f});

}

void Weapon::Update()
{
	
	BaseCharacter::Update();
	BoxCollider::Update(&worldTransform_);

}

void Weapon::Draw(const ViewProjection& viewProjection)
{
	BaseCharacter::Draw(viewProjection);
}

void Weapon::RootInit()
{
		BoxCollider::SetcollitionAttribute(0);
}

void Weapon::AttackInit(Vector3 Pos)
{
	BoxCollider::SetcollitionAttribute(kCollitionAttributeWeapon);
	BoxCollider::SetCenter(Pos);
}

void Weapon::SetParent(const WorldTransform& parent)
{
	worldTransform_.parent_ = &parent;
}

void Weapon::OnCollision(const Collider* collider)
{

	if (collider->GetcollitionAttribute() == kCollitionAttributeEnemy) {
		return;
	}
}
