#pragma once

#include "BehaviourNodes.h"
#include <cstdint>
#include <deque>
#include <godot_cpp/classes/character_body2d.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/sprite2d.hpp>
#include <godot_cpp/variant/typed_array.hpp>
#include <unordered_map>
#include <vector>

namespace BehaviourLib {

using EntityId = uint64_t;
const EntityId NULL_ENTITY = 1000; // TODO: Fix to some adequate way of identifying "No-unit" index

struct UnitBlackBoard
{
  std::chrono::system_clock::time_point timestamp;
  EntityId unit_id{NULL_ENTITY};
  EntityId target_unit_id{NULL_ENTITY};
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
  std::vector<godot::CharacterBody2D*> m_enemies;
  std::vector<godot::CharacterBody2D*> m_units;
  std::vector<UnitBlackBoard> m_boards;
  std::vector<ExecutionContext> m_executionContext;
  std::vector<EntityId> m_movingEntities;
  Node* m_group;

  BehaviourLib::Tree m_tree;

  std::unordered_map<
    std::string,
    std::function<BehaviourLib::Status(BehaviourManager*, UnitBlackBoard&)>>
    m_actionTable;

  BehaviourLib::Status ExecuteNode(EntityId entityId);
  void LoadAiTree(const std::string& filename);
  void RegisterActionTable();

protected:
  static void _bind_methods();

public:
  BehaviourManager();
  
  void _process(double delta) override;
  void _physics_process(double delta) override;
  void _ready() override;
};

} // namespace godot
