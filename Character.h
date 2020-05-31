#include <Urho3D/Urho2D/AnimatedSprite2D.h>
#include <Urho3D/Urho2D/AnimationSet2D.h>
#include <Urho3D/Urho2D/Sprite2D.h>

using namespace Urho3D;

enum class AnimationCode {
	idle,
	blink_idle,
	attack,
	start_jump,
	mid_jump,
	suffer_damage,
	faint,
	walk
} ;

class Character {
private:
	int hp_;
	int damage_;
	float x;
	float y;
	float velocity_x;
	float velocity_y;
	AnimatedSprite2D* animatedSprite;
	AnimationSet2D* animationSet;

	void do_damage(Character target_char);
public:
	Character(int hp, int damage, float x, float y, AnimatedSprite2D* animatedSprite, AnimationSet2D* animationSet);
	Character();
	void play_animation(AnimationCode animation_code);
	void suffer_damage(int damage);
	int attack(Character* all_enemies, int size);
	void faint();
	void MoveX(float move_size);
	float GetX();
	float GetY();
	AnimatedSprite2D* GetAnimatedSprite();
	AnimationSet2D* GetAnimationSet();
	void SetVelocityX(float velocity);
	void UpdateXByVelocity();
};

#pragma once
