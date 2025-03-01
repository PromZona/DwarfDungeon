#pragma once

#include "EntityManager.h"
#include <godot_cpp/templates/vector.hpp>

namespace BehaviourLib {

class BehaviourManager;

class MovementManager
{
public:
  MovementManager() = default;

  void RegisterDependencies(EntityManager* entityManager,
                            BehaviourManager* behaviourManager);
  void Update();

  EntityManager* m_EntityManager;
  BehaviourManager* m_BehaviourManager;
  godot::Vector<EntityId> m_MovingEntities;
};

} // namespace BehaviorLib
