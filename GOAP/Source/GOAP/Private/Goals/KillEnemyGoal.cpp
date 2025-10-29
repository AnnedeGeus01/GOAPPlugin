#include "Goals/KillEnemyGoal.h"

UGOAPKillEnemyGoal::UGOAPKillEnemyGoal()
{
    Priority = 10.0f;

    DesiredState.Bools.Add("EnemyAlive", false);
}

FString UGOAPKillEnemyGoal::GetGoalName_Implementation() const
{
    return GetClass()->GetName();
}

bool UGOAPKillEnemyGoal::IsRelevant_Implementation(const FGOAPWorldState& WorldState) const
{
    const bool* EnemyVisible = WorldState.Bools.Find("EnemyVisible");
    return EnemyVisible && *EnemyVisible == true;
}