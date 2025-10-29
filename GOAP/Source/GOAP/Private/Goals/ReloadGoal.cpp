#include "Goals/ReloadGoal.h"

UGOAPReloadGoal::UGOAPReloadGoal()
{
	Priority = 9.0f;

	DesiredState.Bools.Add("HasBullets", true);
}

FString UGOAPReloadGoal::GetGoalName_Implementation() const
{
	return GetClass()->GetName();
}

bool UGOAPReloadGoal::IsRelevant_Implementation(const FGOAPWorldState& WorldState) const
{
	const bool* Bullets = WorldState.Bools.Find("HasBullets");
	return Bullets && *Bullets == false;
}
