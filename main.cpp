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
#include <Urho3D/Urho2D/CollisionBox2D.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Physics/PhysicsEvents.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/Text3D.h>
#include <sstream>

static const StringHash VAR_MOVESPEED("MoveSpeed");
static const StringHash VAR_ROTATESPEED("RotateSpeed");

void FlashPoint::SpawnWorld() {
    //Init background
    background_node = scene_->CreateChild("Background");
    background_node->SetPosition(Vector2(0,0));
    StaticSprite2D* back_sprite = background_node->CreateComponent<StaticSprite2D>();
    back_sprite->SetSprite(cache_->GetResource<Sprite2D>("Assets/tiles/2 Background/Background.png"));
    back_sprite->SetDrawRect(Rect(-10.5f, -8.3f, 10.5f, 8.3f)); // Set size
    back_sprite->SetLayer(-5);

    //Init floor
    float curr_x = -4.5;
    float curr_y = -0.4f;
    float tile_size = 1;
    const int total_tiles = 10; 
    SharedPtr<Node>* tile_node = new SharedPtr<Node>();
    floor_vec = new Vector<SharedPtr<Node>>;

     // Create every floor tile and push to floor_vec
    for (int i = 0; i < total_tiles; i++) {
        tile_node->operator=(scene_->CreateChild(String("Floor" + i)));
        tile_node->Get()->SetPosition(Vector2(curr_x, curr_y));
        curr_x += tile_size;
        StaticSprite2D* back_sprite = tile_node->Get()->CreateComponent<StaticSprite2D>();
        String tile_path = "Assets/tiles/1 Tiles/Tile_02.png";
        if (i == 0) {
            tile_path = "Assets/tiles/1 Tiles/Tile_01.png";
        }
        else if (i == total_tiles - 1) {
            tile_path = "Assets/tiles/1 Tiles/Tile_03.png";
        }

        back_sprite->SetSprite(cache_->GetResource<Sprite2D>(tile_path));
        back_sprite->SetDrawRect(Rect(-tile_size / 2, -tile_size / 2, tile_size / 2, tile_size / 2)); // Set size
        back_sprite->SetLayer(-1);
        floor_vec->Push(*tile_node);
    }
}

void FlashPoint::SpawnCharacters() {
    this->enemiesSize_++;

    // Spawn Hero
    AnimationSet2D* heroSet = cache_->GetResource<AnimationSet2D>("Assets/characters/PNG/Satyr_03/VectorParts/Animations.scml");
    Node* hero_node = scene_->CreateChild("Hero");
    hero_node->SetPosition2D(0, 0);
    AnimatedSprite2D* heroSprite = hero_node->CreateComponent<AnimatedSprite2D>();
    heroSprite->SetLayer(5);
    heroSprite->SetAnimationSet(heroSet);
    RigidBody2D* hero_body = hero_node->CreateComponent<RigidBody2D>();
    hero_body->SetBodyType(BT_DYNAMIC);
    hero_body->SetLinearDamping(0.0f);  
    hero_body->SetAngularDamping(0.0f);
    CollisionBox2D* hero_box = hero_node->CreateComponent<CollisionBox2D>();
    hero_box->SetSize(Vector2(3.5f, 3)); // Set size
    hero_box->SetDensity(1.0f); // Set shape density (kilograms per meter squared)
    hero_box->SetFriction(0.5f); // Set friction
    hero_box->SetRestitution(0.1f); // Set restitution (slight bounce)
    mainHero_ = Character(hero_node, "hero", 1000, 120, heroSet);

    // Spawn Golem
    AnimationSet2D* enemySet = cache_->GetResource<AnimationSet2D>("Assets/golems/PNG/Golem_03/Vector Parts/Animations.scml");
    Node* enemy_node = scene_->CreateChild("Hero");
    hero_node->SetPosition2D(3, 0);
    AnimatedSprite2D* enemySprite = enemy_node->CreateComponent<AnimatedSprite2D>();
    enemySprite->SetLayer(1);
    enemySprite->SetAnimationSet(enemySet);
    //RigidBody2D* enemy_body = enemy_node->CreateComponent<RigidBody2D>();
    //enemy_body->SetBodyType(BT_DYNAMIC);
    //enemy_body->SetLinearDamping(0.0f);
    //enemy_body->SetAngularDamping(0.0f);
    CollisionBox2D* enemy_box = enemy_node->CreateComponent<CollisionBox2D>();
    enemy_box->SetSize(Vector2(2, 4)); // Set size
    enemy_box->SetDensity(1.0f); // Set shape density (kilograms per meter squared)
    enemy_box->SetFriction(0.5f); // Set friction
    enemy_box->SetRestitution(0.1f); // Set restitution (slight bounce)
    enemies_->Push(Character(enemy_node, "enemy", 500, 120, enemySet));
    //Start Animations
    mainHero_.GetNode()->GetComponent<AnimatedSprite2D>()->SetAnimation(mainHero_.GetAnimationSet()->GetAnimation(0), LM_FORCE_LOOPED);
    enemies_->Front().GetNode()->GetComponent<AnimatedSprite2D>()->SetAnimation(enemies_->Front().GetAnimationSet()->GetAnimation(0), LM_FORCE_LOOPED);
}
FlashPoint::FlashPoint(Context* context) :
    Sample(context),
    animation_index(0),
    enemiesSize_(0),
    write_index(0),
    floor_vec()
{
    floor_vec = new Vector<SharedPtr<Node>>;
    enemies_ = new Vector<Character>();
}

