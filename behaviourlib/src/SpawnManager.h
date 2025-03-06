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

  void RegisterDependencies(EntityManager* manager);
  void Spawn();

  int m_spawnCountTarget = 3;
  int m_currentSpawnCount = 0;
  EntityManager* m_EntityManger;
  bool m_isActive = false;
};

} // namespace BehaviourLib
