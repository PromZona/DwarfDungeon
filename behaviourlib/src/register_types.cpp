#include "register_types.h"

#include "BehaviourManager.h"
#include "EntityManager.h"
#include "Game.h"
#include "PlayerUnitView.h"
#include "SpawnManager.h"
#include "UIManager.h"
#include "Enemy.h"

#include <gdextension_interface.h>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

using namespace godot;

void
initialize_example_module(ModuleInitializationLevel p_level)
{
  if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
    return;
  }

  GDREGISTER_CLASS(BehaviourLib::Game);
  GDREGISTER_CLASS(BehaviourLib::SpawnManager);
  GDREGISTER_CLASS(BehaviourLib::EntityManager);
  GDREGISTER_CLASS(BehaviourLib::UIManager);
  GDREGISTER_CLASS(BehaviourLib::EnemyView);
  GDREGISTER_CLASS(BehaviourLib::PlayerUnitView);
}

void
uninitialize_example_module(ModuleInitializationLevel p_level)
{
  if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
    return;
  }
}

extern "C"
{
  // Initialization.
  GDExtensionBool GDE_EXPORT
  example_library_init(GDExtensionInterfaceGetProcAddress p_get_proc_address,
                       const GDExtensionClassLibraryPtr p_library,
                       GDExtensionInitialization* r_initialization)
  {
    godot::GDExtensionBinding::InitObject init_obj(
      p_get_proc_address, p_library, r_initialization);

    init_obj.register_initializer(initialize_example_module);
    init_obj.register_terminator(uninitialize_example_module);
    init_obj.set_minimum_library_initialization_level(
      MODULE_INITIALIZATION_LEVEL_SCENE);

    return init_obj.init();
  }
}
