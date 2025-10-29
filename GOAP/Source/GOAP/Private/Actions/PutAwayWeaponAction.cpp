#include "Actions/PutAwayWeaponAction.h"
#include "GOAPAgent.h"
#include "GOAPAnimInstance.h"

UGOAPPutAwayWeaponAction::UGOAPPutAwayWeaponAction()
{
	Cost = 3.0f;

	// Preconditions: Has a weapon to put away, enemy is not in sight
	Preconditions.Add("HasWeapon", true);
	Preconditions.Add("EnemyVisible", false);

	//Effects: Weapon is put away
	Effects.Add("HasWeapon", false);
}

void UGOAPPutAwayWeaponAction::Execute_Implementation(AGOAPAgent* Agent)
{
	Finish(Agent, true);
}
