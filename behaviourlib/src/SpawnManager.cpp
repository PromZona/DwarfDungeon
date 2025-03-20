#include "SpawnManager.h"
#include "Enemy.h"
#include "EntityManager.h"
#include "Game.h"
#include "PerformanceLogger.h"
#include "godot_cpp/classes/collision_object2d.hpp"
#include "godot_cpp/classes/collision_shape2d.hpp"
#include "godot_cpp/classes/random_number_generator.hpp"
#include "godot_cpp/variant/typed_array.hpp"
#include <cassert>
#include <cstdlib>
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
  PERF("SpawnManager _process")

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
SpawnManager::InitializeSpawnPoints()
{
  godot::TypedArray<Node> spawnareas = find_children("SpawnArea*");
  for (size_t i = 0; i < spawnareas.size(); i++) {
    game->Data.Spawn.spawnAreas[i] =
      godot::Object::cast_to<godot::CollisionShape2D>(spawnareas[i]);
  }
}

void
SpawnManager::Spawn()
{
  // Pick spawn point
  // get spawn point unit data
  // ask entity manager to create new unit
  int areaId = game->Data.Rand->randi_range(0, 1);
  float x = game->Data.Rand->randf_range(-64.0f, 64.0f);
  float y = game->Data.Rand->randf_range(-64.0f, 64.0f);

  assert(game->Data.Spawn.spawnAreas.size() > 0);
  godot::Vector2 areaPosition =
    game->Data.Spawn.spawnAreas[areaId]->get_global_position();
  godot::Vector2 spawn_position = { areaPosition.x + x, areaPosition.y + y };

  EntityId id = game->EntityManager->AddEnemy();
  EnemyView* enemy = game->EntityManager->GetEnemyView(id);
  enemy->set_global_position(spawn_position);
  enemy->IsDead = false;
}

} // namespace BehaviourLib
