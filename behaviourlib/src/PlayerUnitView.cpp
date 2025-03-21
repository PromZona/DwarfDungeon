#include "PlayerUnitView.h"
#include "Enemy.h"
#include "godot_cpp/classes/animation_player.hpp"
#include "godot_cpp/classes/area2d.hpp"
#include "godot_cpp/classes/circle_shape2d.hpp"
#include "godot_cpp/classes/collision_shape2d.hpp"
#include "godot_cpp/classes/engine.hpp"
#include "godot_cpp/classes/engine_debugger.hpp"
#include "godot_cpp/classes/kinematic_collision2d.hpp"
#include "godot_cpp/classes/object.hpp"
#include "godot_cpp/classes/property_tweener.hpp"
#include "godot_cpp/classes/shape_cast2d.hpp"
#include "godot_cpp/classes/tween.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/core/math.hpp"
#include "godot_cpp/variant/color.hpp"
#include "godot_cpp/variant/node_path.hpp"
#include "godot_cpp/variant/typed_array.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/variant/vector2.hpp"
#include <cfloat>
#include <cmath>

namespace BehaviourLib {

PlayerUnitView::PlayerUnitView() {}

void
PlayerUnitView::_bind_methods()
{
  godot::ClassDB::bind_method(godot::D_METHOD("set_target", "p_target"),
                              &PlayerUnitView::SetTarget);

  godot::ClassDB::bind_method(godot::D_METHOD("apply_damage"),
                              &PlayerUnitView::ApplyDamage);

  godot::ClassDB::bind_method(
    godot::D_METHOD("set_group_direction", "p_direction"),
    &PlayerUnitView::SetGroupDirection);
}

void
PlayerUnitView::_ready()
{
  if (godot::Engine::get_singleton()->is_editor_hint()) {
    return;
  }
  PersonalSpaceArea = (godot::Area2D*)find_child("GroupMemberCollision");
  AttackAreaHandler = (godot::Node2D*)find_child("AttackAreaHandler");
  AttackArea = (godot::ShapeCast2D*)AttackAreaHandler->find_child("AttackArea");
  AttackRadius = (godot::Area2D*)AttackAreaHandler->find_child("AttackRadius");
  AnimPlayer = (godot::AnimationPlayer*)find_child("AnimationPlayer");

  godot::CollisionShape2D* colShape =
    godot::Object::cast_to<godot::CollisionShape2D>(
      PersonalSpaceArea->get_child(0));
  PersonalSpaceRadius =
    godot::Object::cast_to<godot::CircleShape2D>(colShape->get_shape().ptr())
      ->get_radius();
}

void
PlayerUnitView::_process(double delta)
{
  if (godot::Engine::get_singleton()->is_editor_hint()) {
    return;
  }
  queue_redraw();

  godot::TypedArray<Node2D> enemies = AttackRadius->get_overlapping_bodies();
  float minDistance = FLT_MAX;
  godot::Node2D* closestEnemy = nullptr;

  for (int i = 0; i < enemies.size(); i++) {
    godot::Node2D* enemy = godot::Object::cast_to<godot::Node2D>(enemies[i]);
    if (enemy->get_class().nocasecmp_to("EnemyView") != 0) {
      continue;
    }
    float dist =
      get_global_position().distance_to(enemy->get_global_position());
    if (dist < minDistance) {
      minDistance = dist;
      closestEnemy = enemy;
    }
  }

  // If enemy close, check if can attack it
  if (closestEnemy != nullptr) {
    AttackAreaHandler->look_at(closestEnemy->get_global_position());
    AttackArea->force_shapecast_update();
    godot::Array collisions = AttackArea->get_collision_result();
    if (collisions.size() > 0) {
      Attack();
    }
  }
}
void
PlayerUnitView::_physics_process(double delta)
{
  if (godot::Engine::get_singleton()->is_editor_hint()) {
    return;
  }
  godot::Vector2 vel = getFinalVelocity();
  godot::Ref<godot::KinematicCollision2D> col = move_and_collide(vel * delta);
  if (col.is_valid()) {
    KnockbackTime = KnockbackMaxTime;
  }

  if (KnockbackTime > 0.0f) {
    KnockbackTime -= KnockbackDecaySpeed;
    // godot::UtilityFunctions::print(
    //   "[", get_name(), "]KBtime: ", KnockBackForceTime);
  }
}
void
PlayerUnitView::_draw()
{
  // draw_line({ 0, 0 }, getFormationVelocity(), godot::Color::named("Red"));
  // draw_line({ 0, 0 }, getGroupUnitsVelocity(), godot::Color::named("Green"));
  // draw_line({ 0, 0 }, getGroupVelocity(), godot::Color::named("Magenta"));
  // draw_line({ 0, 0 }, getFinalVelocity(), godot::Color::named("Black"));

  // GODOT GDSCRIPT VERSTION
  // draw_line(Vector2.ZERO, formation_velocity(), Color.RED)
  // draw_line(Vector2.ZERO, group_units_velocity(), Color.GREEN)
  // draw_line(Vector2.ZERO, group_velocity(), Color.MAGENTA)
  // draw_string(DefaultFont, Vector2(-20, 35), "dist: %.1f" %
  // global_position.distance_to(Target.global_position),
  // HORIZONTAL_ALIGNMENT_FILL, -1, 9, Color.WHITE) draw_string(DefaultFont,
  // Vector2(-20, 48), "f: %.1v" % formation_velocity(),
  // HORIZONTAL_ALIGNMENT_FILL, -1, 9, Color.WHITE)
}

godot::Vector2
PlayerUnitView::getFormationVelocity()
{
  godot::Vector2 targetPosition = Target->get_global_position();
  godot::Vector2 direction = get_global_position().direction_to(targetPosition);
  float distance = get_global_position().distance_to(targetPosition);

  // If close "enough", remove jittering around
  if (distance < FormationCloseEnoughDistance) {
    return godot::Vector2{ 0, 0 };
  }

  // linear [0; FormationMaxSpeedDistance] -> [0; 1]
  float distanceNormalized =
    godot::Math::smoothstep(0, FormationMaxSpeedDistance, distance);

  // clamp  [0; 1] -> [FormationMinSpeedCoefficient; 1]
  distanceNormalized = godot::Math::clamp<float>(
    distanceNormalized, FormationMinSpeedCoefficient, 1.0);

  // [0.1; 1] -> [0.0; Speed]
  float magnitude = godot::Math::lerp(
    0.0f, Speed * FormationMaxSpeedCoefficient, distanceNormalized);
  return direction * magnitude;
}

godot::Vector2
PlayerUnitView::getGroupVelocity()
{
  return GroupForceDirection * Speed;
}

godot::Vector2
PlayerUnitView::getGroupUnitsVelocity()
{
  godot::Vector2 result = godot::Vector2{ 0, 0 };
  for (auto& area : GroupUnitsTooClose) {
    godot::Vector2 oppositeDir =
      -1 * PersonalSpaceArea->get_global_position().direction_to(
             area->get_global_position());

    float distanceToArea = PersonalSpaceArea->get_global_position().distance_to(
      area->get_global_position());

    float distanceNormalized =
      1.0f -
      godot::Math::smoothstep(0.0f, 2.0f * PersonalSpaceRadius, distanceToArea);

    float magnitude = godot::Math::lerp(0.0f, Speed * 0.2f, distanceNormalized);
    result += oppositeDir * magnitude;
  }
  return result;
}

float
easeOutCubic(float x)
{
  return 1 - godot::Math::pow(1 - x, 3);
}

godot::Vector2
PlayerUnitView::getKnockBackVelocity()
{
  godot::Vector2 result = { 0, 0 };

  if (KnockbackTime <= 0.0f) {
    return result;
  }

  godot::Node2D* group = get_parent()->get_node<godot::Node2D>("Group");
  godot::Vector2 groupCenter = group->call("group_center");
  godot::Vector2 dirToGroupCenter =
    get_global_position().direction_to(groupCenter);
  result = dirToGroupCenter *
           (KnockbackForce * easeOutCubic(godot::Math::smoothstep(
                               0.0f, KnockbackMaxTime, KnockbackTime)));
  return result;
}

godot::Vector2
PlayerUnitView::getFinalVelocity()
{
  if (KnockbackTime > 0.0f) {
    return getKnockBackVelocity();
  }

  // group force
  godot::Vector2 result = getGroupVelocity();

  // formation force
  result += getFormationVelocity();

  // unit force
  result += getGroupUnitsVelocity();

  // Knockback force
  // result += getKnockBackVelocity();

  return result;
}

void
PlayerUnitView::ApplyDamage()
{
  AttackArea->force_shapecast_update();
  godot::Array collisions = AttackArea->get_collision_result();
  for (int i = 0; i < collisions.size(); i++) {
    const auto& col = collisions[i];
    EnemyView* enemy = godot::Object::cast_to<EnemyView>(col.get("collider"));
    if (enemy != nullptr) {
      enemy->ReceiveDamage(Damage);
    }
  }
}
void
PlayerUnitView::Attack()
{
  AnimPlayer->play("Attack");
}
void
PlayerUnitView::onGroupMemberCollisionEntered(godot::Area2D* area)
{
  GroupUnitsTooClose.append(area);
}
void
PlayerUnitView::onGroupMemberCollisionExited(godot::Area2D* area)
{
  GroupUnitsTooClose.erase(area);
}

void
PlayerUnitView::SetTarget(godot::Marker2D* marker)
{
  Target = marker;
}

godot::Marker2D*
PlayerUnitView::GetTarget()
{
  return Target;
}

void
PlayerUnitView::SetGroupDirection(godot::Vector2 dir)
{
  GroupForceDirection = dir;
}
} // namespace BehaviourLib
