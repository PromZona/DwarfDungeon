#pragma once

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

struct Node
{
  NodeId id;
  NodeType type;
};

enum class Status
{
  FAILED,
  RUNNING,
  SUCCESS
};

struct ActionNode
{
  NodeId id;
  std::function<Status()> Execute;
};

struct SequqenceNode
{
  NodeId id;
  std::vector<Node> children;
};

struct SelectorNode
{
  NodeId id;
  std::vector<Node> children;
};

}; // namespace BehaviourLib
