#include "enemy.h"

#include <godot_cpp/classes/animation_mixer.hpp>
#include <godot_cpp/classes/animation_player.hpp>
#include <godot_cpp/classes/engine.hpp>

using namespace godot;

Enemy::Enemy()
  : MaxHealth(100.0f)
  , CurrentHealth(MaxHealth)
  , AnimPlayer(nullptr)
  , HealthBar(nullptr)
{
  if (Engine::get_singleton()->is_editor_hint())
    set_process_mode(Node::ProcessMode::PROCESS_MODE_DISABLED);
}

Enemy::~Enemy() {}

void
Enemy::_bind_methods()
{
  ClassDB::bind_method(D_METHOD("ReceiveDamage", "damage"),
                       &Enemy::ReceiveDamage);
}

void
Enemy::_ready()
{
  if (Engine::get_singleton()->is_editor_hint())
    return;

  HealthBar = get_node<TextureProgressBar>("HealthBar");
  AnimPlayer = get_node<AnimationPlayer>("AnimationPlayer");

  CurrentHealth = MaxHealth;
  HealthBar->set_max(MaxHealth);
  HealthBar->set_value(CurrentHealth);
}

void
Enemy::ReceiveDamage(float damage)
{
  CurrentHealth -= damage;
  HealthBar->set_value(CurrentHealth);
  AnimPlayer->play("receive_damage");
}
