#include "Enemy.h"

#include <godot_cpp/classes/animation_mixer.hpp>
#include <godot_cpp/classes/animation_player.hpp>
#include <godot_cpp/classes/character_body2d.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/shape_cast2d.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

namespace BehaviourLib {
using namespace godot;

EnemyView::EnemyView()
{
  if (Engine::get_singleton()->is_editor_hint()) {
    set_process_mode(Node::ProcessMode::PROCESS_MODE_DISABLED);
  }
}

void
EnemyView::_bind_methods()
{
  ClassDB::bind_method(D_METHOD("ReceiveDamage", "damage"),
                       &EnemyView::ReceiveDamage);

  ClassDB::bind_method(D_METHOD("ApplyDamage"), &EnemyView::ApplyDamage);
}

void
EnemyView::_ready()
{
  if (Engine::get_singleton()->is_editor_hint()) {
    return;
  }

  HealthBar = get_node<TextureProgressBar>("HealthBar");
  AnimPlayer = get_node<AnimationPlayer>("AnimationPlayer");
  AttackAreaHandler = get_node<Node2D>("AttackAreaHandler");

  CurrentHealth = MaxHealth;
  HealthBar->set_max(MaxHealth);
  HealthBar->set_value(CurrentHealth);
}

void
EnemyView::ReceiveDamage(float damage)
{
  CurrentHealth -= damage;
  HealthBar->set_value(CurrentHealth);

  if (CurrentHealth <= 0.0f) {
    IsDead = true;
    return;
  }

  AnimPlayer->play("receive_damage");
}

void
EnemyView::Attack(Vector2 direction)
{
  AttackDirection = direction;
  AttackAreaHandler->look_at(direction);
  AnimPlayer->play("Attack");
}

void
EnemyView::ApplyDamage()
{
  ShapeCast2D* attackArea =
    AttackAreaHandler->get_node<ShapeCast2D>("AttackArea");

  attackArea->force_shapecast_update();
  for (int i = 0; i < attackArea->get_collision_count(); i++) {
    Object* obj = attackArea->get_collider(i);
    CharacterBody2D* charachter = Object::cast_to<CharacterBody2D>(obj);
    if (charachter != nullptr) {
      UtilityFunctions::print(
        "Attack in radius: ", this->get_name(), "->", charachter->get_name());
    }
  }
}

bool
EnemyView::IsAttacking()
{
  return AnimPlayer->is_playing();
}
} // namespace BehaviourLib
