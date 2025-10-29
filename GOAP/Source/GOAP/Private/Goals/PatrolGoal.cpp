#include "Goals/PatrolGoal.h"

UGOAPPatrolGoal::UGOAPPatrolGoal()
{
	Priority = 1.f;

	DesiredState.Bools.Add("IsPatrolling", true);
}

FString UGOAPPatrolGoal::GetGoalName_Implementation() const
{
	return GetClass()->GetName();
}

bool UGOAPPatrolGoal::IsRelevant_Implementation(const FGOAPWorldState& WorldState) const
{
	const bool* EnemyVisible = WorldState.Bools.Find("EnemyVisible");
	return EnemyVisible && *EnemyVisible == false;
}
