#include "MovementManager.h"
#include "BehaviourManager.h"

namespace BehaviourLib {

void
MovementManager::RegisterDependencies(EntityManager* entityManager,
                                      BehaviourManager* behaviourManager)
{
  m_EntityManager = entityManager;
  m_BehaviourManager = behaviourManager;
}

void
MovementManager::Update()
{
  const auto& playerUnits = m_EntityManager->GetPlayerUnits();
  const auto& brainBoards = m_BehaviourManager->m_boards;

  for (EntityId id : m_MovingEntities) {
    const UnitBlackBoard& board = brainBoards[id];

    if (board.target_unit_id == NULL_ENTITY) {
      continue;
    }

    godot::CharacterBody2D* enemy = m_EntityManager->GetEnemy(id);
    const godot::CharacterBody2D* target_unit =
      playerUnits[board.target_unit_id];
    const godot::Vector2 direction =
      enemy->get_position().direction_to(target_unit->get_position());
    enemy->set_velocity(direction * 10.0f);
    enemy->move_and_slide();
  }
}

} // namespace BehaviourLib
