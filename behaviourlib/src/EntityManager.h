#pragma once

#include "GameData.h"

#include <godot_cpp/classes/character_body2d.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/templates/vector.hpp>
#include <godot_cpp/variant/array.hpp>

namespace BehaviourLib {

class Game;

class EntityManager : public godot::Node
{
  GDCLASS(EntityManager, godot::Node)
protected:
  static void _bind_methods();

public:
  EntityManager();
  ~EntityManager() override;

  EntityId AddEnemy();
  [[nodiscard]] EnemyView* GetEnemyView(EntityId id) const;
  bool DeleteEnemy(EntityId id);

  void InitializeEntites();
  void ActivatePlayerUnits();

  void _ready() override;
  void _process(double delta) override;

  Game* game;
};

} // namespace BehaviourLib
