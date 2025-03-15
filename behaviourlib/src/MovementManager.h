#pragma once

#include <GameData.h>
#include <godot_cpp/templates/vector.hpp>

namespace BehaviourLib {

class Game;

class MovementManager
{
public:
  void Update();
  GameData* gameData;
};

} // namespace BehaviorLib
