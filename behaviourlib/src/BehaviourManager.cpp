#include "BehaviourManager.h"
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/math.hpp>
#include <godot_cpp/variant/typed_array.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/variant.hpp>

using namespace godot;

void BehaviourManager::_bind_methods() {}

BehaviourManager::BehaviourManager() { time_passed = 0.0; }

BehaviourManager::~BehaviourManager() {}

void BehaviourManager::_ready() {
  Node *parent = get_parent();
  TypedArray<Node> enemies = parent->find_children("Enemy*");
  m_enemies = enemies;
}

void BehaviourManager::_process(double delta) {

  time_passed += delta;
  for (int i = 0; i < m_enemies.size(); i++) {
    m_enemies[i].set("skew", Math::sin(time_passed) * 10);
  }
}
