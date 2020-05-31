#include "Application.h"
#include <Urho3D/Engine/Application.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Engine/Console.h>
#include <Urho3D/UI/Cursor.h>
#include <Urho3D/Engine/DebugHud.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Engine/EngineDefs.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Scene/SceneEvents.h>
#include <Urho3D/UI/Sprite.h>
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/Core/Timer.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/Resource/XMLFile.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/Zone.h>
#include <Urho3D/Urho2D/AnimatedSprite2D.h>
#include <Urho3D/Urho2D/AnimationSet2D.h>
#include <Urho3D/Urho2D/Sprite2D.h>
#include <Urho3D/Core/CoreEvents.h>

static const StringHash VAR_MOVESPEED("MoveSpeed");
static const StringHash VAR_ROTATESPEED("RotateSpeed");

void FlashPoint::SpawnCharacters() {
    // Spawn Hero
    AnimationSet2D* heroSet = cache_->GetResource<AnimationSet2D>("Assets/characters/PNG/Satyr_03/VectorParts/Animations.scml");
    heroNode_ = scene_->CreateChild("SpriterAnimation");
    AnimatedSprite2D* heroSprite = heroNode_->CreateComponent<AnimatedSprite2D>();
    heroSprite->SetAnimationSet(heroSet);
    mainHero_ = Character(1000, 120, 0, 0, heroSprite, heroSet);

    // Spawn Golem
    AnimationSet2D* enemySet = cache_->GetResource<AnimationSet2D>("Assets/golems/PNG/Golem_03/Vector Parts/Animations.scml");
    enemyNode_ = scene_->CreateChild("SpriterAnimation");
    AnimatedSprite2D* enemySprite = enemyNode_->CreateComponent<AnimatedSprite2D>();
    enemySprite->SetAnimationSet(enemySet);
    enemies_ = new Character[1];
    this->enemiesSize_++;
    enemies_[0] = *new Character(350, 100, 3, 0, enemySprite, enemySet);

    //Start Animations
    mainHero_.GetAnimatedSprite()->SetAnimation(mainHero_.GetAnimationSet()->GetAnimation(0), LM_FORCE_LOOPED);
    enemies_[0].GetAnimatedSprite()->SetAnimation(enemies_[0].GetAnimationSet()->GetAnimation(1), LM_FORCE_LOOPED);
}

FlashPoint::FlashPoint(Context* context) :
    Sample(context),
    animation_index(0),
    enemiesSize_(0)
{
}

void FlashPoint::Setup()
{
    // Called before engine initialization. engineParameters_ member variable can be modified here
}

void FlashPoint::Start()
{
    //
    // SCENE
    //
    scene_ = new Scene(context_);
    scene_->CreateComponent<Octree>();
    cameraNode_ = scene_->CreateChild("Camera");
    cameraNode_->SetPosition(Vector3(0.0f, 0.0f, -10.0f));
    Camera* camera = cameraNode_->CreateComponent<Camera>();
    camera->SetOrthographic(true);
    graphics = GetSubsystem<Graphics>();
    // Set camera ortho size (the value of PIXEL_SIZE is 0.01)
    camera->SetOrthoSize((float)graphics->GetHeight() * 0.01);

    int halfWidth = graphics->GetWidth() / 2;
    int halfHeight = graphics->GetHeight() / 2;
    cache_ = GetSubsystem<ResourceCache>();

    //
    // VIEWPORT
    //
    Renderer* renderer = GetSubsystem<Renderer>();
    SharedPtr<Viewport> viewport(new Viewport(context_, scene_, cameraNode_->GetComponent<Camera>()));
    renderer->SetViewport(0, viewport);

    //
    // Spawner
    //
    SpawnCharacters();

    // Called after engine initialization. Setup application & subscribe to events here
    SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(FlashPoint, HandleKeyDown));
    SubscribeToEvent(E_KEYUP, URHO3D_HANDLER(FlashPoint, HandleKeyUp));
    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(FlashPoint, HandleUpdate));
    SubscribeToEvent(E_MOUSEBUTTONDOWN, URHO3D_HANDLER(FlashPoint, HandleMouseButtonDown));

    // Unsubscribe the SceneUpdate event from base class to prevent camera pitch and yaw in 2D sample
    UnsubscribeFromEvent(E_SCENEUPDATE);
}

void FlashPoint::Stop()
{
    // Perform optional cleanup     after main loop has terminated
}

void FlashPoint::HandleKeyDown(StringHash eventType, VariantMap& eventData)
{
    using namespace KeyDown;
    // Check for pressing ESC. Note the engine_ member variable for convenience access to the Engine object
    int key = eventData[P_KEY].GetInt();
    if (key == KEY_ESCAPE) 
        engine_->Exit();
    if (key == KEY_RIGHT) {
        this->mainHero_.GetAnimatedSprite()->SetFlipX(false);
        this->mainHero_.play_animation(AnimationCode::walk);
        this->mainHero_.SetVelocityX(0.01);
    }
    if (key == KEY_LEFT) {
        this->mainHero_.GetAnimatedSprite()->SetFlipX(true);
        this->mainHero_.play_animation(AnimationCode::walk);
        this->mainHero_.SetVelocityX(-0.01);
    }
}

void FlashPoint::HandleKeyUp(StringHash eventType, VariantMap& eventData) 
{
    using namespace KeyUp;

    int key = eventData[P_KEY].GetInt();
    if (key == KEY_RIGHT || key == KEY_LEFT) {
        this->mainHero_.play_animation(AnimationCode::blink_idle);
        this->mainHero_.SetVelocityX(0);
    }

}

void FlashPoint::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
    using namespace Update;// Take the frame time step, which is stored as a float
    float timeStep = eventData[P_TIMESTEP].GetFloat();
    this->mainHero_.UpdateXByVelocity();
    heroNode_->SetPosition(Vector3(mainHero_.GetX(), mainHero_.GetY(), -1.2f));
    for (int index = 0; index < enemiesSize_; index++) 
    {
        enemyNode_->SetPosition(Vector3(enemies_[index].GetX(), enemies_[0].GetY(), -1.1f));
    }
}

void FlashPoint::HandleMouseButtonDown(StringHash eventType, VariantMap& eventData)
{
    AnimatedSprite2D* spriterAnimatedSprite = heroNode_->GetComponent<AnimatedSprite2D>();
    AnimationSet2D* spriterAnimationSet = spriterAnimatedSprite->GetAnimationSet();
    animation_index = (animation_index + 1) % spriterAnimationSet->GetNumAnimations();
    spriterAnimatedSprite->SetAnimation(spriterAnimationSet->GetAnimation(animation_index), LM_FORCE_LOOPED);
}

URHO3D_DEFINE_APPLICATION_MAIN(FlashPoint)