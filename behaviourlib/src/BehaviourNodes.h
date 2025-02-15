#pragma once

#include <algorithm>
#include <cstdint>
#include <functional>
#include <vector>

// Action Node
// Sequence Node
// Selector Node
namespace BehaviourLib {

using NodeId = int16_t;
const NodeId EMPTY_NODE_ID = -1;

enum class NodeType
{
  Action,
  Sequence,
  Selector
};

enum class Status
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
  std::function<Status()> Execute;
};

struct Tree
{
  BehaviourLib::NodeId root;
  BehaviourLib::Node* nodes;
  int16_t nodesCount;
};

}; // namespace BehaviourLib
