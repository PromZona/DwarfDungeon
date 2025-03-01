#pragma once

#include "BehaviourNodes.h"
#include "EntityManager.h"
#include <cstdint>
#include <deque>
#include <godot_cpp/classes/character_body2d.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/sprite2d.hpp>
#include <godot_cpp/variant/typed_array.hpp>
#include <unordered_map>
#include <vector>

namespace BehaviourLib {

class Game;

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

class BehaviourManager : public godot::Node
{
  GDCLASS(BehaviourManager, godot::Node)

public:
  std::vector<godot::CharacterBody2D*> m_units;
  std::vector<UnitBlackBoard> m_boards;
  std::vector<ExecutionContext> m_executionContext;
  std::vector<EntityId> m_movingEntities;
  Node* m_group;

  EntityManager* m_entityManager;
  Game* m_Game;

  BehaviourLib::Tree m_tree;

  std::unordered_map<
    std::string,
    std::function<BehaviourLib::Status(Game&, UnitBlackBoard&)>>
    m_actionTable;

  void RegisterDependencies(Game* game, EntityManager* entityManager);
  BehaviourLib::Status ExecuteNode(EntityId entityId);
  void LoadAiTree(const std::string& filename);
  void RegisterActionTable();
  void PreGameStart();

protected:
  static void _bind_methods();

public:
  BehaviourManager();

  void _process(double delta) override;
  void _physics_process(double delta) override;
  void _ready() override;
};

} // namespace godot
