#include "Game.h"
#include "BehaviourManager.h"
#include "EntityManager.h"
#include "MovementManager.h"
#include "SpawnManager.h"

#include <godot_cpp/classes/engine.hpp>
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

} // namespace Behaviourlib
