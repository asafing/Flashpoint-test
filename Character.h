#include <Urho3D/Urho2D/AnimatedSprite2D.h>
#include <Urho3D/Urho2D/AnimationSet2D.h>
#include <Urho3D/Urho2D/Sprite2D.h>
#include <Urho3D/Urho2D/RigidBody2D.h>
#include <Urho3D/Scene/Scene.h>
#include "Urho3D/Urho2D/CollisionBox2D.h"


using namespace Urho3D;

namespace Urho3D
{
	class Node;
}

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
	Node* node;
	String name;
	int hp_;
	int damage_;
	float velocity_x;
	float velocity_y;
	AnimationSet2D* animation_set;
	//CollisionBox2D weapon_box;

	void do_damage(Character* target_char);
	Vector<Character*>* ScanHittable(Vector<Character>* chars);
public:
	Character(Node* node, String name, int hp, int damage, AnimationSet2D* set);
	Character();
	void play_animation(AnimationCode animation_code);
	void suffer_damage(int damage);
	Character* attack(Vector<Character>* characters);
	void faint();
	void MoveX(float x_delta);
	int GetDamage();
	int GetHP();
	float GetX();
	float GetY();
	String GetName();
	Node* GetNode();
	AnimationSet2D* GetAnimationSet();
	void SetNode(Node* node);
	float GetVelocityX();
	void SetVelocityX(float velocity);
	float GetVelocityY();
	void SetVelocityY(float velocity);
	void UpdateXByVelocity();
	bool operator!=(Character cmp_char);
	bool CheckCollision(Character coll_char);
};

#pragma once
