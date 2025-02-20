#include "BehaviourManager.h"
#include "BehaviourActions.h"
#include "BehaviourNodes.h"

#include <cstdint>
#include <cstdlib>
#include <godot_cpp/classes/character_body2d.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/global_constants.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/time.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/math.hpp>
#include <godot_cpp/core/memory.hpp>
#include <godot_cpp/variant/typed_array.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/variant.hpp>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

namespace BehaviourLib
{
using namespace godot;


void
BehaviourManager::_bind_methods()
{
}

BehaviourManager::BehaviourManager()
  : m_enemies()
  , m_units()
  , m_boards()
  , m_executionContext()
  , m_movingEntities()
  , m_group(nullptr)
  , m_tree()
  , m_actionTable()
{
  if (Engine::get_singleton()->is_editor_hint())
    set_process_mode(Node::ProcessMode::PROCESS_MODE_DISABLED);

  RegisterActionTable();
}

BehaviourManager::~BehaviourManager() {}

void
BehaviourManager::_ready()
{
  if (Engine::get_singleton()->is_editor_hint())
    return;
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
    m_executionContext.push_back({ .stack = {} });
  }

  LoadAiTree(std::string("res://assets/ai/enemy_ai.txt"));
}

void
BehaviourManager::RegisterActionTable()
{
  m_actionTable["FindTarget"] = &BehaviourLib::FindTarget;
  m_actionTable["StartMove"] = &BehaviourLib::StartMove;
  m_actionTable["CheckIfArrived"] = &BehaviourLib::CheckIfArrived;
  m_actionTable["Pause"] = &BehaviourLib::Pause;
  m_actionTable["Attack"] = &BehaviourLib::Attack;
}

std::vector<std::string>
SplitString(const std::string& s, char sep = ',')
{
  std::vector<std::string> v;
  std::stringstream ss(s);
  std::string token;
  while (std::getline(ss, token, sep)) {
    v.push_back(token);
  }
  return v;
}

BehaviourLib::Status
BehaviourManager::ExecuteNode(const EntityId entityId)
{
  ExecutionContext& context = m_executionContext[entityId];

  if (context.stack.empty()) {
    context.stack.push_back({ m_tree.root, 0 });
  }

  while (!context.stack.empty()) {
    ExecutionFrame& frame = context.stack.back();
    BehaviourLib::Node& node = m_tree.nodes[frame.nodeId];

    switch (node.type) {
      case BehaviourLib::NodeType::Sequence: {
        bool isPreviousChildFailed =
          frame.childIndex > 0 &&
          frame.lastChildStatus == BehaviourLib::Status::FAILED;
        bool isChildrenLeft = frame.childIndex < node.children.size();

        if (isPreviousChildFailed || (!isChildrenLeft)) {
          context.stack.pop_back();
          if (!context.stack.empty()) {
            context.stack.back().lastChildStatus = frame.lastChildStatus;
          }
        } else {
          context.stack.push_back({ node.children[frame.childIndex], 0 });
          frame.childIndex++;
        }
        break;
      }
      case BehaviourLib::NodeType::Selector: {
        break;
      }
      case BehaviourLib::NodeType::Action: {
        BehaviourLib::Status status = node.Execute(this, m_boards[entityId]);
        if (status == BehaviourLib::Status::RUNNING) {
          return status;
        } else {
          context.stack.pop_back();
          context.stack.back().lastChildStatus = status;
        }
        break;
      }
    }
  }
  return BehaviourLib::Status::SUCCESS;
}

void
BehaviourManager::LoadAiTree(const std::string& filename)
{
  Ref<FileAccess> file = FileAccess::open(filename.c_str(), FileAccess::READ);
  String data = file->get_as_text();
  file->close();

  std::string cool_string{ data.utf8() };

  struct TempNodeData
  {
    std::string id;
    std::string type;
    std::string data;
  };

  std::vector<TempNodeData> nodes{};
  std::unordered_map<std::string, BehaviourLib::NodeId> dataIdToNodeId{};

  std::string line;
  std::istringstream dataStream(cool_string);

  int nodesCount = 0;
  std::string rootIdString = {};
  BehaviourLib::NodeId rootId = BehaviourLib::EMPTY_NODE_ID;
  while (std::getline(dataStream, line)) {
    if (line.empty() || line[0] == '[')
      continue;

    std::istringstream lineStream(line);
    std::string id, type, data;

    lineStream >> id >> type >> data;

    if (id == "root") {
      rootIdString = type;
      continue;
    }

    if (id == rootIdString) {
      rootId = nodesCount; // nodesCount is an index in the final array;
    }
    dataIdToNodeId.insert({ id, nodesCount });
    nodes.push_back(TempNodeData{ .id = id, .type = type, .data = data });
    nodesCount++;
  }

  BehaviourLib::Tree tree{};
  tree.nodesCount = nodesCount;
  tree.root = rootId;
  tree.nodes = new BehaviourLib::Node[nodesCount];
  m_tree = tree;

  for (BehaviourLib::NodeId i = 0; i < nodesCount; i++) {
    const TempNodeData& tempData = nodes[i];
    BehaviourLib::Node* node = &m_tree.nodes[i];

    node->id = i;

    if (tempData.type == "A") {

      node->type = BehaviourLib::NodeType::Action;
      auto func = m_actionTable[tempData.data];
      node->Execute = func;
    } else if (tempData.type == "S") {

      node->type = BehaviourLib::NodeType::Sequence;
      std::vector<std::string> ids = SplitString(tempData.data);
      for (auto const& strid : ids) {
        BehaviourLib::NodeId id = dataIdToNodeId.at(strid);
        node->children.push_back(id);
      }
    } else if (tempData.type == "F") {

      node->type = BehaviourLib::NodeType::Selector;
      std::vector<std::string> ids = SplitString(tempData.data);
      for (auto const& strid : ids) {
        BehaviourLib::NodeId id = dataIdToNodeId.at(strid);
        node->children.push_back(id);
      }
    }
  }
}

void
BehaviourManager::_process(double delta)
{
  for (EntityId id = 0; id < m_enemies.size(); id++) {
    BehaviourLib::Status status = ExecuteNode(id);
  }
}

void
BehaviourManager::_physics_process(double delta)
{
  for (EntityId id : m_movingEntities) {
    UnitBlackBoard& board = m_boards[id];

    if (board.target_unit_id == NULL_ENTITY)
      continue;

    CharacterBody2D* enemy = m_enemies[id];
    const CharacterBody2D* target_unit = m_units[board.target_unit_id];
    const Vector2 direction =
      enemy->get_position().direction_to(target_unit->get_position());
    enemy->set_velocity(direction * 10.0f);
    enemy->move_and_slide();
  }
}
} // namespace BehaviourLib
