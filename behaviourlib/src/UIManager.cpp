#include "UIManager.h"
#include "Game.h"
#include "PerformanceLogger.h"
#include "godot_cpp/classes/canvas_item.hpp"
#include "godot_cpp/classes/engine_debugger.hpp"
#include "godot_cpp/classes/label.hpp"
#include <cassert>
#include <godot_cpp/classes/button.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/wrapped.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

namespace BehaviourLib {

void
UIManager::_bind_methods()
{
  godot::ClassDB::bind_method(godot::D_METHOD("PlayButtonHandle"),
                              &UIManager::PlayButtonHandle);
  godot::ClassDB::bind_method(godot::D_METHOD("OptionsButtonHandle"),
                              &UIManager::OptionsButtonHandle);
  godot::ClassDB::bind_method(godot::D_METHOD("ExitButtonHandle"),
                              &UIManager::ExitButtonHandle);
}

void
UIManager::PlayButtonHandle()
{
  game->StartGame();
}

void
UIManager::OptionsButtonHandle()
{
  godot::UtilityFunctions::print("UIManager: Options Button Handle");
}

void
UIManager::ExitButtonHandle()
{
  godot::UtilityFunctions::print("UIManager: Exit Button Handle");
  game->get_parent()->propagate_notification(
    godot::Node::NOTIFICATION_WM_CLOSE_REQUEST);
  game->get_tree()->quit(0);
}

void
UIManager::UpdateUiBindings(const godot::Node* rootStaticScene)
{
  godot::UtilityFunctions::print("UIManger: Updating UI Bindings for ",
                                 rootStaticScene->get_name());

  if (rootStaticScene->get_name() == godot::String("MainMenu")) {

    godot::Button* btn = nullptr;

    btn = (godot::Button*)rootStaticScene->find_child("StartButton");
    btn->connect("pressed", godot::Callable(this, "PlayButtonHandle"));

    btn = (godot::Button*)rootStaticScene->find_child("OptionsButton");
    btn->connect("pressed", godot::Callable(this, "OptionsButtonHandle"));

    btn = (godot::Button*)rootStaticScene->find_child("ExitButton");
    btn->connect("pressed", godot::Callable(this, "ExitButtonHandle"));
    return;
  }

  if (rootStaticScene->get_name() == godot::String("Arena")) {
    return;
  }
}

void
UIManager::DrawDebugInfo()
{
  godot::Label* debugTextLabel =
    game->get_node<godot::Label>("Camera2D/Debug/Label");

  debugTextLabel->set_text(PerformanceLogger.GetGodotString());
}

godot::Label* killLabel = nullptr;

void
UIManager::Update()
{
  if (game->Data.IsArena) {
    if (killLabel == nullptr) {
      killLabel =
        game->get_node<godot::Label>("StaticEnvironment/Arena/ArenaUI/Control/"
                                     "HBoxContainer/EnemiesKilledCount");
    }
    int64_t killsLeft =
      game->Data.Arena.enemiesKillGoal - game->Data.Arena.enemiesKilled;
    killLabel->set_text(godot::String::num_uint64(killsLeft));
  }
}
} // namespace BehaviourLib
