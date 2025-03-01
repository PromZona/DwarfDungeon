#include "EntityManager.h"
#include "BehaviourManager.h"
#include "enemy.h"
#include <godot_cpp/classes/character_body2d.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/global_constants.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/core/memory.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

namespace BehaviourLib {

EntityManager::EntityManager() = default;

EntityManager::~EntityManager() = default;

EntityId
EntityManager::AddEnemy()
{
  EntityId id = m_enemiesCount;

  Enemy* e = m_enemies[id];
  e->set_visible(true);
  e->set_process_mode(Node::ProcessMode::PROCESS_MODE_INHERIT);
  add_child(e);

  m_ActiveEnemies.push_back(id);

  m_enemiesCount++;
  return id;
}

Enemy*
EntityManager::GetEnemy(EntityId id) const
{
  if (id > m_enemiesCount) {
    return nullptr;
  }
  return m_enemies[id];
}

bool
EntityManager::DeleteEnemy(EntityId id)
{
  godot::UtilityFunctions::print("Deleting entities not implemented yet");
  return false;
}

godot::Vector<EntityId>&
EntityManager::GetActiveEnemies()
{
  return m_ActiveEnemies;
}

godot::Vector<Enemy*>&
EntityManager::GetAllEnemies()
{
  return m_enemies;
}

godot::Vector<godot::CharacterBody2D*>&
EntityManager::GetPlayerUnits()
{
  return m_playerUnits;
}

void
EntityManager::_bind_methods()
{
}

void
EntityManager::_ready()
{
  if (godot::Engine::get_singleton()->is_editor_hint()) {
    return;
  }

  m_enemyScene = godot::ResourceLoader::get_singleton()->load(
    "res://charachters/Enemy.tscn", "PackedScene");
  if (!m_enemyScene.is_valid()) {
    godot::UtilityFunctions::print("Error: Failed to find Enemy scene");
    return;
  }

  for (int i = 0; i < m_MaxEnemyCount; i++) {
    Enemy* enemy = Object::cast_to<Enemy>(m_enemyScene->instantiate());
    enemy->set_visible(false);
    enemy->set_process_mode(Node::ProcessMode::PROCESS_MODE_DISABLED);
    m_enemies.push_back(enemy);
  }

  Node* group = find_child("Group");
  godot::TypedArray<godot::Node> units =
    (godot::TypedArray<godot::Node>)group->get("Units");

  for (int i = 0; i < units.size(); i++) {
    godot::CharacterBody2D* p =
      Object::cast_to<godot::CharacterBody2D>(units[i]);
    m_playerUnits.push_back(p);
  }
}

void
EntityManager::_process(double delta)
{
}

} // namespace game
