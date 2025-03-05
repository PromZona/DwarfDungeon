#include "Game.h"
#include "BehaviourManager.h"
#include "EntityManager.h"
#include "MovementManager.h"
#include "SpawnManager.h"

#include <cstdint>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/global_constants.hpp>
#include <godot_cpp/classes/input_event_key.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/variant/typed_array.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

namespace BehaviourLib {

void
Game::_bind_methods()
{
}

void
Game::RegisterManagers()
{
  godot::UtilityFunctions::print("Game: Register Managers");
  this->EntityManager =
    (BehaviourLib::EntityManager*)find_child("EntityManager", false);
  this->BehaviourManager =
    (BehaviourLib::BehaviourManager*)find_child("BehaviourManager", false);
  this->SpawnManager =
    (BehaviourLib::SpawnManager*)find_child("SpawnManager", false);
  this->MovementManager = memnew(class MovementManager);
}

void
Game::PostRegisterManagers()
{
  godot::UtilityFunctions::print("Game: Post Register Managers");
  MovementManager->RegisterDependencies(EntityManager, BehaviourManager);
  BehaviourManager->RegisterDependencies(this, EntityManager);
  SpawnManager->RegisterDependencies(EntityManager);
}

void
Game::PreGameStart()
{
  BehaviourManager->PreGameStart();
}

void
Game::LoadScene(godot::String sceneName)
{
  using namespace godot;
  Ref<PackedScene> scene = ResourceLoader::get_singleton()->load(sceneName);
  if (scene.is_null() || !scene.is_valid()) {

    UtilityFunctions::print("Game, Load Scene: Failed to find and load scene: ",
                            sceneName,
                            ". ",
                            scene.is_null(),
                            "; ",
                            scene.is_valid());
    return;
  }

  godot::Node* staticScene = find_child("StaticEnvironment", false);
  if (staticScene == nullptr) {
    UtilityFunctions::print(
      "Game, Load Scene: Failed to find <StaticEnvironment> Node");
    return;
  }

  Node* sceneInstance = scene->instantiate();
  TypedArray<Node> currentChildrenNodes = staticScene->get_children();

  int64_t childrenCount = currentChildrenNodes.size();
  for (int64_t i = 0; i < childrenCount; i++) {
    Variant v = currentChildrenNodes.pop_front();
    Node* node = Object::cast_to<godot::Node>(v);
    node->queue_free();
  }

  staticScene->add_child(sceneInstance);
}

void
Game::_ready()
{
  if (godot::Engine::get_singleton()->is_editor_hint()) {
    return;
  }

  godot::UtilityFunctions::print("Game: Initialization Start");
  RegisterManagers();
  PostRegisterManagers();
  PreGameStart();
  godot::UtilityFunctions::print("Game: Initialization End");
}

void
Game::_physics_process(double delta)
{
  if (godot::Engine::get_singleton()->is_editor_hint()) {
    return;
  }

  MovementManager->Update();
}

void
Game::_input(const godot::Ref<godot::InputEvent>& event)
{
  const godot::InputEventKey* eventKey =
    godot::Object::cast_to<godot::InputEventKey>(event.ptr());

  if (eventKey == nullptr) {
    return;
  }

  if (eventKey->get_keycode() == godot::KEY_1) {
    LoadScene("res://scenes/Level_1.tscn");
  } else if (eventKey->get_keycode() == godot::KEY_2) {
    LoadScene("res://scenes/Level_2.tscn");
  }
}

} // namespace Behaviourlib
