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

  void RegisterDependencies(Game* game);
  void UpdateUiBindings(const godot::Node* rootStaticScene);

  // Main Menu
  void PlayButtonHandle();
  void OptionsButtonHandle();
  void ExitButtonHandle();

private:
  Game* m_Game;
};

} // namespace BehaviourLib
