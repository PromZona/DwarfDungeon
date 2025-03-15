#include "MovementManager.h"
#include "BehaviourManager.h"

namespace BehaviourLib {

void
MovementManager::Update()
{
  const auto& playerUnits = gameData->Entities.playerUnitViews;
  const auto& brainBoards = gameData->Ai.boards;
  const auto& movingEnemies = gameData->Entities.movingEnemies;
  const auto& enemyViews = gameData->Entities.enemyViews;


  for (EntityId id : movingEnemies) {
    const UnitBlackBoard& board = brainBoards[id];

    if (board.target_unit_id == NULL_ENTITY) {
      continue;
    }

    EnemyView* enemy = enemyViews[id];
    const godot::CharacterBody2D* target_unit =
      playerUnits[board.target_unit_id];
    const godot::Vector2 direction =
      enemy->get_position().direction_to(target_unit->get_position());
    enemy->set_velocity(direction * 10.0f);
    enemy->move_and_slide();
  }
}

} // namespace BehaviourLib
