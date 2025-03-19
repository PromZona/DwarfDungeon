#pragma once

#include "godot_cpp/classes/animation_player.hpp"
#include "godot_cpp/classes/area2d.hpp"
#include "godot_cpp/classes/marker2d.hpp"
#include "godot_cpp/classes/shape_cast2d.hpp"
#include <godot_cpp/classes/character_body2d.hpp>

namespace BehaviourLib {

class PlayerUnitView : public godot::CharacterBody2D
{
  GDCLASS(PlayerUnitView, godot::CharacterBody2D)
protected:
  static void _bind_methods();

public:
  PlayerUnitView();

  void _ready() override;
  void _process(double delta) override;
  void _physics_process(double delta) override;
  void _draw() override;

  godot::Vector2 getFormationVelocity();
  godot::Vector2 getGroupVelocity();
  godot::Vector2 getGroupUnitsVelocity();
  godot::Vector2 getFinalVelocity();
  void ApplyDamage();
  void Attack();
  void onGroupMemberCollisionEntered(godot::Area2D* area);
  void onGroupMemberCollisionExited(godot::Area2D* area);

  // Unit modifiable variables
  float Damage = 5.0f;
  float Speed = 60.0f;
  float FormationCloseEnoughDistance = 15.0f;
  float FormationMaxSpeedDistance = 150.0f;
  float FormationMinSpeedCoefficient = 0.3f;
  float FormationMaxSpeedCoefficient = 1.2f;

  // Scene Components
  // DefaultFont = ThemeDB.fallback_font
  godot::Area2D* PersonalSpaceArea;
  godot::ShapeCast2D* AttackArea;
  godot::Area2D* AttackRadius;
  godot::Node2D* AttackAreaHandler;
  godot::AnimationPlayer* AnimPlayer;

  enum class State
  {
    Idle,
    Move,
    Attack
  };

  // Unit scratch data
  State CurrentState = State::Idle;
  godot::Marker2D* Target;
  godot::Vector2 GroupForceDirection;
  godot::Vector<godot::Area2D*> GroupUnitsTooClose;
  float PersonalSpaceRadius;

  // Getters and setters (Thanks godot developers for forcing me to create 2
  // additional functions for each variable)
  void SetTarget(godot::Marker2D* marker);
  godot::Marker2D* GetTarget();
  void SetGroupDirection(godot::Vector2 dir);
};

} // namespace BehaviourLib
