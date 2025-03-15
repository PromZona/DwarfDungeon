#pragma once

#include <godot_cpp/classes/animation_player.hpp>
#include <godot_cpp/classes/character_body2d.hpp>
#include <godot_cpp/classes/texture_progress_bar.hpp>
#include <godot_cpp/classes/wrapped.hpp>

namespace BehaviourLib {

class EnemyView : public godot::CharacterBody2D
{
  GDCLASS(EnemyView, godot::CharacterBody2D)
public:
  float MaxHealth = 100.0F;
  float CurrentHealth = MaxHealth;
  float AttackRadius = 32.0F + 8.0F;
  bool IsDead = false;

  EnemyView();

  void _ready() override;

  void ReceiveDamage(float damage);
  void Attack(godot::Vector2 direction);
  void ApplyDamage();
  bool IsAttacking();

protected:
  static void _bind_methods();

private:
  godot::TextureProgressBar* HealthBar = nullptr;
  godot::AnimationPlayer* AnimPlayer = nullptr;
  Node2D* AttackAreaHandler = nullptr;

  godot::Vector2 AttackDirection;
};
}
