#include "Goals/RestGoal.h"

UGOAPRestGoal::UGOAPRestGoal()
{
	Priority = 8.0f;

	DesiredState.Bools.Add("IsExhausted", false);
}

FString UGOAPRestGoal::GetGoalName_Implementation() const
{
	return GetClass()->GetName();
}

bool UGOAPRestGoal::IsRelevant_Implementation(const FGOAPWorldState& WorldState) const
{
	const bool* Exhausted = WorldState.Bools.Find("IsExhausted");
	return Exhausted && *Exhausted == true;
}
