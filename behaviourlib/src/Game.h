#pragma once

#include "BehaviourManager.h"
#include "EntityManager.h"
#include "MovementManager.h"
#include "SpawnManager.h"
#include <godot_cpp/classes/input_event.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

namespace BehaviourLib {
class Game : public godot::Node
{
  GDCLASS(Game, godot::Node)

protected:
  static void _bind_methods();

public:
  Game() { godot::UtilityFunctions::print("Game: Constructor"); }
  ~Game() override = default;

  void RegisterManagers();
  void PostRegisterManagers();
  void PreGameStart();

  void LoadScene(godot::String);

  void _ready() override;
  void _physics_process(double delta) override;
  void _input(const godot::Ref<godot::InputEvent>& event) override;

  EntityManager* EntityManager;
  MovementManager* MovementManager;
  BehaviourManager* BehaviourManager;
  SpawnManager* SpawnManager;
};

} // namespace BehaviourLib
