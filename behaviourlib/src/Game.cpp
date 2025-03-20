#include "Game.h"
#include "BehaviourManager.h"
#include "EntityManager.h"
#include "MovementManager.h"
#include "PerformanceLogger.h"
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
  EntityManager->game = this;
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
  SpawnManager->InitializeSpawnPoints();
}

void
Game::LoadScene(Levels level)
{
  using namespace godot;

  godot::String& sceneName = LevelPathTable[level];
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

  Data.Rand.instantiate();

  RegisterManagers();
  PostRegisterManagers();
  PreGameStart();

  LoadScene(Levels::Menu);
  godot::UtilityFunctions::print("Game: Initialization End");
}

void
Game::_process(double delta)
{
  if (godot::Engine::get_singleton()->is_editor_hint()) {
    return;
  }

  // PERFORMANCE LOGGER
  UIManager->DrawDebugInfo();
  PerformanceLogger.Clear();
  PERF("Game _process")
  // PERFORMANCE LOGGER END

  ProcessArena();
  ProcessShop();

  BehaviourManager->Update(delta);
  UIManager->Update();
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
}

void
Game::StartGame()
{
  EnterArena();
  EntityManager->ActivatePlayerUnits();
}

void
Game::EnterShop()
{
  LoadScene(Levels::Shop);

  Data.Spawn.isActive = false;
  Data.IsArena = false;
}

void
Game::EnterArena()
{
  LoadScene(Levels::Arena);

  Data.Spawn.isActive = true;
  Data.IsArena = true;
}

void
Game::ProcessArena()
{
  if (!Data.IsArena) {
    return;
  }

  if (Data.Arena.enemiesKilled >= Data.Arena.enemiesKillGoal) {
    // TODO: Clearing required. Stop active enemies. Zero-ing Arena variables
    godot::UtilityFunctions::print("Game:ProcessArena - Before EnterShop");
    EnterShop();
  }
}
void
Game::ProcessShop()
{
}

} // namespace Behaviourlib
