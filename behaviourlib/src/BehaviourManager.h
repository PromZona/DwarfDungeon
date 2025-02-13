#pragma once

#include "BehaviourNodes.h"
#include <cstdint>
#include <godot_cpp/classes/character_body2d.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/sprite2d.hpp>
#include <godot_cpp/variant/typed_array.hpp>
#include <vector>

namespace godot {

typedef int64_t EntityId;
const EntityId NULL_ENTITY = -1;

struct UnitBlackBoard
{
  EntityId unit_id;
  EntityId target_unit_id;
};

class BehaviourManager : public Node
{
  GDCLASS(BehaviourManager, Node)

private:
  std::vector<CharacterBody2D*> m_enemies;
  std::vector<CharacterBody2D*> m_units;
  std::vector<UnitBlackBoard> m_boards;
  Node* m_group;

  std::vector<BehaviourLib::ActionNode> m_actionNodes;
  std::vector<BehaviourLib::SequqenceNode> m_sequenceNodes;
  std::vector<BehaviourLib::SelectorNode> m_selectorNodes;

  BehaviourLib::Node m_root;

  BehaviourLib::Status ExecuteNode(BehaviourLib::Node node);

protected:
  static void _bind_methods();

public:
  BehaviourManager();
  ~BehaviourManager();

  void _process(double delta) override;
  void _physics_process(double delta) override;
  void _ready() override;

  EntityId FindTarget(EntityId enemy_id) const;
};

} // namespace godot
