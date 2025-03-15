#pragma once

#include "BehaviourManager.h"
#include "EntityManager.h"
#include "GameData.h"
#include "MovementManager.h"
#include "SpawnManager.h"
#include "UIManager.h"
#include "godot_cpp/variant/array.hpp"
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

  bool IsSceneLoading = false;

  void RegisterManagers();
  void PostRegisterManagers();
  void PreGameStart();

  void LoadScene(godot::String);

  void StartGame();

  void _ready() override;
  void _physics_process(double delta) override;
  void _input(const godot::Ref<godot::InputEvent>& event) override;

  GameData Data;

  EntityManager* EntityManager;
  MovementManager* MovementManager;
  BehaviourManager* BehaviourManager;
  SpawnManager* SpawnManager;
  UIManager* UIManager;
};

} // namespace BehaviourLib