void FlashPoint::Setup()
{
    // Called before engine initialization. engineParameters_ member variable can be modified here
    // Modify engine startup parameters
    engineParameters_[EP_WINDOW_TITLE] = GetTypeName();
    engineParameters_[EP_LOG_NAME] = GetSubsystem<FileSystem>()->GetAppPreferencesDir("urho3d", "logs") + GetTypeName() + ".log";
    engineParameters_[EP_FULL_SCREEN] = false;
    engineParameters_[EP_HEADLESS] = false;
    engineParameters_[EP_SOUND] = false;

    // Construct a search path to find the resource prefix with two entries:
    // The first entry is an empty path which will be substituted with program/bin directory -- this entry is for binary when it is still in build tree
    // The second and third entries are possible relative paths from the installed program/bin directory to the asset directory -- these entries are for binary when it is in the Urho3D SDK installation location
    if (!engineParameters_.Contains(EP_RESOURCE_PREFIX_PATHS))
        engineParameters_[EP_RESOURCE_PREFIX_PATHS] = ";../share/Resources;../share/Urho3D/Resources";
}

void FlashPoint::Start()
{
    // Execute base class startup

    // Enable OS cursor
    GetSubsystem<Input>()->SetMouseVisible(true);

    // Set the mouse mode to use in the sample
    Sample::InitMouseMode(MM_FREE);

    //
    // SCENE
    //
    scene_ = new Scene(context_);
    scene_->CreateComponent<Octree>();
    cameraNode_ = scene_->CreateChild("Camera");
    cameraNode_->SetPosition(Vector3(0.0f, 0.0f, -10.0f));
    Camera* camera = cameraNode_->CreateComponent<Camera>();
    camera->SetOrthographic(true);
    camera->SetZoom(0.5);
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
    SpawnWorld();
    SpawnCharacters();

    // Called after engine initialization. Setup application & subscribe to events here
    SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(FlashPoint, HandleKeyDown));
    SubscribeToEvent(E_KEYUP, URHO3D_HANDLER(FlashPoint, HandleKeyUp));
    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(FlashPoint, HandleUpdate));
    SubscribeToEvent(E_MOUSEBUTTONDOWN, URHO3D_HANDLER(FlashPoint, HandleMouseButtonDown));
    SubscribeToEvent(E_PHYSICSCOLLISION, URHO3D_HANDLER(FlashPoint, HandleNodeCollision));

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
        this->mainHero_.GetNode()->GetComponent<AnimatedSprite2D>()->SetFlipX(false);
        this->mainHero_.play_animation(AnimationCode::walk);
        this->mainHero_.SetVelocityX(0.01);
    }
    if (key == KEY_LEFT) {
        this->mainHero_.GetNode()->GetComponent<AnimatedSprite2D>()->SetFlipX(true);
        this->mainHero_.play_animation(AnimationCode::walk);
        this->mainHero_.SetVelocityX(-0.01);
    }
    if (key == KEY_Z) {
        Character* a = this->mainHero_.attack(enemies_);
        if (a != nullptr) {
            WriteText(String(enemies_->Front().GetHP()));
        }
    }
}

