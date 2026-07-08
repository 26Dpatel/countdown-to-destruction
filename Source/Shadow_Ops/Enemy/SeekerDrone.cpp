#include "SeekerDrone.h"
#include "SeekerAIController.h"
#include "Interfaces/Damageable.h"

ASeekerDrone::ASeekerDrone()
{
	PrimaryActorTick.bCanEverTick = true;
	
	AIControllerClass = ASeekerAIController::StaticClass();
}

void ASeekerDrone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CheckContactDamage();
}

void ASeekerDrone::CheckContactDamage()
{
	// check if energy time has passed to damage
	float CurrentTime{ static_cast<float>(GetWorld()->GetTimeSeconds()) };
	if (CurrentTime - LastContactDamageTime < ContactCooldown)
	{
		return;
	}
	
	// get all overlapping actors
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors);
	
	// apply damage
	for (AActor* Actor : OverlappingActors)
	{
		IDamageable* Damageable{ Cast<IDamageable>(Actor) };
		if (Damageable && !Cast<AEnemyBase>(Actor))
		{
			Damageable->ApplyDamage(ContactDamage, this);
			LastContactDamageTime = CurrentTime;
			break;
		}
	}
}