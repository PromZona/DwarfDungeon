#pragma once

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/object.hpp>
namespace BehaviourLib {

class Game;

class UIManager : public godot::Object
{
  GDCLASS(UIManager, godot::Object)
protected:
  static void _bind_methods();

public:
  UIManager() = default;

  void UpdateUiBindings(const godot::Node* rootStaticScene);

  // Main Menu
  void PlayButtonHandle();
  void OptionsButtonHandle();
  void ExitButtonHandle();

  void DrawDebugInfo();

  void Update();

  Game* game;
};

} // namespace BehaviourLib
