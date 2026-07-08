#include "SeekerDrone.h"
#include "Interfaces/Damageable.h"
#include "Kismet/GameplayStatics.h"

ASeekerDrone::ASeekerDrone()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ASeekerDrone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Move toward the player
	APawn* Player = UGameplayStatics::GetPlayerPawn(this, 0);
	if (Player)
	{
		FVector Direction = (Player->GetActorLocation() - GetActorLocation()).GetSafeNormal();
		AddMovementInput(Direction);
	}

	CheckContactDamage();
}

void ASeekerDrone::CheckContactDamage()
{
	float CurrentTime = GetWorld()->GetTimeSeconds();
	if (CurrentTime - LastContactDamageTime < ContactCooldown)
		return;

	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors);

	for (AActor* Actor : OverlappingActors)
	{
		// Don't damage other enemies
		if (Cast<AEnemyBase>(Actor))
			continue;

		// Check interface properly
		if (Actor->GetClass()->ImplementsInterface(UDamageable::StaticClass()))
		{
			IDamageable::Execute_ApplyDamage(Actor, ContactDamage, this);
			LastContactDamageTime = CurrentTime;
			break;
		}
	}
}
