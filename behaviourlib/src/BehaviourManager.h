#pragma once

#include "BehaviourNodes.h"
#include <cstdint>
#include <godot_cpp/classes/character_body2d.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/sprite2d.hpp>
#include <godot_cpp/variant/typed_array.hpp>
#include <unordered_map>
#include <vector>

namespace godot {

typedef int64_t EntityId;
const EntityId NULL_ENTITY = -1;

struct UnitBlackBoard
{
  EntityId unit_id;
  EntityId target_unit_id;
};

struct ExecutionFrame
{
  BehaviourLib::NodeId currentNode;
};

class BehaviourManager : public Node
{
  GDCLASS(BehaviourManager, Node)

private:
  std::vector<CharacterBody2D*> m_enemies;
  std::vector<CharacterBody2D*> m_units;
  std::vector<UnitBlackBoard> m_boards;
  std::vector<> m_executionMemory;
  Node* m_group;

  BehaviourLib::Tree m_tree;

  std::unordered_map<std::string,
                     std::function<BehaviourLib::Status(const BehaviourManager*,
                                                        UnitBlackBoard&)>>
    m_actionTable;

  BehaviourLib::Status ExecuteNode(const BehaviourLib::Node& node);
  void LoadAiTree(const std::string& filename);
  void RegisterActionTable();

protected:
  static void _bind_methods();

public:
  BehaviourManager();
  ~BehaviourManager();

  void _process(double delta) override;
  void _physics_process(double delta) override;
  void _ready() override;

  static BehaviourLib::Status SuperFindTarget(const BehaviourManager* manager,
                                              UnitBlackBoard& blackboard);
  static BehaviourLib::Status StartMove(const BehaviourManager* manager,
                                        UnitBlackBoard& blackboard);
  static BehaviourLib::Status CheckIfArrived(const BehaviourManager* manager,
                                             UnitBlackBoard& blackboard);
  EntityId FindTarget(EntityId enemy_id) const;
};

} // namespace godot
