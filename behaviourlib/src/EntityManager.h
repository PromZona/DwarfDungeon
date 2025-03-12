#pragma once

#include "enemy.h"

#include <godot_cpp/classes/character_body2d.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/templates/vector.hpp>
#include <godot_cpp/variant/array.hpp>

namespace BehaviourLib {

using EntityId = uint64_t;
const EntityId NULL_ENTITY =
  1000; // TODO: Fix to some adequate way of identifying "No-unit" index

class EntityManager : public godot::Node
{
  GDCLASS(EntityManager, godot::Node)
protected:
  static void _bind_methods();

public:
  EntityManager();
  ~EntityManager() override;

  EntityId AddEnemy();
  [[nodiscard]] Enemy* GetEnemy(EntityId id) const;
  bool DeleteEnemy(EntityId id);
  godot::Vector<EntityId>& GetActiveEnemies();
  godot::Vector<Enemy*>& GetAllEnemies();
  godot::Vector<godot::CharacterBody2D*>& GetPlayerUnits();

  void ActivatePlayerUnits();

  void _ready() override;
  void _process(double delta) override;

private:
  godot::Ref<godot::PackedScene> m_enemyScene;
  godot::Ref<godot::PackedScene> m_playerUnitScene;
  godot::Ref<godot::PackedScene> m_groupScene;

  godot::Vector<Enemy*> m_enemies;
  godot::Vector<EntityId> m_ActiveEnemies;
  godot::Vector<godot::CharacterBody2D*> m_playerUnits;

  godot::Vector<EntityId> m_AvailableEnemyIDs;

  godot::Node2D* m_Group;

  size_t m_enemiesCount;
  const uint16_t m_MaxEnemyCount = 100;
};

} // namespace BehaviourLib
