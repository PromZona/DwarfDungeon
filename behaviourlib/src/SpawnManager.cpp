#include "SpawnManager.h"
#include "Enemy.h"
#include "EntityManager.h"
#include "Game.h"
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

namespace BehaviourLib {

void
SpawnManager::_bind_methods()
{
}

SpawnManager::SpawnManager() {}

void
SpawnManager::_ready()
{
  if (godot::Engine::get_singleton()->is_editor_hint()) {
    return;
  }
}

double deltaAccumulated = 0.0f;

void
SpawnManager::_process(double delta)
{
  if (godot::Engine::get_singleton()->is_editor_hint()) {
    return;
  }

  if (!game->Data.Spawn.isActive) {
    return;
  }
  if (game->Data.Spawn.currentSpawnCount >= game->Data.Spawn.spawnCountTarget) {
    return;
  }
  if (deltaAccumulated < 3.0f) {
    deltaAccumulated += delta;
    return;
  }

  deltaAccumulated = 0.0f;
  game->Data.Spawn.currentSpawnCount++;
  Spawn();
}

void
SpawnManager::Spawn()
{
  // Pick spawn point
  // get spawn point unit data
  // ask entity manager to create new unit
  godot::Vector2 spawn_position = { 10.0f, 10.0f };

  EntityId id = game->EntityManager->AddEnemy();
  EnemyView* enemy = game->EntityManager->GetEnemyView(id);
  enemy->set_global_position(spawn_position);
  enemy->IsDead = false;
}

} // namespace BehaviourLib
