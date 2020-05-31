#include "Character.h"

Character::Character(int hp, int damage, float x, float y, AnimatedSprite2D* animatedSprite, AnimationSet2D* animationSet) :
	hp_(hp),
	damage_(damage),
	x(x),
	y(y),
	velocity_x(0),
	velocity_y(0),
	animatedSprite(animatedSprite),
	animationSet(animationSet)
{
}

Character::Character() : hp_(100),
	damage_(10),
	x(0),
	y(0),
	velocity_x(0),
	velocity_y(0),
	animatedSprite()
{
}

void Character::do_damage(Character target_char)
{
	target_char.suffer_damage(this->damage_);
}

void Character::suffer_damage(int damage)
{
	this->hp_ -= damage;
}

int Character::attack(Character* all_enemies, int size)
{
	play_animation(AnimationCode::faint);
	return 0;
}

void Character::faint()
{
	play_animation(AnimationCode::faint);
}

void Character::play_animation(AnimationCode animation_code) 
{
	switch (animation_code) {
	case AnimationCode::walk:
		this->animatedSprite->SetAnimation(animationSet->GetAnimation(3), LM_FORCE_LOOPED);
		break;
	case AnimationCode::blink_idle:
		this->animatedSprite->SetAnimation(animationSet->GetAnimation(1), LM_FORCE_LOOPED);
		break;
	default:
		break;
	}
	int a = 3;
}

void Character::MoveX(float move_size) {
	this->x += move_size;
}

float Character::GetX()
{
	return this->x;
}
float Character::GetY()
{
	return this->y;
}

AnimatedSprite2D* Character::GetAnimatedSprite()
{
	return this->animatedSprite;
}

AnimationSet2D* Character::GetAnimationSet() {
	return this->animationSet;
}
void Character::SetVelocityX(float velocity)
{
	this->velocity_x = velocity;
}
void Character::UpdateXByVelocity() {
	this->MoveX(velocity_x);
}