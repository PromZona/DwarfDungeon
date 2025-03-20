#include "BehaviourManager.h"
#include "BehaviourActions.h"
#include "BehaviourNodes.h"
#include "EntityManager.h"
#include "Game.h"
#include "GameData.h"
#include "PerformanceLogger.h"

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

namespace BehaviourLib {
using namespace godot;

BehaviourManager::BehaviourManager()
{

  UtilityFunctions::print("Behaviour Manager Constructor");
  RegisterActionTable();
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

void
BehaviourManager::PreGameStart()
{
  size_t enemiesCount = EntitiesData::MaxEnemyCount;
  for (size_t i = 0; i < enemiesCount; i++) {
    UnitBlackBoard board{ .timestamp = {},
                          .unit_id = i,
                          .target_unit_id = NULL_ENTITY,
                          .isWaiting = false,
                          .isAttacking = false };
    game->Data.Ai.boards[i] = board;
  }

  LoadAiTree(std::string("res://assets/ai/enemy_ai.txt"));
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
  AiData& ai = game->Data.Ai;
  ExecutionContext& context = ai.executionContext[entityId];

  if (context.stack.empty()) {
    context.stack.push_back({ ai.tree.root, 0, Status::FAILED });
  }

  while (!context.stack.empty()) {
    ExecutionFrame& frame = context.stack.back();
    BehaviourLib::Node& node = ai.tree.nodes[frame.nodeId];

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
          context.stack.push_back(
            { node.children[frame.childIndex], 0, Status::FAILED });
          frame.childIndex++;
        }
        break;
      }
      case BehaviourLib::NodeType::Selector: {
        break;
      }
      case BehaviourLib::NodeType::Action: {
        BehaviourLib::Status status = node.Execute(*game, ai.boards[entityId]);
        if (status == BehaviourLib::Status::RUNNING) {
          return status;
        }

        context.stack.pop_back();
        context.stack.back().lastChildStatus = status;

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
  String dataText = file->get_as_text();
  file->close();

  std::string cool_string{ dataText.utf8() };

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

  NodeId nodesCount = 0;
  std::string rootIdString = {};
  NodeId rootId = BehaviourLib::EMPTY_NODE_ID;
  while (std::getline(dataStream, line)) {
    if (line.empty() || line[0] == '[') {
      continue;
    }

    std::istringstream lineStream(line);
    std::string id;
    std::string type;
    std::string data;

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

  BehaviourLib::Tree& tree = game->Data.Ai.tree;

  tree.nodesCount = nodesCount;
  tree.root = rootId;
  tree.nodes = new BehaviourLib::Node[nodesCount];

  for (BehaviourLib::NodeId i = 0; i < nodesCount; i++) {
    const TempNodeData& tempData = nodes[i];
    BehaviourLib::Node* node = &tree.nodes[i];

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
BehaviourManager::Update(double delta)
{
  PERF("BehaviourManager Update")
  auto& enemies = game->Data.Entities.activeEnemies;
  for (EntityId id : enemies) {
    BehaviourLib::Status status = ExecuteNode(id);

    // TODO: Do something with status? (this line is to disable warning)
    (void)status;
  }
}

} // namespace BehaviourLib
