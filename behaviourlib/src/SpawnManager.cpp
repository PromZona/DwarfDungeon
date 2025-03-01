#include "SpawnManager.h"
#include "EntityManager.h"
#include "enemy.h"
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
  if (m_currentSpawnCount >= m_spawnCountTarget) {
    return;
  }
  if (deltaAccumulated < 3.0f) {
    deltaAccumulated += delta;
    return;
  }

  deltaAccumulated = 0.0f;
  m_currentSpawnCount++;
  Spawn();
}

void
SpawnManager::RegisterDependencies(EntityManager* manager)
{
  m_EntityManger = manager;
}

void
SpawnManager::Spawn()
{
  // Pick spawn point
  // get spawn point unit data
  // ask entity manager to create new unit
  godot::Vector2 spawn_position = { 10.0f, 10.0f };

  EntityId id = m_EntityManger->AddEnemy();
  Enemy* enemy = m_EntityManger->GetEnemy(id);
  enemy->set_global_position(spawn_position);
}

} // namespace BehaviourLib
