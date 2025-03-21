#include "EntityManager.h"
#include "Game.h"
#include "GameData.h"
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
EntityManager::AddEnemy(godot::Vector2 position)
{
  EntityId id = game->Data.Entities.enemiesCount;

  godot::UtilityFunctions::print(
    id, " ", BehaviourLib::EntitiesData::MaxEnemyCount);
  assert(id <= game->Data.Entities.MaxEnemyCount);

  EnemyView* e = game->Data.Entities.enemyViews[id];

  e->set_global_position(position);
  e->set_visible(true);
  e->set_process_mode(Node::ProcessMode::PROCESS_MODE_INHERIT);
  add_child(e);

  game->Data.Entities.activeEnemies.push_back(id);
  game->Data.Entities.enemiesCount++;
  return id;
}

EnemyView*
EntityManager::GetEnemyView(EntityId id) const
{
  if (id > game->Data.Entities.enemiesCount) {
    return nullptr;
  }
  return game->Data.Entities.enemyViews[id];
}

bool
EntityManager::DeleteEnemy(EntityId id)
{
  EnemyView* enemy = GetEnemyView(id);
  if (enemy == nullptr) {
    return false;
  }

  game->Data.Arena.enemiesKilled++;

  godot::UtilityFunctions::print(
    "+1 killed [", game->Data.Arena.enemiesKilled, "]");
  enemy->set_visible(false);
  enemy->set_process_mode(Node::ProcessMode::PROCESS_MODE_DISABLED);
  game->Data.Entities.availableIds.push(id);

  auto it = std::ranges::find(game->Data.Entities.activeEnemies, id);
  if (it == game->Data.Entities.activeEnemies.end()) {
    return false;
  }
  game->Data.Entities.activeEnemies.erase(it);

  auto movingItr = std::ranges::find(game->Data.Entities.movingEnemies, id);
  if (movingItr == game->Data.Entities.movingEnemies.end()) {
    return false;
  }
  game->Data.Entities.movingEnemies.erase(movingItr);

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

  for (size_t i = 0; i < game->Data.Entities.MaxEnemyCount; i++) {
    EnemyView* enemy = Object::cast_to<EnemyView>(enemyScene->instantiate());
    enemy->set_visible(false);
    enemy->set_process_mode(Node::ProcessMode::PROCESS_MODE_DISABLED);
    game->Data.Entities.enemyViews[i] = enemy;
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
    game->Data.Entities.playerUnitViews[i] = unit;
  }

  // INITIALIZE GROUP
  godot::Ref<godot::PackedScene> groupScene =
    godot::ResourceLoader::get_singleton()->load("res://charachters/Group.tscn",
                                                 "PackedScene");
  if (!groupScene.is_valid()) {
    godot::UtilityFunctions::print("Error: Failed to find Enemy scene");
    return;
  }
  game->Data.Entities.groupView =
    Object::cast_to<godot::Node2D>(groupScene->instantiate());
  game->Data.Entities.groupView->set_visible(false);
  game->Data.Entities.groupView->set_process_mode(
    Node::ProcessMode::PROCESS_MODE_DISABLED);

  godot::TypedArray<godot::CharacterBody2D> arr{};
  for (auto* unit : game->Data.Entities.playerUnitViews) {
    arr.push_back(unit);
  }
  game->Data.Entities.groupView->set("Units", arr);
}

void
EntityManager::ActivatePlayerUnits()
{
  game->Data.Entities.groupView->set_visible(true);
  game->Data.Entities.groupView->set_process_mode(PROCESS_MODE_INHERIT);
  add_child(game->Data.Entities.groupView);

  for (auto& unit : game->Data.Entities.playerUnitViews) {
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
  for (EntityId unitId : game->Data.Entities.activeEnemies) {
    auto* enemy = game->Data.Entities.enemyViews[unitId];
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
