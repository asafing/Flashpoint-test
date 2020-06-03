#include <Urho3D/Engine/Application.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Graphics/Zone.h>
#include "Sample.h"
#include "Character.h"

namespace Urho3D
{
	class Node;
	class Scene;
	class Sprite;
}


using namespace Urho3D;

class FlashPoint : public Sample
{
	URHO3D_OBJECT(FlashPoint, Sample);

private:
	Character mainHero_;
	Character* enemies_;
	int enemiesSize_;
	int animation_index;
	SharedPtr<Scene> scene_;
	SharedPtr<Zone> zone_;
	SharedPtr<Node> cameraNode_;
	SharedPtr<Node> heroNode_;
	SharedPtr<Node> enemyNode_;
	SharedPtr<Graphics> graphics;
	Vector<SharedPtr<Node> > spriteNodes_;
	ResourceCache* cache_;
public:
	FlashPoint(Context* context);
	virtual void Setup();
	virtual void Start();
	virtual void Stop();
	void HandleKeyDown(StringHash eventType, VariantMap& eventData);
	void HandleUpdate(StringHash eventType, VariantMap& eventData);
	void HandleMouseButtonDown(StringHash eventType, VariantMap& eventData);
	void HandleKeyUp(StringHash eventType, VariantMap& eventData);
	void SpawnCharacters();
};