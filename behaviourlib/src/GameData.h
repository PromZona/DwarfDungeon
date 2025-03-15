#pragma once

#include "BehaviourNodes.h"
#include "Enemy.h"
#include "godot_cpp/classes/character_body2d.hpp"
#include <queue>

namespace BehaviourLib {

using EntityId = size_t;
const EntityId NULL_ENTITY = 10000;

struct EntitiesData
{
  static const uint16_t MaxEnemyCount = 1000;
  static const uint16_t MaxPlayerUnitsCount = 4;

  // VIEWS (Godot classes)
  std::array<godot::CharacterBody2D*, MaxPlayerUnitsCount> playerUnitViews;
  std::array<EnemyView*, MaxEnemyCount> enemyViews;
  godot::Node2D* groupView;

  // Units Data
  // Maybe this should be not oop, but different components of the data in
  // different arrays std::array<EnemyData, MaxEnemyCount> Enemies;
  // std::array<PlayerData, MaxPlayerUnitsCount> PlayerUnits;

  std::vector<EntityId> activeEnemies;
  std::vector<EntityId> movingEnemies;

  std::queue<EntityId> availableIds;
  size_t enemiesCount;
};

struct UnitBlackBoard
{
  std::chrono::system_clock::time_point timestamp;
  EntityId unit_id{ NULL_ENTITY };
  EntityId target_unit_id{ NULL_ENTITY };
  bool isWaiting = false;
  bool isAttacking = false;
};

struct ExecutionFrame
{
  BehaviourLib::NodeId nodeId;
  size_t childIndex;
  BehaviourLib::Status lastChildStatus;
};

struct ExecutionContext
{
  std::deque<ExecutionFrame> stack;
};

struct AiData
{
  std::array<UnitBlackBoard, EntitiesData::MaxEnemyCount> boards;
  std::array<ExecutionContext, EntitiesData::MaxEnemyCount> executionContext;
  BehaviourLib::Tree tree;
};

struct SpawnData
{
    int spawnCountTarget = 3;
    int currentSpawnCount = 0;
    bool isActive = false;
};

struct GameData
{
  EntitiesData Entities;
  AiData Ai;
  SpawnData Spawn;
};

} // namepsace BehaviourLib
