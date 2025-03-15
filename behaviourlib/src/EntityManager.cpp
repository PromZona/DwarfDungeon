#include "EntityManager.h"
#include <algorithm>
#include <cassert>
#include <godot_cpp/classes/character_body2d.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/global_constants.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/core/memory.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/typed_array.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

namespace BehaviourLib {

EntityManager::EntityManager() = default;
EntityManager::~EntityManager() = default;

EntityId
EntityManager::AddEnemy()
{
  EntityId id = data->enemiesCount;

  assert(id <= data->MaxEnemyCount);

  EnemyView* e = data->enemyViews[id];
  e->set_visible(true);
  e->set_process_mode(Node::ProcessMode::PROCESS_MODE_INHERIT);
  add_child(e);

  data->activeEnemies.push_back(id);

  data->enemiesCount++;
  return id;
}

EnemyView*
EntityManager::GetEnemyView(EntityId id) const
{
  if (id > data->enemiesCount) {
    return nullptr;
  }
  return data->enemyViews[id];
}

bool
EntityManager::DeleteEnemy(EntityId id)
{
  EnemyView* enemy = GetEnemyView(id);
  if (enemy == nullptr) {
    return false;
  }

  enemy->set_visible(false);
  enemy->set_process_mode(Node::ProcessMode::PROCESS_MODE_DISABLED);

  data->availableIds.push(id);

  auto it =
    std::find(data->activeEnemies.begin(), data->activeEnemies.end(), id);
  if (it == data->activeEnemies.end()) {
    return false;
  }

  data->activeEnemies.erase(it);
  return true;
}

void
EntityManager::_bind_methods()
{
}

void
EntityManager::_ready()
{
}

void
EntityManager::InitializeEntites()
{
  // INITIALIZE ENEMIES
  godot::Ref<godot::PackedScene> enemyScene =
    godot::ResourceLoader::get_singleton()->load("res://charachters/Enemy.tscn",
                                                 "PackedScene");
  if (!enemyScene.is_valid()) {
    godot::UtilityFunctions::print("Error: Failed to find Enemy scene");
    return;
  }

  for (size_t i = 0; i < data->MaxEnemyCount; i++) {
    EnemyView* enemy = Object::cast_to<EnemyView>(enemyScene->instantiate());
    enemy->set_visible(false);
    enemy->set_process_mode(Node::ProcessMode::PROCESS_MODE_DISABLED);
    data->enemyViews[i] = enemy;
  }

  // INITIALIZE PLAYER UNITS
  godot::Ref<godot::PackedScene> playerUnitScene =
    godot::ResourceLoader::get_singleton()->load("res://charachters/Unit.tscn",
                                                 "PackedScene");
  if (!playerUnitScene.is_valid()) {
    godot::UtilityFunctions::print("Error: Failed to find Enemy scene");
    return;
  }

  for (size_t i = 0; i < 4; i++) {
    godot::CharacterBody2D* unit =
      Object::cast_to<godot::CharacterBody2D>(playerUnitScene->instantiate());
    unit->set_visible(false);
    unit->set_process_mode(Node::ProcessMode::PROCESS_MODE_DISABLED);
    data->playerUnitViews[i] = unit;
  }

  // INITIALIZE GROUP
  godot::Ref<godot::PackedScene> groupScene =
    godot::ResourceLoader::get_singleton()->load("res://charachters/Group.tscn",
                                                 "PackedScene");
  if (!groupScene.is_valid()) {
    godot::UtilityFunctions::print("Error: Failed to find Enemy scene");
    return;
  }
  data->groupView = Object::cast_to<godot::Node2D>(groupScene->instantiate());
  data->groupView->set_visible(false);
  data->groupView->set_process_mode(Node::ProcessMode::PROCESS_MODE_DISABLED);

  godot::TypedArray<godot::CharacterBody2D> arr{};
  for (auto* unit : data->playerUnitViews) {
    arr.push_back(unit);
  }
  data->groupView->set("Units", arr);
}

void
EntityManager::ActivatePlayerUnits()
{
  data->groupView->set_visible(true);
  data->groupView->set_process_mode(PROCESS_MODE_INHERIT);
  add_child(data->groupView);

  for (auto& unit : data->playerUnitViews) {
    unit->set_visible(true);
    unit->set_process_mode(PROCESS_MODE_INHERIT);
    add_child(unit);
  }
}

void
EntityManager::_process(double delta)
{
  if (godot::Engine::get_singleton()->is_editor_hint()) {
    return;
  }
  godot::Vector<EntityId> freedIDs{};
  for (EntityId unitId : data->activeEnemies) {
    auto* enemy = data->enemyViews[unitId];
    if (enemy->IsDead) {
      freedIDs.push_back(unitId);
    }
  }

  for (EntityId id : freedIDs) {
    godot::UtilityFunctions::print("Freeing Dead Unit [", id, "]");
    DeleteEnemy(id);
  }
}

} // namespace BehaviourLib
