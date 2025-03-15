#include "Game.h"
#include "BehaviourManager.h"
#include "EntityManager.h"
#include "MovementManager.h"
#include "SpawnManager.h"
#include "godot_cpp/core/memory.hpp"

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
  this->SpawnManager =
    (BehaviourLib::SpawnManager*)find_child("SpawnManager", false);
  this->MovementManager = memnew(class MovementManager);
  this->BehaviourManager = memnew(class BehaviourManager);
  this->UIManager = memnew(class UIManager);
}

void
Game::PostRegisterManagers()
{
  godot::UtilityFunctions::print("Game: Post Register Managers");
  EntityManager->data = &Data.Entities;
  MovementManager->gameData = &Data;
  BehaviourManager->game = this;
  SpawnManager->game = this;
  UIManager->game = this;
}

void
Game::PreGameStart()
{
  godot::UtilityFunctions::print("Game: Pre Game Start");
  EntityManager->InitializeEntites();
  BehaviourManager->PreGameStart();
}

void
Game::LoadScene(godot::String sceneName)
{
  using namespace godot;

  UtilityFunctions::print("Loading scene... : ", sceneName);

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

  UIManager->UpdateUiBindings(sceneInstance);
  UtilityFunctions::print("Scene Loaded!");
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

  LoadScene("res://scenes/MainMenu.tscn");
  godot::UtilityFunctions::print("Game: Initialization End");
}

void
Game::_physics_process(double delta)
{
  if (godot::Engine::get_singleton()->is_editor_hint()) {
    return;
  }

  MovementManager->Update();
  BehaviourManager->Update(delta);
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

void
Game::StartGame()
{
  LoadScene("res://scenes/Level_1.tscn");

  EntityManager->ActivatePlayerUnits();
  Data.Spawn.isActive = true;
}

} // namespace Behaviourlib
