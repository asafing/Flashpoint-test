#include "Character.h"
#include "Urho3D/Urho2D/CollisionBox2D.h"

Character::Character(Node* node, String name, int hp, int damage, AnimationSet2D* animationset) :
	node(node),
	name(name),
	hp_(hp),
	damage_(damage),
	velocity_x(0),
	velocity_y(0),
	animation_set(animationset)
{
}

Character::Character() : hp_(100),
	damage_(10),
	velocity_x(0),
	velocity_y(0)
{
	node;
}

void Character::do_damage(Character* target_char)
{
	target_char->suffer_damage(this->damage_);
}

void Character::suffer_damage(int damage)
{
	this->hp_ -= damage;
}

Character* Character::attack(Vector<Character>* chars)
{
	play_animation(AnimationCode::attack);

	float smallest_x = FLT_MAX;
	Character* target = nullptr;
	Vector<Character*>* hittable_chars = ScanHittable(chars);

	// Iterate over all hittable enemies and find the closest one
	for (Vector<Character*>::Iterator it = hittable_chars->Begin(); it != hittable_chars->End(); it++) {
		float distance = Abs(this->GetX() - (*it)->GetX());
		if (distance < smallest_x) {
			smallest_x = distance;
			target = *it;
		}
	}

	if (target != nullptr)
	{
		target->suffer_damage(this->damage_);
	}

	return target;
}

Vector<Character*>* Character::ScanHittable(Vector<Character>* chars)
{
	Vector<Character*>* hittable = new Vector<Character*>();
	for (Vector<Character>::Iterator it = chars->Begin(); it != chars->End(); it++) 
	{
		if (this->CheckCollision(*it)) {
			hittable->Push(it.ptr_);
		}
	}

	return hittable;
}
void Character::faint()
{
	play_animation(AnimationCode::faint);
}

void Character::play_animation(AnimationCode animation_code) 
{
	switch (animation_code) {
	case AnimationCode::walk:
		node->GetComponent<AnimatedSprite2D>()->SetAnimation(animation_set->GetAnimation(3), LM_FORCE_LOOPED);
		break;
	case AnimationCode::blink_idle:
		node->GetComponent<AnimatedSprite2D>()->SetAnimation(animation_set->GetAnimation(1), LM_FORCE_LOOPED);
		break;
	case AnimationCode::idle:
		node->GetComponent<AnimatedSprite2D>()->SetAnimation(animation_set->GetAnimation(1), LM_FORCE_LOOPED);
		break;
	case AnimationCode::attack:
		node->GetComponent<AnimatedSprite2D>()->SetAnimation(animation_set->GetAnimation(4), LM_DEFAULT);
		node->GetComponent<AnimatedSprite2D>()->SetLoopMode(LM_DEFAULT);
		break;
	case AnimationCode::faint:
		node->GetComponent<AnimatedSprite2D>()->SetAnimation(animation_set->GetAnimation(8), LM_DEFAULT);
		break;
	case AnimationCode::mid_jump:
		node->GetComponent<AnimatedSprite2D>()->SetAnimation(animation_set->GetAnimation(6), LM_DEFAULT);
		break;
	case AnimationCode::start_jump:
		node->GetComponent<AnimatedSprite2D>()->SetAnimation(animation_set->GetAnimation(5), LM_DEFAULT);
		break;
	case AnimationCode::suffer_damage:
		node->GetComponent<AnimatedSprite2D>()->SetAnimation(animation_set->GetAnimation(7), LM_DEFAULT);
		break;
	default:
		break;
	}
}

void Character::MoveX(float x_delta) {
	this->node->SetPosition2D(GetX() + x_delta, GetY());
}

float Character::GetX()
{
	return this->node->GetPosition2D().x_;
}

float Character::GetY()
{
	return this->node->GetPosition2D().y_;
}

void Character::SetVelocityX(float velocity)
{
	this->velocity_x = velocity;
}
void Character::UpdateXByVelocity() {
	this->MoveX(velocity_x);
}

Node* Character::GetNode()
{
	return node;
}

AnimationSet2D* Character::GetAnimationSet()
{
	return animation_set;
}

void Character::SetNode(Node* node) 
{
	this->node = node;
}

int Character::GetDamage()
{
	return damage_;
}
int Character::GetHP()
{
	return hp_;
}

String Character::GetName() 
{
	return name;
}

bool Character::operator!=(Character cmp_char)
{
	if (cmp_char.GetName().Compare(name)) {
		return true;
	}

	return false;
}
bool Character::CheckCollision(Character coll_char) {
	float this_end_x = this->GetX() + (this->GetNode()->GetComponent<CollisionBox2D>()->GetSize().x_ / 2);
	float coll_end_x = coll_char.GetX() + (coll_char.GetNode()->GetComponent<CollisionBox2D>()->GetSize().x_ / 2);
	float this_end_y = this->GetY() + (this->GetNode()->GetComponent<CollisionBox2D>()->GetSize().y_ / 2);
	float coll_end_y = coll_char.GetY() + (coll_char.GetNode()->GetComponent<CollisionBox2D>()->GetSize().y_ / 2);
	float this_start_x = this->GetX() - (this->GetNode()->GetComponent<CollisionBox2D>()->GetSize().x_ / 2);
	float coll_start_x = coll_char.GetX() - (coll_char.GetNode()->GetComponent<CollisionBox2D>()->GetSize().x_ / 2);
	float this_start_y = this->GetY() - (this->GetNode()->GetComponent<CollisionBox2D>()->GetSize().y_ / 2);
	float coll_start_y = coll_char.GetY() - (coll_char.GetNode()->GetComponent<CollisionBox2D>()->GetSize().y_ / 2);
	if (this_start_x <= coll_end_x &&
		this_end_x >= coll_start_x &&
		this_end_y >= coll_start_y &&
		this_start_y < coll_end_y) {
		return true;
	}

	return false;

}