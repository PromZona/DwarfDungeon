#pragma once
#include "BehaviourManager.h"
#include "enemy.h"
#include <algorithm>
#include <godot_cpp/classes/character_body2d.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

namespace BehaviourLib {

static BehaviourLib::Status
FindTarget(BehaviourManager* manager, UnitBlackBoard& blackboard)
{
  godot::CharacterBody2D* enemy = manager->m_enemies[blackboard.unit_id];
  godot::Vector2 enemy_pos = enemy->get_position();

  float lowestDistance = std::numeric_limits<float>::max();
  EntityId closestEntity = NULL_ENTITY;
  for (int i = 0; i < manager->m_units.size(); i++) {
    float distance = manager->m_units[i]->get_position().distance_to(enemy_pos);
    if (distance < lowestDistance) {
      closestEntity = EntityId(i);
      lowestDistance = distance;
    }
  }
  blackboard.target_unit_id = closestEntity;
  godot::UtilityFunctions::print("Find target action: ",
                                 blackboard.unit_id,
                                 "->",
                                 blackboard.target_unit_id);
  return BehaviourLib::Status::SUCCESS;
}

static BehaviourLib::Status
StartMove(BehaviourManager* manager, UnitBlackBoard& blackboard)
{
  manager->m_movingEntities.push_back(blackboard.unit_id);
  return BehaviourLib::Status::SUCCESS;
}

static BehaviourLib::Status
CheckIfArrived(BehaviourManager* manager, UnitBlackBoard& blackboard)
{
  Enemy* enemy = static_cast<Enemy*>(manager->m_enemies[blackboard.unit_id]);
  godot::CharacterBody2D* targetUnit =
    manager->m_units[blackboard.target_unit_id];
  float distance =
    enemy->get_position().distance_to(targetUnit->get_position());

  if (distance < enemy->AttackRadius) {
    auto position = std::find(manager->m_movingEntities.begin(),
                              manager->m_movingEntities.end(),
                              blackboard.unit_id);

    if (position != manager->m_movingEntities.end())
      manager->m_movingEntities.erase(position);
    return BehaviourLib::Status::SUCCESS;
  }
  return BehaviourLib::Status::RUNNING;
}

static BehaviourLib::Status
Pause(BehaviourManager* manager, UnitBlackBoard& blackboard)
{
  if (!blackboard.isWaiting) {

    blackboard.timestamp = std::chrono::system_clock::now();
    blackboard.isWaiting = true;
  }
  auto now = std::chrono::system_clock::now();
  std::chrono::seconds duration =
    std::chrono::duration_cast<std::chrono::seconds>(now -
                                                     blackboard.timestamp);

  if (duration.count() >= 5.0f) {
    blackboard.isWaiting = false;
    return BehaviourLib::Status::SUCCESS;
  }
  return BehaviourLib::Status::RUNNING;
}

static BehaviourLib::Status
Attack(BehaviourManager* manager, UnitBlackBoard& blackboard)
{
  Enemy* enemy = static_cast<Enemy*>(manager->m_enemies[blackboard.unit_id]);
  if (!blackboard.isAttacking) {
    godot::CharacterBody2D* targetUnit =
      manager->m_units[blackboard.target_unit_id];
    godot::Vector2 targetPosition = targetUnit->get_position();
    enemy->Attack(targetPosition);
    blackboard.isAttacking = true;
    return BehaviourLib::Status::RUNNING;
  }

  if (!enemy->IsAttacking()) {
    blackboard.isAttacking = false;
    return BehaviourLib::Status::SUCCESS;
  }
  return BehaviourLib::Status::RUNNING;
}
} // namespace BehaviourLib
