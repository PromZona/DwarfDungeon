#pragma once

#include "BehaviourNodes.h"
#include "EntityManager.h"
#include <deque>
#include <godot_cpp/classes/character_body2d.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/sprite2d.hpp>
#include <godot_cpp/variant/typed_array.hpp>
#include <unordered_map>
#include <vector>

namespace BehaviourLib {

class Game;

class BehaviourManager {
public:
  BehaviourManager();

  Game* game;

  // leave it here?
  // it is not game state, technically
  std::unordered_map<
    std::string,
    std::function<BehaviourLib::Status(Game&, UnitBlackBoard&)>>
    m_actionTable;

  void RegisterDependencies(Game* game, EntityManager* entityManager);
  BehaviourLib::Status ExecuteNode(EntityId entityId);
  void LoadAiTree(const std::string& filename);
  void RegisterActionTable();
  void PreGameStart();

  void Update(double delta);
};

} // namespace godot
