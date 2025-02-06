#pragma once

#include <godot_cpp/classes/animation_player.hpp>
#include <godot_cpp/classes/character_body2d.hpp>
#include <godot_cpp/classes/texture_progress_bar.hpp>
#include <godot_cpp/classes/wrapped.hpp>

namespace godot {

class Enemy : public CharacterBody2D
{
  GDCLASS(Enemy, CharacterBody2D)
public:
  float MaxHealth = 100.0f;
  float CurrentHealth = MaxHealth;

  Enemy();
  ~Enemy();

  void _ready() override;

  void ReceiveDamage(float damage);

protected:
  static void _bind_methods();

private:
  TextureProgressBar* HealthBar = nullptr;
  AnimationPlayer* AnimPlayer = nullptr;
};
}