void FlashPoint::HandleKeyUp(StringHash eventType, VariantMap& eventData) 
{
    using namespace KeyUp;

    int key = eventData[P_KEY].GetInt();
    if (key == KEY_RIGHT && mainHero_.GetVelocityX() < 0)
    {
        this->mainHero_.play_animation(AnimationCode::walk);

    } else if (key == KEY_LEFT && mainHero_.GetVelocityX() > 0)
    {
        this->mainHero_.play_animation(AnimationCode::walk);
    } else if (key == KEY_RIGHT || key == KEY_LEFT) 
    {
        this->mainHero_.play_animation(AnimationCode::blink_idle);
        this->mainHero_.SetVelocityX(0);
    }

}

void FlashPoint::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
    using namespace Update;// Take the frame time step, which is stored as a float
    float timeStep = eventData[P_TIMESTEP].GetFloat();

    // Pop dead enemies
    for (auto it = enemies_->Begin(); it != enemies_->End();) {
        if (it->GetHP() < 0)
        {
            it->GetNode()->GetComponent<AnimatedSprite2D>()->Remove();
            it = enemies_->Erase(it);
        }
        else
            ++it;
    }

    this->mainHero_.UpdateXByVelocity();
    mainHero_.GetNode()->SetPosition(Vector3(mainHero_.GetX(), mainHero_.GetY(), -1.2f));
    for (Vector<Character>::Iterator it = enemies_->Begin(); it != enemies_->End(); it++) {
        it->GetNode()->SetPosition(Vector3(it->GetX(), it->GetY(), -1.2f));
    }
}

void FlashPoint::HandleMouseButtonDown(StringHash eventType, VariantMap& eventData)
{
    AnimatedSprite2D* spriterAnimatedSprite = mainHero_.GetNode()->GetComponent<AnimatedSprite2D>();
    AnimationSet2D* spriterAnimationSet = spriterAnimatedSprite->GetAnimationSet();
    animation_index = (animation_index + 1) % spriterAnimationSet->GetNumAnimations();
    spriterAnimatedSprite->SetAnimation(spriterAnimationSet->GetAnimation(animation_index), LM_FORCE_LOOPED);
}

void FlashPoint::HandleNodeCollision(StringHash eventType, VariantMap& eventData)
{
    WriteText("AAAAAAA");
}

void FlashPoint::WriteText(String s)
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    UI* ui = GetSubsystem<UI>();

    // Construct new Text object, set string to display and font to use
    Text* instructionText = ui->GetRoot()->CreateChild<Text>();
    instructionText->SetText(s);
    instructionText->SetFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 15);
    instructionText->SetTextAlignment(HA_CENTER); // Center rows in relation to each other

    // Position the text relative to the screen center
    instructionText->SetHorizontalAlignment(HA_CENTER);
    instructionText->SetVerticalAlignment(VA_CENTER);
    instructionText->SetPosition(0, ui->GetRoot()->GetHeight() / 10  + (20 * write_index));
    write_index++;
}

URHO3D_DEFINE_APPLICATION_MAIN(FlashPoint)