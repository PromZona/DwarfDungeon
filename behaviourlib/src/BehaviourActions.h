#pragma once
#include "BehaviourManager.h"
#include "BehaviourNodes.h"
#include "EntityManager.h"
#include "Game.h"
#include "MovementManager.h"
#include "enemy.h"
#include <algorithm>
#include <godot_cpp/classes/character_body2d.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

namespace BehaviourLib {

static BehaviourLib::Status
FindTarget(Game& game, UnitBlackBoard& blackboard)
{
  auto playerUnits = game.EntityManager->GetPlayerUnits();

  godot::CharacterBody2D* enemy =
    game.EntityManager->GetEnemy(blackboard.unit_id);
  godot::Vector2 enemy_pos = enemy->get_position();

  float lowestDistance = std::numeric_limits<float>::max();
  EntityId closestEntity = NULL_ENTITY;
  for (size_t i = 0; i < playerUnits.size(); i++) {
    float distance = playerUnits[i]->get_position().distance_to(enemy_pos);
    if (distance < lowestDistance) {
      closestEntity = EntityId(i);
      lowestDistance = distance;
    }
  }
  if (closestEntity == NULL_ENTITY) {
    godot::UtilityFunctions::print(
      "Find Target: Failed to find unit to follow");
    return Status::FAILED;
  }

  blackboard.target_unit_id = closestEntity;
  godot::UtilityFunctions::print("Find target action: ",
                                 blackboard.unit_id,
                                 "->",
                                 blackboard.target_unit_id);
  return BehaviourLib::Status::SUCCESS;
}

static BehaviourLib::Status
StartMove(Game& game, UnitBlackBoard& blackboard)
{
  game.MovementManager->m_MovingEntities.push_back(blackboard.unit_id);
  return BehaviourLib::Status::SUCCESS;
}

static BehaviourLib::Status
CheckIfArrived(Game& game, UnitBlackBoard& blackboard)
{
  auto playerUnits = game.EntityManager->GetPlayerUnits();
  MovementManager* manager = game.MovementManager;

  Enemy* enemy = game.EntityManager->GetEnemy(blackboard.unit_id);
  godot::CharacterBody2D* targetUnit = playerUnits[blackboard.target_unit_id];
  float distance =
    enemy->get_position().distance_to(targetUnit->get_position());

  if (distance < enemy->AttackRadius) {
    manager->m_MovingEntities.erase(blackboard.unit_id);
    return BehaviourLib::Status::SUCCESS;
  }
  return BehaviourLib::Status::RUNNING;
}

static BehaviourLib::Status
Pause(Game& game, UnitBlackBoard& blackboard)
{
  if (!blackboard.isWaiting) {

    blackboard.timestamp = std::chrono::system_clock::now();
    blackboard.isWaiting = true;
  }
  auto now = std::chrono::system_clock::now();
  std::chrono::seconds duration =
    std::chrono::duration_cast<std::chrono::seconds>(now -
                                                     blackboard.timestamp);

  if (duration.count() >= 5) {
    blackboard.isWaiting = false;
    return BehaviourLib::Status::SUCCESS;
  }
  return BehaviourLib::Status::RUNNING;
}

static BehaviourLib::Status
Attack(Game& game, UnitBlackBoard& blackboard)
{
  auto playerUnits = game.EntityManager->GetPlayerUnits();

  Enemy* enemy = game.EntityManager->GetEnemy(blackboard.unit_id);
  if (!blackboard.isAttacking) {
    godot::CharacterBody2D* targetUnit = playerUnits[blackboard.target_unit_id];
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
