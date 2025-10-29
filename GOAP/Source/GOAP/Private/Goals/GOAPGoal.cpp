#include "Goals/GOAPGoal.h"

UGOAPGoal::UGOAPGoal()
{
    Priority = 1.0f;
}

// Default name
FString UGOAPGoal::GetGoalName_Implementation() const
{
    return GetClass()->GetName();
}

// Default relevance: always relevant by default
bool UGOAPGoal::IsRelevant_Implementation(const FGOAPWorldState& WorldState) const
{
    return true;
}
