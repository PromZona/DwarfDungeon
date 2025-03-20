#pragma once

#include "EntityManager.h"
#include <godot_cpp/classes/node.hpp>
namespace BehaviourLib {

class SpawnManager : public godot::Node
{
  GDCLASS(SpawnManager, godot::Node)
protected:
  static void _bind_methods();

public:
  SpawnManager();

  void _ready() override;
  void _process(double delta) override;

  void InitializeSpawnPoints();
  void Spawn();

  Game* game;
};

} // namespace BehaviourLib
