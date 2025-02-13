#include "BehaviourManager.h"
#include "BehaviourNodes.h"

#include <cstdint>
#include <godot_cpp/classes/character_body2d.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/global_constants.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/time.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/math.hpp>
#include <godot_cpp/core/memory.hpp>
#include <godot_cpp/variant/typed_array.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/variant.hpp>
#include <limits>
#include <vector>

using namespace godot;

void
BehaviourManager::_bind_methods()
{
}

BehaviourManager::BehaviourManager()
  : m_enemies()
  , m_units()
  , m_boards()
  , m_group(nullptr)
  , m_actionNodes()
  , m_sequenceNodes()
  , m_selectorNodes()
  , m_root()
{
  if (Engine::get_singleton()->is_editor_hint())
    set_process_mode(Node::ProcessMode::PROCESS_MODE_DISABLED);
}

BehaviourManager::~BehaviourManager() {}

void
BehaviourManager::_ready()
{
  BehaviourLib::ActionNode AttackNode{ .id = 0, .Execute = []() {
                                        UtilityFunctions::print("Attack");
                                        return BehaviourLib::Status::SUCCESS;
                                      } };
  BehaviourLib::ActionNode ChillNode{ .id = 1, .Execute = []() {
                                       UtilityFunctions::print("Chill");
                                       return BehaviourLib::Status::SUCCESS;
                                     } };

  BehaviourLib::SequqenceNode AttackAndChill{
    .id = 0,
    .children =
      std::vector<BehaviourLib::Node>{
        BehaviourLib::Node{ .id = 0, .type = BehaviourLib::NodeType::Action },
        BehaviourLib::Node{ .id = 1, .type = BehaviourLib::NodeType::Action } }
  };

  m_actionNodes.push_back(AttackNode);
  m_actionNodes.push_back(ChillNode);
  m_sequenceNodes.push_back(AttackAndChill);

  m_root = { .id = 0, .type = BehaviourLib::NodeType::Sequence };

  BehaviourLib::Status st = ExecuteNode(m_root);

  Node* parent = get_parent();
  TypedArray<Node> enemies = parent->find_children("Enemy*");

  m_group = parent->find_child("Group");
  TypedArray<Node> units = (TypedArray<Node>)m_group->get("Units");

  for (int i = 0; i < enemies.size(); i++) {
    CharacterBody2D* p = Object::cast_to<CharacterBody2D>(enemies[i]);
    m_enemies.push_back(p);
  }

  for (int i = 0; i < units.size(); i++) {
    CharacterBody2D* p = Object::cast_to<CharacterBody2D>(units[i]);
    m_units.push_back(p);
  }

  for (uint32_t i = 0; i < m_enemies.size(); i++) {
    UnitBlackBoard board{ .unit_id = i, .target_unit_id = NULL_ENTITY };
    m_boards.push_back(board);
  }
}

BehaviourLib::Status
BehaviourManager::ExecuteNode(BehaviourLib::Node node)
{
  switch (node.type) {
    case BehaviourLib::NodeType::Action:
      return m_actionNodes[node.id].Execute();
    case BehaviourLib::NodeType::Sequence:
      BehaviourLib::SequqenceNode& seqNode = m_sequenceNodes[node.id];
      for (auto& child : seqNode.children) {
        BehaviourLib::Status status = ExecuteNode(child);
        if (status == BehaviourLib::Status::FAILED)
          return status;
      }
      return BehaviourLib::Status::SUCCESS;
  }
}

void
BehaviourManager::_process(double delta)
{
  for (auto& board : m_boards) {
    if (board.target_unit_id == NULL_ENTITY) {
      board.target_unit_id = FindTarget(board.unit_id);
    }
  }
}

void
BehaviourManager::_physics_process(double delta)
{
  for (auto& board : m_boards) {
    if (board.target_unit_id == NULL_ENTITY)
      continue;

    CharacterBody2D* enemy = m_enemies[board.unit_id];
    const CharacterBody2D* target_unit = m_units[board.target_unit_id];
    const Vector2 direction =
      enemy->get_position().direction_to(target_unit->get_position());
    enemy->set_velocity(direction * 10.0f);
    enemy->move_and_slide();
  }
}

EntityId
BehaviourManager::FindTarget(EntityId enemy_id) const
{
  CharacterBody2D* enemy = m_enemies[enemy_id];
  Vector2 enemy_pos = enemy->get_position();

  float lowestDistance = std::numeric_limits<float>::max();
  EntityId closestEntity = NULL_ENTITY;
  for (int i = 0; i < m_units.size(); i++) {
    float distance = m_units[i]->get_position().distance_to(enemy_pos);
    if (distance < lowestDistance) {
      closestEntity = EntityId(i);
      lowestDistance = distance;
    }
  }

  UtilityFunctions::print(
    "Enemy: ", enemy_id, " ; Closest Unit: ", closestEntity);
  return closestEntity;
}
