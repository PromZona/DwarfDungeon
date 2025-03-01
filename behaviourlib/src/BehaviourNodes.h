#pragma once

#include <cstdint>
#include <functional>
#include <vector>

namespace BehaviourLib {

class Game;
class BehaviourManager;
struct UnitBlackBoard;

using NodeId = uint16_t;
const NodeId EMPTY_NODE_ID = 10000; // TODO: Fix this to somehing adequate

enum class NodeType : uint8_t
{
  Action,
  Sequence,
  Selector
};

enum class Status : uint8_t
{
  FAILED,
  RUNNING,
  SUCCESS
};

struct NodeRef
{
  NodeId id;
  NodeType type;
};

struct Node
{
  NodeId id;
  NodeType type;
  std::vector<NodeId> children;
  std::function<Status(Game&, UnitBlackBoard&)> Execute;
};

struct Tree
{
  BehaviourLib::NodeId root;
  BehaviourLib::Node* nodes;
  uint16_t nodesCount;
};

}; // namespace BehaviourLib
