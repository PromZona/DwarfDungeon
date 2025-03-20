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
#include <unordered_map>

namespace BehaviourLib {

class Game : public godot::Node
{
  GDCLASS(Game, godot::Node)

protected:
  static void _bind_methods();

public:
  enum class Levels
  {
    Arena = 0,
    Shop = 1,
    Menu = 2
  };

  std::unordered_map<Levels, godot::String> LevelPathTable{
    { Levels::Arena, "res://scenes/Arena.tscn" },
    { Levels::Shop, "res://scenes/Shop.tscn" },
    { Levels::Menu, "res://scenes/MainMenu.tscn" },
  };

  Game() { godot::UtilityFunctions::print("Game: Constructor"); }
  ~Game() override = default;

  bool IsSceneLoading = false;

  void RegisterManagers();
  void PostRegisterManagers();
  void PreGameStart();

  void LoadScene(Levels level);
  void StartGame();
  void EnterShop();
  void EnterArena();

  void ProcessArena();
  void ProcessShop();

  void _ready() override;
  void _process(double delta) override;
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
